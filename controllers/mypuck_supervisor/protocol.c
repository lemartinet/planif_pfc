/*
 * Tolman & Honzik's experiment. 
 * Opens the doors, moves the robot to the start position, ...
 * louis-emmanuel.martinet@upmc.fr
 */
#include "protocol.h"
#include "analysebehavior.h"
#include "mypuck_supervisor.h"
#include <webots/servo.h>
#include <webots/distance_sensor.h>
#include <webots/led.h>
#include <webots/emitter.h>
#include <webots/supervisor.h>
#include <webots/robot.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Webots devices: servos, sensors and emitter
#define NBSENSORS 5
#define NBSERVOS 7
WbDeviceTag sensors[NBSENSORS];
WbDeviceTag servos[NBSERVOS];
WbDeviceTag emitter;
enum servo_name { way1_start = 0, way1_mid = 1, way1_end = 2,
                  way2_start = 3, way2_end = 4, 
                  way3_start = 5, way3_end = 6 };
enum sensor_name {  s_mid = 0, s_end = 1, s_goal = 2, 
                    s_way2 = 3, s_way2_close = 4 };

// output file for protocol observations
FILE * output_protoc = NULL;
enum protocol_type { P1 = 1, P2 = 2, P3 = 3, FREE = 4, BLOCA = 5, BLOCB = 6 };
// variable for way 1 random choice during training period
int nbChoice1 = 0;
int previous_way1 = 0;
// position of servos (c : close, o : open), and actions
enum servo_position { c = -1, o = 1 };
float orders[NBSERVOS];
// states of servos way2_start/way3_start and way2_end
int open_p23 = 0, open_p2 = 0;
// trials informations
int trial = 1;
int day = 1;
double trial_time;
int total_step = 1;
// The start & goal points
const char* start_name = "START";
double* start_position;
double* start_rotation;
const char* goal_name = "GOAL";
double* goal_position;
// Sleep parameter
int SLEEP_STEP;
// Enable the addition of 6 last forced trials
int FINAL_FORCED;
// stuff to create an automaton for the robot
enum robot_states { SEARCHING, GOALFOUND, SLEEPING } robot_state;
int wait = -1;

void message_protocol (enum protocol_type s) {
  fprintf(output_protoc, "%d %d %d %d\n", total_step, day, trial, s);
  fflush(output_protoc);
}

int go_way1 () {
  if (previous_way1 || nbChoice1 == 2 || drand48() > 0.4 ) {
    previous_way1 = 0;
    return 0;
  }
  else {
    nbChoice1++;
    previous_way1 = 1;
    return 1;
  }
}

void new_trial_doors(){
  if (day == 16 || day == 1) {
    /*
    trial 1 -> open : way 1
    trial 2 -> open : way 2
    trial 3 -> open : way 3
    trial 4-12 -> open : all
    */
    //~ if (trial == 1) {
    if (trial == 1 || trial == 4) {
      orders[0] = orders[1] = orders[2] = o;
      orders[3] = orders[4] = orders[5] = orders[6] = c;
      message_protocol (P1);
    }
    //~ else if (trial == 2) {
    else if (trial == 2 || trial == 5) {
      orders[3] = orders[4] = orders[2] = o;
      orders[0] = orders[1] = orders[5] = orders[6] = c;
      message_protocol (P2);
    }
    //~ else if (trial == 3) {
    else if (trial == 3 || trial == 6) {
      orders[5] = orders[6] = o;
      orders[3] = orders[4] = orders[0] = orders[1] = orders[2] = c;
      message_protocol (P3);
    }
    else {
      orders[0] = orders[1] = orders[2] = 
      orders[3] = orders[4] = orders[5] = orders[6] = o;
      message_protocol (FREE);
    }
  }
  else if (day == 15) {
    // Test period
    orders[2] = orders[3] = orders[4] = orders[5] = c;
    orders[0] = orders[1] = orders[6] = o;
    // uncomment if you don't want to force P1 at the beginning of each trial
    //orders[2] = orders[4] = c;
    //orders[0] = orders[1] = orders[3] = orders[5] = orders[6] = o;
    message_protocol (BLOCB);
  }
  else {
    // only 2 non-successive trials in way 1
    if (go_way1()) {
      orders[0] = orders[1] = orders[2] = 
      orders[3] = orders[4] = orders[5] = orders[6] = o;
      open_p23 = 1;
      message_protocol (FREE);
    }
    else {
      orders[1] = orders[3] = orders[5] = c;
      orders[0] = orders[2] = orders[4] = orders[6] = o;
      message_protocol (BLOCA);
    }
  }
}

void init_protocol (int simu_id, int final_forced, int sleep) {
  char output_path[128];
  sprintf (output_path, "../../data/data_raw/%d/protocol.txt", simu_id);
  output_protoc = fopen (output_path, "w");
  fprintf(output_protoc, "%s format : total_step day trial type\n", "%");
  fprintf(output_protoc, "%s type : 1,2,3=P1,P2,P3 ; 4=free ; 5=bloc@a ; 6=bloc@b\n", "%");

  // retrieve device handles
  int i;
  char servo_name[] = "servo0";
  for(i = 0; i < NBSERVOS; i++){
    servos[i] = wb_robot_get_device (servo_name);
    servo_name[5]++;
  }
  char sensor_name[] = "sensor0";
  for(i = 0; i < NBSENSORS; i++){
    sensors[i] = wb_robot_get_device (sensor_name);
    wb_distance_sensor_enable (sensors[i], TIME_STEP);
    sensor_name[6]++;
  }
  emitter = wb_robot_get_device ("emitter");

  // retrieve the start position
  WbNodeRef start_point = wb_supervisor_node_get_from_def (start_name);
  if (start_point != NULL) {
    WbFieldRef t_start = wb_supervisor_node_get_field (start_point, "translation");
    WbFieldRef r_start = wb_supervisor_node_get_field (start_point, "rotation");
    start_position = (double*) wb_supervisor_field_get_sf_vec3f (t_start);
    start_rotation = (double*) wb_supervisor_field_get_sf_rotation (r_start);
    //printf("Protocol: %f %f %f\n", start_position[0], start_position[2], start_rotation[3]);
  } 
  else {
    printf("Error: node %s not found\n", start_name);
  }
  // retrieve the goal position
  WbNodeRef goal_point = wb_supervisor_node_get_from_def (goal_name);
  if (goal_point != NULL) {
    WbFieldRef t_goal = wb_supervisor_node_get_field (goal_point, "translation");
    goal_position = (double*) wb_supervisor_field_get_sf_vec3f (t_goal);
    goal_position[2] += 0.04;
  } 
  else {
    printf("Error: node %s not found\n", goal_name);
  }
  
  trial_time = 0.0;
  FINAL_FORCED = final_forced;
  SLEEP_STEP = sleep;
  new_trial_doors();
  wb_robot_step (TIME_STEP);
}

void do_orders () {
  int i;
  for(i = 0; i < NBSERVOS; i++) {
    wb_servo_set_position (servos[i], orders[i]);
  }
}

void move_doors () {
  int i;
  int value[NBSENSORS];
  for(i = 0; i < NBSENSORS; i++) {
    value[i] = wb_distance_sensor_get_value (sensors[i]);
  }
  if (robot_state == SEARCHING && value[s_goal] < 900) {
    robot_state = GOALFOUND;
    wait = 100;
    char msg [] = "goal";
    wb_emitter_send (emitter, msg, strlen(msg) + 1);
    message (GOAL);
  } 
  if (value[s_way2_close] < 900 || value[s_end] < 900) {
    open_p2 = 0;
  }
  else if (value[s_way2] < 900) {
    open_p2 = 1;
  }
  orders[way2_end] = open_p2 ? o : c;
  if(day > 1 && day < 16) {
    if (value[s_mid] < 900) {
      open_p23 = 1;
    }
    orders[way2_start] = orders[way3_start] = open_p23 ? o : c;
  }
  do_orders();
}

void run_protocol () {
  // printing the simulation stats
  char score[128];
  sprintf(score, "Day %d, Trial %d : %s", day, trial, day >= 15 ? "Test" : (robot_state == SLEEPING ? "Sleep" : "Training"));
  wb_supervisor_set_label (0, score, 0.01, 0.01, 0.05, 0x0000ff, 0);
  sprintf(score, "Time %d s", (int)trial_time);
  wb_supervisor_set_label (1, score, 0.01, 0.05, 0.05, 0x0000ff, 0);

  // change the doors according to the trial number
  move_doors();

  trial_time += (float) TIME_STEP / 1000;
  total_step++;
  
  // sending the distance of the robot to the goal
  const double* translation = wb_supervisor_field_get_sf_vec3f (t_field);
  double dist[] = {translation[0] - goal_position[0], translation[2] - goal_position[2]};
  char dist_msg[128];
  sprintf(dist_msg, "%f", sqrt(dist[0] * dist[0] + dist[1] * dist[1]));
  wb_emitter_send (emitter, dist_msg, strlen(dist_msg) + 1);
  
  if (robot_state == GOALFOUND && wait == -1 && ((day - 1) * 12 + trial) % 6 == 0) {
    robot_state = SLEEPING;
    wait = SLEEP_STEP;
    if (SLEEP_STEP > 0 ) {
      message (SLEEP);
      char msg [] = "sleep";
      wb_emitter_send (emitter, msg, strlen(msg) + 1);
    }
  }
  else if (robot_state == SEARCHING && trial_time > 1000) {
    // maximum time reached
    message (EXCEED);
    // we move the animat at the goal position
    wb_supervisor_field_set_sf_vec3f (t_field, goal_position);
    wb_supervisor_field_set_sf_rotation (r_field, start_rotation);
  }
  else if ((robot_state == GOALFOUND || robot_state == SLEEPING) && wait == -1) {
    // Reset the maze & robot
    wb_supervisor_field_set_sf_vec3f (t_field, start_position);
    wb_supervisor_field_set_sf_rotation (r_field, start_rotation);
    trial++;
    // Let's change the day if necessary
    if ((day == 16 && trial > 6) || (day == 15 && trial > 7) || (trial > 12)){
      day++;
      trial = 1;
      nbChoice1 = previous_way1 = 0;
    }
    // When all the trials are done, we save the controller and quit webots
    if (day > 16 || (! FINAL_FORCED && day > 15)){
      supervisor_die ();
    }
    open_p23 = 0;
    new_trial_doors();
    trial_time = 0;
    current_way_init ();
    robot_state = SEARCHING;
  }
  if (wait >= 0) {
    wait--;
  }
}

int learning_done () {
  return (day > 1 || trial > 6);
}

void open_all () {
  orders[0] = orders[1] = orders[2] = orders[3] = orders[4] = orders[5] = orders[6] = o;
  do_orders ();
}

void die_protocol () {
  fflush(output_protoc);
  fclose(output_protoc);
}

int get_trial(){
  return trial;
}

int get_day(){
  return day;
}

int get_total_step(){
  return total_step;
}

double get_trial_time(){
  return trial_time;
}
