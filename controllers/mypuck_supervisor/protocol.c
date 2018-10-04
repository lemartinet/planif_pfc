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
WbNodeRef start_point;
const char* start_name = "START";
double* start_position;
double* start_rotation;

// Sleep parameter
int SLEEP_STEP;
int sleep = -1;

/////////////////////////////////////
// Functions to control the servos
/////////////////////////////////////

void init_protocol () {
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
  start_point = wb_supervisor_node_get_from_def (start_name);
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
    message ("goal", total_step, day, trial, trial_time);
  } 
  else if (day == 1) {
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
        message ("p1", total_step, day, trial, trial_time);
      }
      else if (trial == 2|| trial == 5) {
//      else if (trial == 2 || trial == 5 || trial == 8) {
        orders[3] = orders[4] = orders[2] = o;
        orders[0] = orders[1] = orders[5] = orders[6] = c;
        message ("p2", total_step, day, trial, trial_time);
      }
      else if (trial == 3|| trial == 6) {
//      else if (trial == 3 || trial == 6 || trial == 9) {
        orders[5] = orders[6] = o;
        orders[3] = orders[4] = orders[0] = orders[1] = orders[2] = c;
        message ("p3", total_step, day, trial, trial_time);
      }
      else {
        orders[0] = orders[1] = orders[2] = 
        orders[3] = orders[4] = orders[5] = orders[6] = o;
        message ("free", total_step, day, trial, trial_time);
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
      message ("blocb", total_step, day, trial, trial_time);
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
        message ("free", total_step, day, trial, trial_time);
      }
      else {
        orders[1] = orders[3] = orders[5] = c;
        orders[0] = orders[2] = orders[4] = orders[6] = o;
        message ("bloca", total_step, day, trial, trial_time);
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

  trial_time += (float) TIME_STEP / 1000;
  total_step++;

  // printing the simulation stats
  char score[128];
  sprintf(score, "Day %d, Trial %d : %s", day, trial, day >= 15 ? "Test" : (sleep > 0 ? "Sleep" : "Training"));
  wb_supervisor_set_label (0, score, 0.01, 0.01, 0.05, 0x0000ff, 0);
  sprintf(score, "Time %d s", (int)trial_time);
  wb_supervisor_set_label (1, score, 0.01, 0.05, 0.05, 0x0000ff, 0);

  // change the doors according to the trial number
  move_doors();
  
  if (sleep == -1 && wait == 0) {
//    if (sleep == 0 && wait == 0 && ((*day_ - 1) * 12 + *trial_) % 3 == 0) {
      if (SLEEP_STEP > 0) {
        sleep = SLEEP_STEP;
        message ("sleep", total_step, day, trial, trial_time);
        emit_sleep ();
      }
      else {
        sleep = 1;
      }
  }
  else if ((sleep == 0 && wait == -1) || (sleep == -1 && trial_time > 1000)) {
    if (trial_time > 1000) {
      message ("exceeded", total_step, day, trial, trial_time);
    }
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
  if ((day == 15 && trial > 7) || (trial > 12)){
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
  if (day > 15){
    day = 15;
    trial = 7;
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
}

void sleep_step_set (int step) {
  SLEEP_STEP = step;
}
