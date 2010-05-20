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


// output files and variables for behavior analysis
FILE * output_file = NULL, * output_vars = NULL;
int write_step;

void run_behavior () 
{
	const double* rotation = wb_supervisor_field_get_sf_rotation (r_field);
	double orientation = rotation[3];

	if (get_total_step() % write_step == 0) {
		const double* translation = wb_supervisor_field_get_sf_vec3f (t_field);
		const double* rotation = wb_supervisor_field_get_sf_rotation (r_field);
		fprintf(output_vars, "%d %d %d %f %f %f\n", get_total_step(), get_day(), get_trial(),
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
	fprintf(output_file, "%s event = 1=goal reached, 2=exceeded time, 3=sleep\n", "%");

	write_step = step;
}

void die_behavior () {
	fflush(output_file);
	fflush(output_vars);
	fclose(output_file);
	fclose(output_vars);
}

void message (enum message_type s) {
	fprintf(output_file, "%d %d %d %.2f %d\n", get_total_step(), get_day(), get_trial(), get_trial_time(), s);
	fflush(output_file);
}
