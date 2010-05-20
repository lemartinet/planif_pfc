/*
 * Description:  Supervisor for webots 
 * Author:       louis-emmanuel.martinet@upmc.fr
 */
#include "supervisor.h"
#include "analysebehavior.h"
#include "protocol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <webots/robot.h>

const char* robot_name = "ICEASIM";

bool read_simulation_params (char* param_file_path, int* id, int* write_step, int* sleep_step)
{
	FILE * param_file = fopen (param_file_path, "r");
	char key[128];
	char value[128];

	if (param_file == NULL) {
		printf ("Supervisor: unable to open: %s\n", param_file_path);
		return false;
	}
	printf ("Supervisor: loading params from file: %s\n", param_file_path);
	//while (!feof (param_file)) {
	while (fscanf (param_file, "%s %s", key, value) != EOF) {
		if (strcmp(key, "SIMULATION_ID") == 0) {
			*id = atoi(value);
		} else if (strcmp(key, "STEP_LOG") == 0) {
			*write_step = atoi(value);
		} else if (strcmp(key, "SLEEP_STEP") == 0) {
			*sleep_step = atoi(value);
		} else if (strcmp(key, "SLEEP_FREQ") == 0) {
			sleep_freq = atoi(value);
		}
	}
	fclose (param_file);
	return true;
}


/////////////////////////////////
// Main functions
/////////////////////////////////

void init ()
{
	int simu_id = 1, write_step = 1, sleep_step = 0;
	if (!read_simulation_params ("../../data/params.txt", &simu_id, &write_step, &sleep_step)) {
		printf ("Supervisor: erreur d'init. Exit !\n");
		exit (-1);	
	}
	TIME_STEP = wb_robot_get_basic_time_step();
	printf("Supervisor: SIMULATION_ID %d\n", simu_id);

	init_behavior (simu_id, write_step);
	init_protocol (simu_id, sleep_step);

	// We get a handler to the robot & the start position
	robot = wb_supervisor_node_get_from_def (robot_name);
	if (robot != NULL) {
		// we get position and orientation of the robot
		t_field = wb_supervisor_node_get_field (robot, "translation");
		r_field = wb_supervisor_node_get_field (robot, "rotation");
	}
	else {
		printf ("Error: node %s not found\n", robot_name);
	}
}


void supervisor_die () {
	die_protocol ();
	die_behavior ();
	wb_supervisor_simulation_quit();
}


int main ()
{
	srand48 (time (0));
	wb_robot_init ();
	init ();
	do {
		run_behavior ();
		run_protocol ();
	} while (wb_robot_step (TIME_STEP) != -1);
	wb_robot_cleanup ();
	return 0;
}
