#include "analysebehavior.h"
#include "mypuck_supervisor.h"
#include <webots/supervisor.h>
#include <webots/robot.h>
#include <webots/distance_sensor.h>
#include <webots/emitter.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// output files and variables for behavior analysis
FILE * output_file = NULL, * output_vars = NULL;

// tab to count each decision for way0,..way3,-way0,..-way3
// for each day (day 15, here idx 14, is the test day)
int nb_way[8][15];

WbDeviceTag dsensor_way1, dsensor_way2, dsensor_way3, 
  dsensor_way0, emitter;

int current_way;

// output params
int write_step;

////////////////////////////////////////////
// Functions to analyse the robot behavior
////////////////////////////////////////////

double ecart_angulaire (double reel, double voulu)
{
  double a = voulu - reel;

  if (a > M_PI)
    return a - 2 * M_PI;
  else if (a < -M_PI)
    return a + 2 * M_PI;
  else
    return a;
}

void analyse_behavior (int day, int trial, double trial_time, int total_step) 
{
  const double* rotation = wb_supervisor_field_get_sf_rotation (r_field);
  double orientation = rotation[3];
	double diff1 = fabs(ecart_angulaire (orientation, 0));
	double diff2 = fabs(ecart_angulaire (orientation, M_PI/2.0));
  double diff3 = fabs(ecart_angulaire (orientation, M_PI));
	double diff4 = fabs(ecart_angulaire (orientation, -M_PI/2.0));
  
  double dway1 = wb_distance_sensor_get_value (dsensor_way1);
  double dway2 = wb_distance_sensor_get_value (dsensor_way2);
  double dway3 = wb_distance_sensor_get_value (dsensor_way3);
  double dway0 = wb_distance_sensor_get_value (dsensor_way0);  
  
  char way[8];
  double diff[] = {diff1, diff1, diff2, diff4, diff3, diff3, diff4, diff2};
  double dway[] = {dway0, dway1, dway2, dway3, dway0, dway1, dway2, dway3};
  int i;
  for (i = 0; i < 8; i++) {
    if (current_way != i && dway[i] < 500 && diff[i] < 0.2) {
<<<<<<< HEAD
      sprintf (way, "%sway%d", i<4?"":"-", i<4?i:i-4);
=======
      int the_path = i<4?i:i-4;
      the_path = the_path == 0 ? 4 : the_path;
      sprintf (way, "%s%d", i<4?"":"-", the_path);
>>>>>>> 16cde28... First commit to git. Includes:
      current_way = i;
      nb_way[i][day-1]++;
      wb_emitter_send (emitter, way, strlen(way) + 1);
      message (way, total_step, day, trial, trial_time);
      break;
    }
  }
  
  if (total_step % write_step == 0) {
    const double* translation = wb_supervisor_field_get_sf_vec3f (t_field);
    const double* rotation = wb_supervisor_field_get_sf_rotation (r_field);
    fprintf(output_vars, "%d %d %d %f %f %f\n", total_step, day, trial, translation[0], translation[2], rotation[3]);
    fflush(output_vars);
  }
}

void init_behavior (int simu_id) {
  char output_path[128];
  sprintf (output_path, "../../data/data_raw/%d/vars.txt", simu_id);
  output_vars = fopen (output_path, "w");
  fprintf(output_vars, "%s format : total_step day trial x y angle\n", "%");
  sprintf (output_path, "../../data/data_raw/%d/events.txt", simu_id);
  output_file = fopen (output_path, "w");
  fprintf(output_file, "%s format : total_step day trial time(s) event\n", "%");
<<<<<<< HEAD
  fprintf(output_file, "%s event = [-]wayX (current direction), goal (reached), exceeded (time), pX (forced path), free (path), bloca/b\n", "%");
=======
  fprintf(output_file, "%s event = [-]1/2/3/4=[-]wayX (current direction), 5=goal reached, 6=exceeded time, 7=sleep\n", "%");
>>>>>>> 16cde28... First commit to git. Includes:

  current_way = -1;  
  dsensor_way1 = wb_robot_get_device ("sensor_mid");
  dsensor_way2 = wb_robot_get_device ("sensor_way2_start");
  dsensor_way3 = wb_robot_get_device ("sensor_way3_start");
  dsensor_way0 = wb_robot_get_device ("sensor_way0_start");
  emitter = wb_robot_get_device ("emitter");
  
  wb_distance_sensor_enable (dsensor_way1, TIME_STEP);
  wb_distance_sensor_enable (dsensor_way2, TIME_STEP);
  wb_distance_sensor_enable (dsensor_way3, TIME_STEP);
  wb_distance_sensor_enable (dsensor_way0, TIME_STEP);
}

<<<<<<< HEAD
void die_behavior (int day, int trial) {
=======
void die_behavior () {
>>>>>>> 16cde28... First commit to git. Includes:
  //~ fprintf(output_file, "===========================================\n");
  //~ fprintf(output_file, "%d days %d trials\n", day, (day-1) * 12 + trial);
  //~ char way[8];
  //~ int i;
  //~ for (i = 0; i < 15; i++) {
    //~ fprintf(output_file, "Day%d : ", i+1);
    //~ int j;
    //~ for (j = 0; j < 8; j++) {
      //~ sprintf (way, "%sway%d", j<4?"":"-", j<4?j:j-4);
      //~ fprintf(output_file, "%s %d, ", way, nb_way[j][i]);
    //~ }
    //~ fprintf(output_file, "\n");
  //~ }
  //~ int nb_way1_train = 0, nb_way2_train = 0, nb_way3_train = 0;
  //~ for (i = 0; i < 14; i++) {
    //~ nb_way1_train += nb_way[1][i];
    //~ nb_way2_train += nb_way[2][i];
    //~ nb_way3_train += nb_way[3][i];
  //~ }
  //~ fprintf(output_file, "Training : way1 %d, way2 %d, way3 %d\n", 
    //~ nb_way1_train, nb_way2_train, nb_way3_train);
  //~ fprintf(output_file, "Test : way1 %d, way2 %d, way3 %d\n", 
    //~ nb_way[1][14], nb_way[2][14], nb_way[3][14]);
  fflush(output_file);
  fflush(output_vars);
  fclose(output_file);
  fclose(output_vars);
}

void message (const char* s, int total_step, int day, int trial, double trial_time) {
  fprintf(output_file, "%d %d %d %.2f %s\n", total_step, day, trial, trial_time, s);
  fflush(output_file);
}

void write_step_set (int step) {
  write_step = step;
}

void emit_goal_found () {
  char msg [] = "goal";
  wb_emitter_send (emitter, msg, strlen(msg) + 1);
  current_way_init ();
}

void emit_sleep () {
  char msg [] = "sleep";
  wb_emitter_send (emitter, msg, strlen(msg) + 1);
}

void current_way_init () {
  current_way = -1;  
}