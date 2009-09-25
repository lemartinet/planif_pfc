/*
 * File:         rat_controller.c
 * Date:         9 March 09
 * Description:  sample
 * Author:       fabien.rohrer@cyberbotics.com
 */

#include <webots/robot.h>
#include <webots/differential_wheels.h>
#include <webots/distance_sensor.h>
#include <webots/camera.h>
#include <webots/emitter.h>
#include <webots/gps.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TIME_STEP 32
#define MAX_DISTANCE_SENSOR 1000.0
#define MAX_SPEED 5.0

WbDeviceTag right_cam, left_cam, right_whiskers,
  left_whiskers, emitter, gps;

void step() {
  if (wb_robot_step(TIME_STEP)==-1) {
    wb_robot_cleanup();
    exit(EXIT_SUCCESS);
  }
}

void reset() {
  // Food found => tell supervisor to reset
  const char message[] = "R";
  wb_emitter_send(emitter,message,strlen(message));
  //wait on the supervisor
  wb_differential_wheels_set_speed(0.0,0.0);
  while (wb_robot_battery_sensor_get_value() > 1.0) step();
}

int main(int argc, char **argv)
{
  wb_robot_init();
  
  right_cam = wb_robot_get_device("right_cam");
  left_cam = wb_robot_get_device("left_cam");
  right_whiskers = wb_robot_get_device("right_whiskers");
  left_whiskers = wb_robot_get_device("left_whiskers");
  emitter = wb_robot_get_device("emitter");
  gps = wb_robot_get_device("gps");
  
  wb_distance_sensor_enable(right_whiskers,TIME_STEP);
  wb_distance_sensor_enable(left_whiskers,TIME_STEP);
  wb_camera_enable(right_cam,TIME_STEP);
  wb_camera_enable(left_cam,TIME_STEP);
  wb_gps_enable(gps,TIME_STEP);
  
  wb_robot_battery_sensor_enable(TIME_STEP); 
  
  double left_speed, right_speed;
  int counter = 0;
  
  while (true) {
    //init
    counter++;
    
    //Read the sensors
    double right_whiskers_value = wb_distance_sensor_get_value(right_whiskers);
    double left_whiskers_value = wb_distance_sensor_get_value(left_whiskers);
    double battery_value = wb_robot_battery_sensor_get_value();
    const double *gps_values = wb_gps_get_values(gps);
    
    // Process sensor data
    if (counter>100) {
      counter=0;
      printf("My position is (%f,%f)\n",gps_values[0],gps_values[2]);
    }
    if (battery_value > 1.0) {
      //reset
      reset();
    }
    if (right_whiskers_value < MAX_DISTANCE_SENSOR ||
        left_whiskers_value < MAX_DISTANCE_SENSOR) {
      // obstacle detected => turn
      left_speed = -MAX_SPEED;
      right_speed = MAX_SPEED;
    } else {
      // no obstacle => forward
      left_speed = MAX_SPEED;
      right_speed = MAX_SPEED;
    }
    
    // send actuator commands
    wb_differential_wheels_set_speed(left_speed,right_speed);
    
    //perform a simulation step
    step();
  }
  
  return 0;
}
