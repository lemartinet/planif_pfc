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
WbDeviceTag dsensor_way1, dsensor_way2, dsensor_way3, dsensor_way0;

// output files and variables for behavior analysis
FILE * output_file = NULL, * output_vars = NULL;
int write_step;


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
	double diff2 = fabs(ecart_angulaire (orientation, M_PI/2.0));
	double diff3 = fabs(ecart_angulaire (orientation, M_PI));
	double diff4 = fabs(ecart_angulaire (orientation, -M_PI/2.0));

	double dway1 = wb_distance_sensor_get_value (dsensor_way1);
	double dway2 = wb_distance_sensor_get_value (dsensor_way2);
	double dway3 = wb_distance_sensor_get_value (dsensor_way3);
	double dway0 = wb_distance_sensor_get_value (dsensor_way0);

	double diff[] = {diff1, diff1, diff2, diff4, diff3, diff3, diff4, diff2};
	double dway[] = {dway0, dway1, dway2, dway3, dway0, dway1, dway2, dway3};
	int i, the_path;
	for (i = 0; i < 8; i++) {
		if (current_way != i && dway[i] < 500 && diff[i] < 0.2) {
			the_path = i<4?i:i-4;
			the_path = the_path == 0 ? 4 : the_path;
			the_path = (i < 4 ? 1 : -1) * the_path;
			if (first_way == -1 && i != 0 && i != 4 &&
					(protocol_state <= FREE || (protocol_state > FREE && i != 1 && i != 5))) {
				//printf("fw : %d %d %d\n", first_way, i, protocol_state);
				first_way = i;
			}
			current_way = i;
			message (the_path);
			break;
		}
	}

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
	fprintf(output_file, "%s event = [-]1/2/3/4=[-]wayX (current direction), 5=goal reached, 6=exceeded time, 7=sleep\n", "%");

	dsensor_way1 = wb_robot_get_device ("sensor0");
	dsensor_way2 = wb_robot_get_device ("sensor_way2_start");
	dsensor_way3 = wb_robot_get_device ("sensor_way3_start");
	dsensor_way0 = wb_robot_get_device ("sensor_way0_start");
	wb_distance_sensor_enable (dsensor_way1, TIME_STEP);
	wb_distance_sensor_enable (dsensor_way2, TIME_STEP);
	wb_distance_sensor_enable (dsensor_way3, TIME_STEP);
	wb_distance_sensor_enable (dsensor_way0, TIME_STEP);
	current_way = -1;
	first_way = -1;
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
