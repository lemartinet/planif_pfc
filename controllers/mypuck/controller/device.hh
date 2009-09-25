#ifndef DEVICE_HH
# define DEVICE_HH

#include <device/robot.h>
#include <device/differential_wheels.h>
#include <device/distance_sensor.h>
#include <device/receiver.h>
#include <device/camera.h>
#include <device/gps.h>
#include "coord.hh"

#define NB_SENSORS    8

// Permet de communiquer avec webots (recuperation des valeurs des capteurs...)
class RobotDevice
{
public:
  RobotDevice () : synch_(true), is_moving(false), goal_reached_prec_(false),
  			position_(0.0, -0.05), orientation_(0.0), num_pixels_goal_(0) {};

  inline
  void speed_set (float v1, float v2) { differential_wheels_set_speed ((int)v1, (int)v2); is_moving = (v1 > 0 || v2 > 0)?true:false; }
	
  inline 
  bool is_moving_get() {return is_moving;}

  inline
  int dist_sensor_get () { return distance_sensor_get_value (ps_[2]); }

  inline
  float angle_get () { return orientation_; }

  inline
  Coord& position_get () { return position_; }

  inline
  float obstacle_left_get () { return obstacle_left_; }

  inline
  float obstacle_right_get () { return obstacle_right_; }
  
  inline
  float obstacle_left_back_get () { return obstacle_left_back; }

  inline
  float obstacle_right_back_get () { return obstacle_right_back; }
  
  inline
  float obstacle_left_mid_get () { return obstacle_left_mid; }

  inline
  float obstacle_right_mid_get () { return obstacle_right_mid; }
  
  inline
  float obstacle_left_front_get () { return obstacle_left_front; }

  inline
  float obstacle_right_front_get () { return obstacle_right_front; }

  void init ();
  void synch ();
  
  bool goal_reached ();
  
  inline
  bool goal_reached_prec () { return goal_reached_prec_;}
  
  double distance_goal_factor ();
  
  bool all_ways_visited (bool reinit = false);
  
  bool obstacle_avoidance_module (double angle);
  
  bool manually_moved () { return manually_moved_; }

private:
  void update_obstacle_info ();
  void avoid_block ();

  DeviceTag ps_[NB_SENSORS];
  DeviceTag camera_;
  DeviceTag gps_;
  bool synch_;
  bool is_moving;
  bool manually_moved_;
  bool goal_reached_prec_;

  Coord            position_;
  float            orientation_;
  float            obstacle_left_front;
  float            obstacle_right_front;
  float            obstacle_left_mid;
  float            obstacle_right_mid;
  float            obstacle_left_;
  float            obstacle_right_;
  float            obstacle_left_back;
  float            obstacle_right_back;
  double goal_dir_;
  int num_pixels_goal_;
};

#endif
