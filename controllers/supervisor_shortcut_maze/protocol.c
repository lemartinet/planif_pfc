/*
 * Shortcut experiment. 
 * Opens the doors, moves the robot to the start position, ...
 * louis-emmanuel.martinet@upmc.fr
 */
#include "protocol.h"
#include "analysebehavior.h"
#include "supervisor.h"
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
#define NBSENSORS 1
#define NBSERVOS 4
WbDeviceTag sensors[NBSENSORS];
WbDeviceTag servos[NBSERVOS];
WbDeviceTag emitter;
enum servo_name { short1_start = 0, short1_end = 1,
                  short2_start = 2, short2_end = 3 };
enum sensor_name { s_goal = 0 };

// output file for protocol observations
FILE * output_protoc = NULL;
enum protocol_type { FREE = 0, BLOC = 1 };
// position of servos (c : close, o : open), and actions
enum servo_position { c = 1, o = -1 };
float orders[NBSERVOS];
// trials informations
int trial = 1;
double trial_time;
int total_step = 1;
// The start & goal points
const char* start_name = "START";
double* start_position;
double* start_rotation;
const char* goal_name = "GOAL";
double* goal_position;
// stuff to create an automaton for the robot
enum robot_states { SEARCHING, GOALFOUND, SLEEPING } robot_state;
int wait = -1;
// shortcut typedef (0 = direct shortcut, 1 = indirect shortcut)
int shortcut;

void message_protocol (enum protocol_type s) {
  fprintf(output_protoc, "%d %d %d\n", total_step, trial, s);
  fflush(output_protoc);
}

void new_trial_doors(){
  if (trial < 7) {
    orders[0] = orders[1] = orders[2] = orders[3] = c;
    message_protocol (BLOC);
  } else if (shortcut == 0) {
    // Test period
    orders[1] = orders[3] = o;
    orders[0] = orders[2] = c;
    message_protocol (FREE);
  } else if (shortcut == 1) {
    // Test period
    orders[1] = orders[3] = c;
    orders[0] = orders[2] = o;
    message_protocol (FREE);
  }
}

void init_protocol (int simu_id, int cut) {
  char output_path[128];
  sprintf (output_path, "../../data/data_raw/%d/protocol.txt", simu_id);
  output_protoc = fopen (output_path, "w");
  fprintf(output_protoc, "%s format : total_step day trial type\n", "%");
  fprintf(output_protoc, "%s type : 0=free ; 1=blocs\n", "%");

  // retrieve device handles
  int i;
  char servo_name[] = "servo0";
  for(i = 0; i < NBSERVOS; i++){
    servos[i] = wb_robot_get_device (servo_name);
    servo_name[5]++;
  }
  sensors[0] = wb_robot_get_device ("sensor_goal");
  wb_distance_sensor_enable (sensors[0], TIME_STEP);
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
  shortcut = cut;
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
  do_orders();
}

void run_protocol () {
  // printing the simulation stats
  char score[128];
  sprintf(score, "Trial %d : %s", trial, trial >= 7 ? "Test" : "Training");
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
  
  if (robot_state == SEARCHING && trial_time > 1000) {
    // maximum time reached
    message (EXCEED);
    // we move the animat at the goal position
    wb_supervisor_field_set_sf_vec3f (t_field, goal_position);
    wb_supervisor_field_set_sf_rotation (r_field, start_rotation);
  }
  else if (robot_state == GOALFOUND && wait == -1) {
    // Reset the maze & robot
    wb_supervisor_field_set_sf_vec3f (t_field, start_position);
    wb_supervisor_field_set_sf_rotation (r_field, start_rotation);
    trial++;
    // When all the trials are done, we quit webots
    if (trial > 7) {
      supervisor_die ();
    }
    new_trial_doors();
    trial_time = 0;
    current_way_init ();
    robot_state = SEARCHING;
  }
  if (wait >= 0) {
    wait--;
  }
}

void die_protocol () {
  fflush(output_protoc);
  fclose(output_protoc);
}

int get_trial(){
  return trial;
}

int get_total_step(){
  return total_step;
}

double get_trial_time(){
  return trial_time;
}
