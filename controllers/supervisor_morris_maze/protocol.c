/*
 * water maze experiment
 * Opens the doors, moves the robot to the start position, ...
 * louis-emmanuel.martinet@upmc.fr
 */
#include "protocol.h"
#include "analysebehavior.h"
#include "supervisor.h"
#include <webots/distance_sensor.h>
#include <webots/emitter.h>
#include <webots/supervisor.h>
#include <webots/robot.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// Webots devices: servos, sensors and emitter
#define NBSENSORS 1
WbDeviceTag sensors[NBSENSORS];
WbDeviceTag emitter;
enum sensor_name {  s_goal = 0 };

// output file for protocol observations
FILE * output_protoc = NULL;
// trials informations
int trial = 1;
int day = 1;
double trial_time;
int total_step = 1;
// The start & goal points
const char* start_name = "START";
double* start_position;
double* start_rotation;
const char* goal_name = "GOAL";
double* goal_position;
// Sleep parameter
int SLEEP_STEP;
// stuff to create an automaton for the robot
enum robot_states { SEARCHING, GOALFOUND, SLEEPING } robot_state;
int wait = -1;

#define MEAN_SIZE 10
const double SUCCESS_SWITCH = 0.7;
int success[MEAN_SIZE];
int success_idx = 0;
FILE * output_success = NULL;


void reinit_success() {
	int i;
	for (i = 0; i < MEAN_SIZE; i++) {
		success[i] = 0;
	}
}

double mean_success() {
	int i;
	double sum = 0;
	for (i = 0; i < MEAN_SIZE; i++) {
		sum += success[i];
	}
	return sum / MEAN_SIZE;
}

void message_protocol (enum protocol_type s) {
	fprintf(output_protoc, "%d %d %d %d\n", total_step, day, trial, s);
	fflush(output_protoc);
}

void new_trial(){
/*	if (protocol_state == P1) {
		// change the starting point
	}
*/
	message_protocol(protocol_state);
}

void init_protocol (int simu_id, int sleep) {
	char output_path[128];
	sprintf (output_path, "../../data/data_raw/%d/protocol.txt", simu_id);
	output_protoc = fopen (output_path, "w");
	fprintf(output_protoc, "%s format : total_step day trial type\n", "%");
	fprintf(output_protoc, "%s type : 1=free \n", "%");

	sprintf (output_path, "../../data/data_raw/%d/success.txt", simu_id);
	output_success = fopen (output_path, "w");
	fprintf(output_success, "%s format: day, trial, success, mean\n", "%");

	// retrieve device handles
	int i;
	char sensor_name[] = "sensor0";
	for(i = 0; i < NBSENSORS; i++){
		sensors[i] = wb_robot_get_device (sensor_name);
		wb_distance_sensor_enable (sensors[i], TIME_STEP);
		sensor_name[6]++;
	}
	emitter = wb_robot_get_device ("emitter");

	// retrieve the start position
	WbNodeRef start_point = wb_supervisor_node_get_from_def (start_name);
	if (start_point != NULL) {
		WbFieldRef t_start = wb_supervisor_node_get_field (start_point, "translation");
		WbFieldRef r_start = wb_supervisor_node_get_field (start_point, "rotation");
		start_position = (double*) wb_supervisor_field_get_sf_vec3f (t_start);
		start_rotation = (double*) wb_supervisor_field_get_sf_rotation (r_start);
		//printf("Protocol: %f %f %f\n", start_position[0], start_position[2], start_rotation[3]);
	}
	else {
		printf("Error: node %s not found\n", start_name);
	}
	// retrieve the goal position
	WbNodeRef goal_point = wb_supervisor_node_get_from_def (goal_name);
	if (goal_point != NULL) {
		WbFieldRef t_goal = wb_supervisor_node_get_field (goal_point, "translation");
		goal_position = (double*) wb_supervisor_field_get_sf_vec3f (t_goal);
		goal_position[2] += 0.04;
	}
	else {
		printf("Error: node %s not found\n", goal_name);
	}

	trial_time = 0.0;
	SLEEP_STEP = sleep;
	protocol_state = FREE;
	reinit_success();
	new_trial();
	wb_robot_step (TIME_STEP);
}

void observe_robot() {
	int i;
	int value[NBSENSORS];
	for(i = 0; i < NBSENSORS; i++) {
		value[i] = wb_distance_sensor_get_value (sensors[i]);
	}
	if (robot_state == SEARCHING && value[s_goal] < 900) {
		robot_state = GOALFOUND;
		wait = 100;
		char msg [] = "goal";
		wb_emitter_send (emitter, msg, strlen(msg) + 1);
		message (GOAL);

		success[success_idx] = 1;
		fprintf(output_success, "%d %d %d %f\n", day, trial,
				success[success_idx], mean_success());
		fflush(output_success);
		success_idx = (success_idx + 1) % MEAN_SIZE;
	}
}

void run_protocol () {
	// printing the simulation stats
	char score[128];
	sprintf(score, "Day %d, Trial %d : %s", day, trial, robot_state == SLEEPING ? "Sleep" : "Running");
	wb_supervisor_set_label (0, score, 0.01, 0.01, 0.05, 0x0000ff, 0);
	sprintf(score, "Time %d s", (int)trial_time);
	wb_supervisor_set_label (1, score, 0.01, 0.05, 0.05, 0x0000ff, 0);

	observe_robot();

	trial_time += (float) TIME_STEP / 1000;
	total_step++;

	// sending the distance of the robot to the goal
	const double* translation = wb_supervisor_field_get_sf_vec3f (t_field);
	double dist[] = {translation[0] - goal_position[0], translation[2] - goal_position[2]};
	char dist_msg[128];
	sprintf(dist_msg, "%f", sqrt(dist[0] * dist[0] + dist[1] * dist[1]));
	wb_emitter_send (emitter, dist_msg, strlen(dist_msg) + 1);

	if (robot_state == GOALFOUND && wait == -1 && (trial % sleep_freq == 0)) {
		robot_state = SLEEPING;
		wait = SLEEP_STEP;
		if (SLEEP_STEP > 0 ) {
			message (SLEEP);
			char msg [] = "sleep";
			wb_emitter_send (emitter, msg, strlen(msg) + 1);
		}
	}
	else if (robot_state == SEARCHING && trial_time > 1000) {
		// maximum time reached
		message (EXCEED);
		// we move the animat at the goal position
		wb_supervisor_field_set_sf_vec3f (t_field, goal_position);
		wb_supervisor_field_set_sf_rotation (r_field, start_rotation);
	}
	else if ((robot_state == GOALFOUND || robot_state == SLEEPING) && wait == -1) {
		// Reset the maze & robot
		wb_supervisor_field_set_sf_vec3f (t_field, start_position);
		wb_supervisor_field_set_sf_rotation (r_field, start_rotation);
		trial++;
		// Let's change the protocol pĥase
		/*
		if (protocol_state == BLOCB && mean_success() >= SUCCESS_SWITCH){
			protocol_state = P1;
			day++;
			trial = 1;
			reinit_success();
		} else if (protocol_state == P1 || protocol_state == P2 || protocol_state == P3) {
			protocol_state++;
			reinit_success();
		} else if (mean_success() >= SUCCESS_SWITCH){
			protocol_state++;
			reinit_success();
		}*/

		// When all the trials are done, we save the controller and quit webots
		if (day > 4){
			supervisor_die ();
		}
		new_trial();
		trial_time = 0;
		robot_state = SEARCHING;
	}
	if (wait >= 0) {
		wait--;
	}
}

void die_protocol () {
	fflush(output_protoc);
	fclose(output_protoc);
	fflush(output_success);
	fclose(output_success);
}

int get_trial(){
	return trial;
}

int get_day(){
	return day;
}

int get_total_step(){
	return total_step;
}

double get_trial_time(){
	return trial_time;
}
