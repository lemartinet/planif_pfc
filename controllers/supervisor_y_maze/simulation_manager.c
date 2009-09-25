/*
 * File:         simulation_manager.c
 * Date:         12 Mar 09
 * Description:  Supervise the simulation
 * Author:       fabien.rohrer@cyberbotics.com
 */

#include <webots/robot.h>
#include <webots/supervisor.h>
#include <webots/receiver.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TIME_STEP 128

WbFieldRef left_charger_battery, right_charger_battery,
  rat_battery, rat_translation, rat_rotation,
  left_color, right_color;

double translation[3] = {0, 0.005, -0.7};
double rotation[4] = {0,1,0,-1.5708};
double red[3] = {1,0,0};
double green[3] = {0,1,0};

// reset the robot position and the battery levels
void reset () {
  wb_supervisor_field_set_mf_float(left_charger_battery,0,1.0);
  wb_supervisor_field_set_mf_float(right_charger_battery,0,1.0);
  wb_supervisor_field_set_mf_float(rat_battery,0,1.0);
  wb_supervisor_field_set_sf_vec3f(rat_translation, translation);
  wb_supervisor_field_set_sf_rotation(rat_rotation, rotation);
  if (rand()%2) {
    wb_supervisor_field_set_sf_color(left_color, red);
    wb_supervisor_field_set_sf_color(right_color, green);
  } else {
    wb_supervisor_field_set_sf_color(left_color, green);
    wb_supervisor_field_set_sf_color(right_color, red);  
  }
}

void fields_initialization () {
  left_charger_battery = wb_supervisor_node_get_field(
    wb_supervisor_node_get_from_def("CHARGER_LEFT"),
    "battery");
  right_charger_battery = wb_supervisor_node_get_field(
    wb_supervisor_node_get_from_def("CHARGER_RIGHT"),
    "battery");
  WbNodeRef rat = wb_supervisor_node_get_from_def("RAT");
  rat_battery = wb_supervisor_node_get_field(rat,"battery");  
  rat_translation = wb_supervisor_node_get_field(rat,"translation");
  rat_rotation = wb_supervisor_node_get_field(rat,"rotation");
  left_color = wb_supervisor_node_get_field(
    wb_supervisor_node_get_from_def("LEFT_POINT_LIGHT"),
    "color");
  right_color = wb_supervisor_node_get_field(
    wb_supervisor_node_get_from_def("RIGHT_POINT_LIGHT"),
    "color");
}

int main(int argc, char **argv)
{
  wb_robot_init();
  
  //init the random seed with the current time
  srand((int)time(NULL));
  
  fields_initialization();
  
  WbDeviceTag receiver = wb_robot_get_device("receiver");
  wb_receiver_enable(receiver, TIME_STEP);
  wb_robot_keyboard_enable(TIME_STEP); 
  
  reset();
  wb_robot_step(TIME_STEP);
  
  bool to_reset = false;
  int key;
  
  do {
    //Init
    to_reset = false;
    
    //Read the sensors
    while (wb_receiver_get_queue_length(receiver)>0) {
      const char *message = wb_receiver_get_data(receiver);
      if (message[0]=='R') {
        //if a robot has sent R, reset the simualtion
        to_reset = true;
      } else {
        printf("Received an unknown message\n");
      }
      wb_receiver_next_packet(receiver);
    }
    do {
      key = wb_robot_keyboard_get_key();
      //if r is pressed rest the simualtion
      if (key == 'R') to_reset = true;
    } while (key!=0);
    
    // Process sensor data
    if (to_reset) {
      reset();
      to_reset = false;
    }
    
  } while (wb_robot_step(TIME_STEP) != -1);

  wb_robot_cleanup();
  
  return 0;
}
