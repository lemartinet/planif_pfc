/*
 * Module to analyse the robot behavior:
 * position, current way, ...
 */
#include "analysebehavior.h"
#include "supervisor.h"
#include "protocol.h"
#include <webots/supervisor.h>
#include <webots/robot.h>
#include <webots/distance_sensor.h>
#include <stdio.h>
#include <math.h>

// Webots devices
WbDeviceTag dsensor_short1, dsensor_short2;

// output files and variables for behavior analysis
FILE * output_file = NULL, * output_vars = NULL;
int write_step;
int current_way;

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

void run_behavior () 
{
  const double* rotation = wb_supervisor_field_get_sf_rotation (r_field);
  double orientation = rotation[3];
	double diff1 = fabs(ecart_angulaire (orientation, 0));
	double diff2 = fabs(ecart_angulaire (orientation, -M_PI/4));
  double diff3 = fabs(ecart_angulaire (orientation, M_PI));
	double diff4 = fabs(ecart_angulaire (orientation, 3*M_PI/4));
  
  double dway1 = wb_distance_sensor_get_value (dsensor_short1);
  double dway2 = wb_distance_sensor_get_value (dsensor_short2); 
  double diff[] = {diff1, diff2, diff3, diff4};
  double dway[] = {dway2, dway1, dway2, dway1};
  int i;
  for (i = 0; i < 4; i++) {
    if (current_way != i && dway[i] < 500 && diff[i] < 0.2) {
      int the_path = i < 2 ? i + 1 : i - 1;
      the_path = (i < 2 ? 1 : -1) * the_path;
      current_way = i;
      message (the_path);
      break;
    }
  }
  
  if (get_total_step() % write_step == 0) {
    const double* translation = wb_supervisor_field_get_sf_vec3f (t_field);
    const double* rotation = wb_supervisor_field_get_sf_rotation (r_field);
    fprintf(output_vars, "%d %d %f %f %f\n", get_total_step(), get_trial(),
            translation[0], translation[2], rotation[3]);
    fflush(output_vars);
  }
}

void init_behavior (int simu_id, int step) {
  char output_path[128];
  sprintf (output_path, "../../data/data_raw/%d/vars.txt", simu_id);
  output_vars = fopen (output_path, "w");
  fprintf(output_vars, "%s format : total_step day trial x y angle\n", "%");
  sprintf (output_path, "../../data/data_raw/%d/events.txt", simu_id);
  output_file = fopen (output_path, "w");
  fprintf(output_file, "%s format : total_step day trial time(s) event\n", "%");
  fprintf(output_file, "%s event = [-]1/2=[-]wayX (current direction), 5=goal reached, 6=exceeded time, 7=sleep\n", "%");

  dsensor_short1 = wb_robot_get_device ("sensor_short1");
  dsensor_short2 = wb_robot_get_device ("sensor_short2");
  wb_distance_sensor_enable (dsensor_short1, TIME_STEP);
  wb_distance_sensor_enable (dsensor_short2, TIME_STEP);
  current_way = -1;
  write_step = step;
}

void die_behavior () {
  fflush(output_file);
  fflush(output_vars);
  fclose(output_file);
  fclose(output_vars);
}

void message (enum message_type s) {
  fprintf(output_file, "%d %d %.2f %d\n", get_total_step(), get_trial(), get_trial_time(), s);
  fflush(output_file);
}

void current_way_init () {
  current_way = -1;  
}