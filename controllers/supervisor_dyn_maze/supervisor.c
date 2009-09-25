//-----------------------------------------------------------------------------
// Name:        icea_rat_supervisor.cpp
// Description: Doors & Lamp supervisor for ICEA rat robot simulation
// Author:      Yvan Bourquin <yvan.bourquin@cyberbotics.com>
// Date:        April 18th, 2006
//
// 16.03.2009   Updated to Webots 6.1
//-----------------------------------------------------------------------------

#include <webots/robot.h>
#include <webots/servo.h>
#include <webots/led.h>
#include <webots/touch_sensor.h>
#include <webots/distance_sensor.h>

const int TIME_STEP = 64; // milliseconds.

WbDeviceTag red_door_servo, blue_door_servo, red_touch_sensor,
  blue_touch_sensor, led, distance_sensor;
int previous_red_bumper_value = 0, previous_blue_bumper_value = 0;
float red_door_pos = -5.0, blue_door_pos = -5.0;

void reset() {
  // retrieve device handles
  red_door_servo = wb_robot_get_device("red_door_servo");
  blue_door_servo = wb_robot_get_device("blue_door_servo");
  red_touch_sensor = wb_robot_get_device("red_touch_sensor");
  blue_touch_sensor = wb_robot_get_device("blue_touch_sensor");
  led = wb_robot_get_device("led");
  distance_sensor = wb_robot_get_device("distance_sensor");
  
  // enable sensors
  wb_distance_sensor_enable(distance_sensor, TIME_STEP);
  wb_touch_sensor_enable(red_touch_sensor, TIME_STEP);
  wb_touch_sensor_enable(blue_touch_sensor, TIME_STEP);
}

void run() {
  
  // red sensor & door
  int sensor_value = wb_touch_sensor_get_value(red_touch_sensor);
  if (sensor_value && ! previous_red_bumper_value)
    red_door_pos = -red_door_pos;
  previous_red_bumper_value = sensor_value;
  wb_servo_set_position(red_door_servo, red_door_pos);
  
  // blue sensor & door
  sensor_value = wb_touch_sensor_get_value(blue_touch_sensor);
  if (sensor_value && ! previous_blue_bumper_value)
    blue_door_pos = -blue_door_pos;
  previous_blue_bumper_value = sensor_value;
  wb_servo_set_position(blue_door_servo, blue_door_pos);
  
  // distance sensor & lamp
  sensor_value = wb_distance_sensor_get_value(distance_sensor);
  wb_led_set(led, sensor_value < 900 ? 1 : 0);
}

int main() {
  wb_robot_init();
  reset();
  while(true) {
    run();
    wb_robot_step(TIME_STEP);
  }
  return 0;
}
