#include "protocol.h"
#include "analysebehavior.h"
#include "mypuck_supervisor.h"
#include <webots/servo.h>
#include <webots/distance_sensor.h>
#include <webots/led.h>
#include <webots/supervisor.h>
#include <webots/robot.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Tolman & Honzik's experiment. 
 * Opens the doors, moves the robot to the start position, ...
 */
<<<<<<< HEAD
=======
// output file for protocol observations
FILE * output_protoc = NULL;
>>>>>>> 16cde28... First commit to git. Includes:

// Webots data
WbDeviceTag sensor_mid, sensor_end, sensor_goal, sensor_way2, sensor_way2_close,
  way1_start_servo,way1_mid_servo, way1_end_servo, way2_start_servo, 
  way2_end_servo, way3_start_servo, way3_end_servo;

// variable to wait before starting a new trial
int wait = -1;

// variable for way 1 random choice during training period
int nbChoice1 = 0;
int previous_way1 = 0;

// position of servos (c : close, o : open)
float c = -1.0, o = 1.0;
float orders[7];

// states of sensor_mid et sensor_end
int open_mid=0, open_end=0, open_2=0;

// trials informations
int trial = 1;
int day = 1;
int new_trial = 1;
double trial_time;
int total_step = 1;

// The start point & its positions (X,Z) and orientation
<<<<<<< HEAD
WbNodeRef start_point;
=======
>>>>>>> 16cde28... First commit to git. Includes:
const char* start_name = "START";
double* start_position;
double* start_rotation;

<<<<<<< HEAD
=======
// The goal name and position
const char* goal_name = "GOAL";
double* goal_position;


>>>>>>> 16cde28... First commit to git. Includes:
// Sleep parameter
int SLEEP_STEP;
int sleep = -1;

<<<<<<< HEAD
=======
// Enable the addition of 6 last forced trials
int FINAL_FORCED;

>>>>>>> 16cde28... First commit to git. Includes:
/////////////////////////////////////
// Functions to control the servos
/////////////////////////////////////

<<<<<<< HEAD
void init_protocol () {
=======
void init_protocol (int simu_id, int final_forced) {
  char output_path[128];
  sprintf (output_path, "../../data/data_raw/%d/protocol.txt", simu_id);
  output_protoc = fopen (output_path, "w");
  fprintf(output_protoc, "%s format : total_step day trial type\n", "%");
  fprintf(output_protoc, "%s type : 1,2,3=P1,P2,P3 ; 4=free ; 5=bloc@a ; 6=bloc@b\n", "%");

>>>>>>> 16cde28... First commit to git. Includes:
  trial_time = 0.0;

  // retrieve device handles
  way1_start_servo = wb_robot_get_device ("way1_start_servo");
  way1_mid_servo = wb_robot_get_device ("way1_mid_servo");
  way1_end_servo = wb_robot_get_device ("way1_end_servo");
  way2_start_servo = wb_robot_get_device ("way2_start_servo");
  way2_end_servo = wb_robot_get_device ("way2_end_servo");
  way3_start_servo = wb_robot_get_device ("way3_start_servo");
  way3_end_servo = wb_robot_get_device ("way3_end_servo");
  sensor_mid = wb_robot_get_device ("sensor_mid");
  sensor_end = wb_robot_get_device ("sensor_end");
  sensor_goal = wb_robot_get_device ("sensor_goal");
  sensor_way2 = wb_robot_get_device ("sensor_way2");
  sensor_way2_close = wb_robot_get_device ("sensor_way2_close");

  // enable sensors
  wb_distance_sensor_enable (sensor_mid, TIME_STEP);
  wb_distance_sensor_enable (sensor_end, TIME_STEP);
  wb_distance_sensor_enable (sensor_goal, TIME_STEP);
  wb_distance_sensor_enable (sensor_way2, TIME_STEP);
  wb_distance_sensor_enable (sensor_way2_close, TIME_STEP);

  // retrieve the start position
<<<<<<< HEAD
  start_point = wb_supervisor_node_get_from_def (start_name);
=======
  WbNodeRef start_point = wb_supervisor_node_get_from_def (start_name);
>>>>>>> 16cde28... First commit to git. Includes:
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
<<<<<<< HEAD
=======
  WbNodeRef goal_point = wb_supervisor_node_get_from_def (goal_name);
  if (goal_point != NULL) {
    WbFieldRef t_goal = wb_supervisor_node_get_field (goal_point, "translation");
    goal_position = (double*) wb_supervisor_field_get_sf_vec3f (t_goal);
    goal_position[2] += 0.04;
  } 
  else {
    printf("Error: node %s not found\n", goal_name);
  }


  FINAL_FORCED = final_forced;
>>>>>>> 16cde28... First commit to git. Includes:

  // retrieve the goal position
  
  wb_robot_step (TIME_STEP);
}

void do_orders () {
  wb_servo_set_position (way1_start_servo, orders[0]);
  wb_servo_set_position (way1_mid_servo, orders[1]);
  wb_servo_set_position (way1_end_servo, orders[2]);
  wb_servo_set_position (way2_start_servo, orders[3]);
  wb_servo_set_position (way2_end_servo, orders[4]);
  wb_servo_set_position (way3_start_servo, orders[5]);
  wb_servo_set_position (way3_end_servo, orders[6]);
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

void move_doors () {
  int sensor_value = wb_distance_sensor_get_value (sensor_goal);
  if (sensor_value == 0) {
    // suppress artefacts at the beginning of the simulation
  }
  else if (sleep < 0 && wait < 0 && sensor_value < 900) {
    wait = 100;
    emit_goal_found ();
<<<<<<< HEAD
    message ("goal", total_step, day, trial, trial_time);
  } 
  else if (day == 1) {
=======
    message ("5", total_step, day, trial, trial_time);
  } 
  else if ((FINAL_FORCED && day == 16) || day == 1) {
>>>>>>> 16cde28... First commit to git. Includes:
    /*
    trial 1 -> open : way 1
    trial 2 -> open : way 2
    trial 3 -> open : way 3
    trial 4-12 -> open : all
    */
    if (new_trial) {
      if (trial == 1|| trial == 4) {
//      if (trial == 1 || trial == 4 || trial == 7) {
        orders[0] = orders[1] = orders[2] = o;
        orders[3] = orders[4] = orders[5] = orders[6] = c;
<<<<<<< HEAD
        message ("p1", total_step, day, trial, trial_time);
=======
        message_protocol ("1", total_step, day, trial);
>>>>>>> 16cde28... First commit to git. Includes:
      }
      else if (trial == 2|| trial == 5) {
//      else if (trial == 2 || trial == 5 || trial == 8) {
        orders[3] = orders[4] = orders[2] = o;
        orders[0] = orders[1] = orders[5] = orders[6] = c;
<<<<<<< HEAD
        message ("p2", total_step, day, trial, trial_time);
=======
        message_protocol ("2", total_step, day, trial);
>>>>>>> 16cde28... First commit to git. Includes:
      }
      else if (trial == 3|| trial == 6) {
//      else if (trial == 3 || trial == 6 || trial == 9) {
        orders[5] = orders[6] = o;
        orders[3] = orders[4] = orders[0] = orders[1] = orders[2] = c;
<<<<<<< HEAD
        message ("p3", total_step, day, trial, trial_time);
=======
        message_protocol ("3", total_step, day, trial);
>>>>>>> 16cde28... First commit to git. Includes:
      }
      else {
        orders[0] = orders[1] = orders[2] = 
        orders[3] = orders[4] = orders[5] = orders[6] = o;
<<<<<<< HEAD
        message ("free", total_step, day, trial, trial_time);
=======
        message_protocol ("4", total_step, day, trial);
>>>>>>> 16cde28... First commit to git. Includes:
      }
      new_trial = 0;
    }
  }
  else if (day == 15) {
    // Test period
    if (new_trial) {
      orders[2] = orders[3] = orders[4] = orders[5] = c;
      orders[0] = orders[1] = orders[6] = o;
      // uncomment if you don't want to force P1 at the beginning of each trial
      //orders[2] = orders[4] = c;
      //orders[0] = orders[1] = orders[3] = orders[5] = orders[6] = o;
<<<<<<< HEAD
      message ("blocb", total_step, day, trial, trial_time);
=======
      message_protocol ("6", total_step, day, trial);
>>>>>>> 16cde28... First commit to git. Includes:
      new_trial = 0;
    }
    else {
      // comment if you don't want to force P1 at the beginning of each trial
      sensor_value = wb_distance_sensor_get_value (sensor_end);
      if (sensor_value < 900 || open_end) {
        open_end = 1;
        orders[3] = orders[5] = o;
      }
      sensor_value = wb_distance_sensor_get_value (sensor_mid);
      if ((sensor_value < 900 || open_mid) && open_end) {
        open_mid = 1;
        orders[4] = o;
      }
    }
  }
  else {
    /*
    only 2 non-successive trials in way 1
    */
    if (new_trial) {
      if (go_way1()) {
        orders[0] = orders[1] = orders[2] = 
        orders[3] = orders[4] = orders[5] = orders[6] = o;
<<<<<<< HEAD
        message ("free", total_step, day, trial, trial_time);
=======
        message_protocol ("4", total_step, day, trial);
>>>>>>> 16cde28... First commit to git. Includes:
      }
      else {
        orders[1] = orders[3] = orders[5] = c;
        orders[0] = orders[2] = orders[4] = orders[6] = o;
<<<<<<< HEAD
        message ("bloca", total_step, day, trial, trial_time);
=======
        message_protocol ("5", total_step, day, trial);
>>>>>>> 16cde28... First commit to git. Includes:
      }
      new_trial = 0;
    }
    else if (!previous_way1) {
      sensor_value = wb_distance_sensor_get_value (sensor_mid);
      if (sensor_value < 900 || open_mid) {
        open_mid = 1;
        orders[3] = orders[5] = o;
      }
    }    
  }
  sensor_value = wb_distance_sensor_get_value (sensor_way2_close);
  int sensor_value2 = wb_distance_sensor_get_value (sensor_end);
  int sensor_value3 = wb_distance_sensor_get_value (sensor_way2);
  if (sensor_value < 900 || sensor_value2 < 900) {
    open_2 = 0;
  }
  else if (sensor_value3 < 900) {
    open_2 = 1;
  }
  if (open_2) {
    orders[4] = o;
  }
  else {
    orders[4] = c;
  }
  do_orders();
}

void run_protocol (int* day_, int* trial_, double* trial_time_, int* total_step_) {
  //  day = 15; trial = 1;

<<<<<<< HEAD
  trial_time += (float) TIME_STEP / 1000;
  total_step++;

=======
>>>>>>> 16cde28... First commit to git. Includes:
  // printing the simulation stats
  char score[128];
  sprintf(score, "Day %d, Trial %d : %s", day, trial, day >= 15 ? "Test" : (sleep > 0 ? "Sleep" : "Training"));
  wb_supervisor_set_label (0, score, 0.01, 0.01, 0.05, 0x0000ff, 0);
  sprintf(score, "Time %d s", (int)trial_time);
  wb_supervisor_set_label (1, score, 0.01, 0.05, 0.05, 0x0000ff, 0);

  // change the doors according to the trial number
  move_doors();
<<<<<<< HEAD
  
  if (sleep == -1 && wait == 0) {
//    if (sleep == 0 && wait == 0 && ((*day_ - 1) * 12 + *trial_) % 3 == 0) {
      if (SLEEP_STEP > 0) {
        sleep = SLEEP_STEP;
        message ("sleep", total_step, day, trial, trial_time);
=======

  trial_time += (float) TIME_STEP / 1000;
  total_step++;
  
  if (sleep == -1 && wait == 0) {
//    if (sleep == 0 && wait == 0 && ((*day_ - 1) * 12 + *trial_) % 3 == 0) {
      if (SLEEP_STEP > 0 && ((*day_ - 1) * 12 + *trial_) % 6 == 0) {
        sleep = SLEEP_STEP;
        message ("7", total_step, day, trial, trial_time);
>>>>>>> 16cde28... First commit to git. Includes:
        emit_sleep ();
      }
      else {
        sleep = 1;
      }
  }
<<<<<<< HEAD
  else if ((sleep == 0 && wait == -1) || (sleep == -1 && trial_time > 1000)) {
    if (trial_time > 1000) {
      message ("exceeded", total_step, day, trial, trial_time);
    }
=======
  else if (sleep == -1 && wait == -1 && trial_time > 1000) {
    // maximum time reached
    message ("6", total_step, day, trial, trial_time);
    // we move the animat at the goal position
    wb_supervisor_field_set_sf_vec3f (t_field, goal_position);
    wb_supervisor_field_set_sf_rotation (r_field, start_rotation);
  }
  else if (sleep == 0 && wait == -1) {
>>>>>>> 16cde28... First commit to git. Includes:
    // Reset the maze & robot
    wb_supervisor_field_set_sf_vec3f (t_field, start_position);
    wb_supervisor_field_set_sf_rotation (r_field, start_rotation);
    trial++;
    new_trial = 1;
    open_mid = 0;
    open_end = 0;
    trial_time = 0;
    current_way_init ();
  }
  if (wait >= 0) {
    wait--;
  }
  if (sleep >= 0) {
    sleep--;
  }
  
  // Let's change the day if necessary
<<<<<<< HEAD
  if ((day == 15 && trial > 7) || (trial > 12)){
=======
  if ((FINAL_FORCED && day == 16 && trial > 6) || (day == 15 && trial > 7) || (trial > 12)){
>>>>>>> 16cde28... First commit to git. Includes:
// ces lignes permettent de sauter de day1 à day15
//    if (trial > 12) {
//      day = 15;
//    }
//    else {
      day++;
//    }
    //printf("Supervisor : day %d\n", day);
    trial = 1;
    nbChoice1 = 0;
    previous_way1 = 0;
  }
  
  // When all the trials are done, we save the controller and quit webots
  //if (day > 1){ // one day long simulation to test the 2nd layer of columns
<<<<<<< HEAD
  if (day > 15){
    day = 15;
    trial = 7;
=======
  if ((FINAL_FORCED && day > 16) || (! FINAL_FORCED && day > 15)){
    day = FINAL_FORCED ? 16 : 15;
    trial = FINAL_FORCED ? 6 : 7;
>>>>>>> 16cde28... First commit to git. Includes:
    supervisor_die ();
  }
  *day_ = day;
  *trial_ = trial;
  *trial_time_ = trial_time;
  *total_step_ = total_step;
}

int learning_done () {
  return (day > 1 || trial > 6);
}

void open_all () {
  orders[0] = orders[1] = orders[2] = orders[3] = orders[4] = orders[5] = orders[6] = o;
  do_orders ();
}

void die_protocol (int* day_, int* trial_) {
  *day_ = day;
  *trial_ = trial;
<<<<<<< HEAD
=======
  fflush(output_protoc);
  fclose(output_protoc);
>>>>>>> 16cde28... First commit to git. Includes:
}

void sleep_step_set (int step) {
  SLEEP_STEP = step;
}
<<<<<<< HEAD
=======

void message_protocol (const char* s, int total_step, int day, int trial) {
  fprintf(output_protoc, "%d %d %d %s\n", total_step, day, trial, s);
  fflush(output_protoc);
}
>>>>>>> 16cde28... First commit to git. Includes:
