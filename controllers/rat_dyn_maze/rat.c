//-----------------------------------------------------------------------------
// Name:        icea_rat.c
// Description: demo ICEA robot controller:
//              The robot explores the labyrinth, avoiding obstacles.
//              When a feeder is detected by the camera, the robot 
//              approaches the feeder and recharges its batteries.
// Author:      Yvan Bourquin <Yvan.Bourquin@cyberbotics.com>
//
// 17.04.2006 v0.1 Created
// 05.05.2006 v0.2 Added camera servos to enable active vision
// 26.05.2006 v0.3 Added DistanceSensor whiskers + behaviours 
// 15.06.2006 v0.4 Replaced DistanceSensor whiskers by Servos whiskers with
//                 position feedback + added whiskers' vibrating motion
// 16.03.2009 v0.6 Updated to Webots 6.1
//-----------------------------------------------------------------------------

#include <webots/robot.h>
#include <webots/differential_wheels.h>
#include <webots/camera.h>
#include <webots/servo.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

// list of servos (wheels are not included)
enum {
  hips, right_arm, left_arm,
  head_tilt, head_pan,
  left_eye_pan, left_eye_tilt,
  right_eye_pan, right_eye_tilt,
  tail,
  NUM_SERVOS
};

WbDeviceTag servo[NUM_SERVOS];

const char *SERVO_NAMES[NUM_SERVOS] = {
  "hips_servo",
  "right_arm_servo",
  "left_arm_servo",
  "head_tilt_servo",
  "head_pan_servo",
  "left_eye_pan_servo",
  "left_eye_tilt_servo",
  "right_eye_pan_servo",
  "right_eye_tilt_servo",
  "tail_servo"
};

// wheels speed
const double NORMAL_SPEED = 40.0;
const double RUSH_SPEED = 80.0;

// misc
int keyboard_control = 0;
double prev_battery_level, battery_level = 99999.9;
double biological_clock = 0.0;

// list of whiskers
enum {
  left_small, left_medium, left_large,
  right_small, right_medium, right_large,
  NUM_WHISKERS
};

WbDeviceTag whisker[NUM_WHISKERS];
double desired_whisker_position[NUM_WHISKERS];

const char *WHISKER_NAMES[NUM_WHISKERS] = {
  "left_small_whisker",
  "left_medium_whisker",
  "left_large_whisker",
  "right_small_whisker",
  "right_medium_whisker",
  "right_large_whisker"
};

// loaded Charger color: green
const int REF_COLOR[3] = { 0, 255, 0 };

// cameras
int cameras_enabled = 1;
WbDeviceTag left_cam, right_cam;
int image_width = 0, image_height = 0, retina_size = 0;
double field_of_view = 0.0;
double eyes_tilt_angle = 0.0;
double eyes_pan_angle = 0.0;
double head_pan_angle = 0.0;
const double EYE_ANGLE_STEP = 0.05;

// log info message to console
void log_info(const char *info) {
  static const char *prev_info;
  if (info != prev_info) {
    printf(": %s\n", info);
    prev_info = info;
  }
}

inline int abs(int v) { return v < 0 ? -v : v; };

inline int rgb_diff(const unsigned char a[3], const int b[3]) {
  return abs(a[0] - b[0]) + abs(a[1] - b[1]) + abs(a[2] - b[2]);
}

// find feeder direction in camera image
// the image is compared to a reference color (ref_color) and thresholded,
// matching pixels are added and weighted from left to right
// returns: a number between -1.0 and +1.0
// -1.0 -> the feeder is located in the leftmost part of the image
// +1.0 -> the feeder is located in the rightmost part of the image
//  0.0 -> the feeder is located in the exact middle of the image
//  nan -> no pixel close to ref_color is currently visible
// output: npixels indicates the count of matching pixels in the image
double process_camera_image(const unsigned char *image, const int ref_color[3], int *npixels) {

  // compute rgb discrepancy with respect to ref_color
  int x, sum_pixels = 0, total = 0;
  for (x = 0; x < image_width; x++) {
    int y, sum_column = 0;
    for (y = 0; y < image_height; y++) {
      const unsigned char *pixel = image + 3 * (y * image_width + x);
      int bin = rgb_diff(pixel, ref_color) < 10 ? 1 : 0;
      sum_pixels += bin;
      sum_column += bin;
    }

    total += sum_column * x;
  }

  if (npixels) *npixels = sum_pixels;

  return ((double)total / ((double)sum_pixels * ((double)image_width - 1.0))) * 2.0 - 1.0;
}

// find feeder direction using two camera images
// assumed: the cameras are looking in the same direction
// returns: angle [rad] where the feeder is located, with respect
//   to therobot's longitudinal axis (head/tail)
// 0    -> feeder straight ahead
// pi/2 -> feeder on the right hand side, etc ...
// output: npixels indicates the sum of matching pixels in both images
double compute_target_direction(const unsigned char *left_image, const unsigned char *right_image, const int ref_color[3], int *npixels) {

  int lpixels, rpixels;
  double ldir = process_camera_image(left_image, ref_color, &lpixels);
  double rdir = process_camera_image(right_image, ref_color, &rpixels);

  if (npixels) *npixels = lpixels + rpixels;

  double dir;
  if (lpixels && ! rpixels)
    dir = ldir;
  else if (! lpixels && rpixels)
    dir = rdir;
  else
    dir = (ldir + rdir) / 2.0;

  // take fov and head angle into account
  double angle = dir * field_of_view / 2.0;
  return angle - head_pan_angle;
}

void reset() {

  int i;
  
  // enable body servos
  for (i = 0; i < NUM_SERVOS; i++)
    servo[i] = wb_robot_get_device(SERVO_NAMES[i]);

  // initialize cameras
  left_cam = wb_robot_get_device("left_cam");
  right_cam = wb_robot_get_device("right_cam");
  wb_camera_enable(left_cam, 128);
  wb_camera_enable(right_cam, 128);

  // get image dimensions (assuming left_cam are similar)
  image_width = wb_camera_get_width(left_cam);
  image_height = wb_camera_get_height(left_cam);
  retina_size = image_width * image_height;
  field_of_view = wb_camera_get_fov(left_cam);

  // enable whisker servos + feedback
  for (i = 0; i < NUM_WHISKERS; i++) {
    whisker[i] = wb_robot_get_device(WHISKER_NAMES[i]);
    servo_enable_position(whisker[i], 8);
  }

  // enable keyborad and battery sensor
  wb_robot_keyboard_enable(32);
  wb_robot_battery_sensor_enable(32);
}

// fits "value" into the specified range
void ensure_bounds(double *value, double lowest, double highest) {
  if (*value > highest)
    *value = highest;
  else if (*value < lowest)
    *value = lowest;
}

// move both cameras up/down dimultaneously
void look_up_down(double tilt_delta) {
  eyes_tilt_angle += tilt_delta;
  ensure_bounds(&eyes_tilt_angle, -1.5, 1.5);
  wb_servo_set_position(servo[left_eye_tilt], eyes_tilt_angle);
  wb_servo_set_position(servo[right_eye_tilt], eyes_tilt_angle);
}

// move both cameras left/right dimultaneously
void look_left_right(double pan_delta) {
  eyes_pan_angle += pan_delta;
  ensure_bounds(&eyes_pan_angle, -1.5, 1.5);
  wb_servo_set_position(servo[left_eye_pan], eyes_pan_angle);
  wb_servo_set_position(servo[right_eye_pan], eyes_pan_angle);
}

void set_head_pan_angle(double angle) {
  head_pan_angle = angle;
  wb_servo_set_position(servo[head_pan], angle);
}

void stand_up() {
  wb_servo_set_position(servo[hips], 0.0);
  wb_servo_set_position(servo[head_tilt], 0.0);
  set_head_pan_angle(0.0);
  wb_servo_set_position(servo[tail], 0.0);
}

void lay_down() {
  wb_servo_set_position(servo[hips], -1.4);
  wb_servo_set_position(servo[head_tilt], 1.75);
  set_head_pan_angle(0.0);
  wb_servo_set_position(servo[tail], 2.5);
}

// make the whiskers vibrate in anterior/posterior motion
void animate_whiskers() {
  const double A = 0.3;   // amplitude of whiskers anterior/posterio motion [rad]
  const double F = 2.0;   // motion cycle frequency [Hz]
  int i;
  for (i = 0; i < NUM_WHISKERS / 2; i++) {
    double new_pos = A * sin(biological_clock * 2.0 * M_PI * F);
    
    // left and right side whiskers get are synched
    desired_whisker_position[i] = new_pos;
    desired_whisker_position[i + NUM_WHISKERS / 2] = -new_pos;
  }
  
  // actuate whiskers
  for (i = 0; i < NUM_WHISKERS; i++)
    wb_servo_set_position(whisker[i], desired_whisker_position[i]);
}

int handle_keyboard(int key) {
  wb_differential_wheels_set_speed(0.0, 0.0);
  switch (key) {
    case 'C': // turn cameras on
      cameras_enabled = 1;
      break;
    case ' ': // turn cameras off
      cameras_enabled = 0;
      break;
    case 'M': // 'm' manual control
      keyboard_control = 1;
      break;
    case 'A': // 'a' auto control
      keyboard_control = 0;
      break;
    case WB_ROBOT_KEYBOARD_UP: // move forward [cursor up]
      wb_differential_wheels_set_speed(NORMAL_SPEED, NORMAL_SPEED);
      break;
    case WB_ROBOT_KEYBOARD_DOWN: // move backward [cursor down]
      wb_differential_wheels_set_speed(-NORMAL_SPEED, -NORMAL_SPEED);
      break;
    case WB_ROBOT_KEYBOARD_LEFT: // turn left [cursor left]
      wb_differential_wheels_set_speed(-NORMAL_SPEED, NORMAL_SPEED);
      break;
    case WB_ROBOT_KEYBOARD_RIGHT: // turn right [cursor right]
      wb_differential_wheels_set_speed(NORMAL_SPEED, -NORMAL_SPEED);
      break;
    case WB_ROBOT_KEYBOARD_PAGEUP:
      stand_up();
      break;
    case WB_ROBOT_KEYBOARD_PAGEDOWN:
      lay_down();
      break;
    case WB_ROBOT_KEYBOARD_HOME:
      wb_servo_set_position(servo[right_arm], -1.0);
      wb_servo_set_position(servo[left_arm], 1.0);
      break;
    case WB_ROBOT_KEYBOARD_END:
      wb_servo_set_position(servo[right_arm], 0.7);
      wb_servo_set_position(servo[left_arm], -0.7);
      break;
    case WB_ROBOT_KEYBOARD_NUMPAD_UP:
      look_up_down(EYE_ANGLE_STEP);
      break;
    case WB_ROBOT_KEYBOARD_NUMPAD_DOWN:
      look_up_down(-EYE_ANGLE_STEP);
      break;
    case WB_ROBOT_KEYBOARD_NUMPAD_LEFT:
      look_left_right(EYE_ANGLE_STEP);
      break;
    case WB_ROBOT_KEYBOARD_NUMPAD_RIGHT:
      look_left_right(-EYE_ANGLE_STEP);
      break;
    case WB_ROBOT_KEYBOARD_NUMPAD_HOME:
      wb_servo_set_position(servo[head_pan], 1.0);
      break;
    case WB_ROBOT_KEYBOARD_NUMPAD_END:
      wb_servo_set_position(servo[head_pan], -1.0);
      break;
  }
  return 32;
}

int recharge() {
  log_info("recharging batteries");
  wb_differential_wheels_set_speed(0.0, 0.0);
  animate_whiskers();
  return 32;
}

int avoid_obstacle(double left_contact, double right_contact) {
  log_info("avoiding obstacle");

  if (left_contact > right_contact) {
    // look and turn right
    set_head_pan_angle(-0.7);
    wb_differential_wheels_set_speed(NORMAL_SPEED, -NORMAL_SPEED);
  }
  else {
    // look and turn left
    set_head_pan_angle(0.7);
    wb_differential_wheels_set_speed(-NORMAL_SPEED, NORMAL_SPEED);
  }

  // turn by a random amount (=duration): this allows getting out of many tricky situations
  return (rand() % 31 + 1) * 32;
}

// the feeder is located: move quickly towards it
int approach_charger(double dir) {
  log_info("rushing to charger");
  lay_down();
  wb_differential_wheels_set_speed((1.0 + dir / 2.0) * RUSH_SPEED, (1.0 - dir / 2.0) * RUSH_SPEED);
  return 32;
}

// expore maze: a sequence of straight wheels displacements in low posture
// interrupted by head scans in high posture 
int explore_maze() {
  log_info("exploring maze");
  int state = (int)biological_clock;
  switch (state % 15) {
    case 0:
      // stop wheels rotation and stand up
      wb_differential_wheels_set_speed(0, 0);
      stand_up();
      break;
      // turn head from left to right
    case 1: set_head_pan_angle( 1.0); break;
    case 2: set_head_pan_angle(-1.0); break;
    case 3: set_head_pan_angle( 0.0); break;
    default:
      // lower body and move straight forward
      lay_down();
      wb_differential_wheels_set_speed(NORMAL_SPEED, NORMAL_SPEED);
      break;
  };
  
  animate_whiskers();

  return 32;
}

// Compute amount of contact by adding the discrepencies between
// the desired and effective whisker position.
// This function adds the difference for each whisker from start_index to stop_index.
// According to the indices specified, it can be used for the left or/and the right whiskers
// the result is a sum of angle differences in [rad]
double compute_whisker_contact(int start_index, int stop_index) {

  double contact = 0.0;
  int i;
  for (i = start_index; i <= stop_index; i++) {
    // difference between the desired position (of the vibrating motion) and the
    // effective position measured by the simulator.
    // the difference indicates a collision with a wall or another obstacle
    // (the whisker bends in case of collision because each whisker motor force
    // defined in the .wbt file is set to be very low (weak))
    contact += fabs(wb_servo_get_position(whisker[i]) - desired_whisker_position[i]);
  }
  
  return contact;
}

int run_step() {

  // read keyboard input
  int key = wb_robot_keyboard_get_key();

  // read battery level
  prev_battery_level = battery_level;
  battery_level = wb_robot_battery_sensor_get_value();

  // compute amount of whisker contact
  double left_contact = compute_whisker_contact(left_small, left_large);
  double right_contact = compute_whisker_contact(right_small, right_large);

  // read and process camera images
  int num_pixels;
  double feeder_dir;
  if (cameras_enabled) {
    unsigned char *left_image = wb_camera_get_image(left_cam);
    unsigned char *right_image = wb_camera_get_image(right_cam);
    feeder_dir = compute_target_direction(left_image, right_image, REF_COLOR, &num_pixels);
  }

  // choose behaviour according to stimuli
  if (keyboard_control || key)
    return handle_keyboard(key);
  else if (battery_level > prev_battery_level)
    return recharge();
  else if (left_contact > 0.7 || right_contact > 0.7)
    return avoid_obstacle(left_contact, right_contact);
  else if (cameras_enabled && num_pixels > 0)
    return approach_charger(feeder_dir);
  else
    return explore_maze();
}

int run() {
  int step = run_step();
  biological_clock += step / 1000.0;
  return step;
}

int main() {
  srand(time(NULL)); // seed random number generator
  wb_robot_init();
  reset();
  while(true){
    wb_robot_step(run());
  }
  return 0;
}
