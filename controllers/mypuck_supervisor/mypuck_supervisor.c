/*
 * File:         mypuck_supervisor.c
 * Date:         Novembre 2008
 * Description:  Supervisor for webots 
 * Author:       louis-emmanuel.martinet@upmc.fr
 */
#include "mypuck_supervisor.h"
#include "randommove.h"
#include "analysebehavior.h"
#include "protocol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <webots/robot.h>
#include <webots/emitter.h>
#include <unistd.h>

/////////////////////////////////
// Variables
/////////////////////////////////

const char* robot_name = "MYPUCK";

// Webots data
//WbDeviceTag emitter;

// Id of the simulation
int simu_id = 1;

// Type of simulation (normal or random move)
int random_move = 0;

<<<<<<< HEAD
bool read_simulation_params (char* param_file_path, int* time_step, int* id, int* random_move) 
=======
bool read_simulation_params (char* param_file_path, int* time_step, int* id, int* random_move, int* final_forced) 
>>>>>>> 16cde28... First commit to git. Includes:
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
    if (strcmp (key, "SIMULATION_ID") == 0) {
      *id = atoi(value);
    }
    else if (strcmp (key, "RANDOM_MOVE") == 0) {
      *random_move = atoi(value);
    }
<<<<<<< HEAD
=======
    else if (strcmp (key, "FINAL_FORCED") == 0) {
      *final_forced = atoi(value);
    }
>>>>>>> 16cde28... First commit to git. Includes:
    else if (strcmp (key, "STEP_LOG") == 0) {
      int write_step = atoi(value);
      write_step_set (write_step);
    }
    else if (strcmp (key, "SLEEP_STEP") == 0) {
      int sleep_step = atoi(value);
      sleep_step_set (sleep_step);
    }
    else if (strcmp (key, "TIME_STEP") == 0) {
      *time_step = atoi(value);
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
<<<<<<< HEAD
  if (!read_simulation_params ("../../data/params.txt", &TIME_STEP, &simu_id, &random_move)) {
=======
  int final_forced = 0;
  if (!read_simulation_params ("../../data/params.txt", &TIME_STEP, &simu_id, &random_move, &final_forced)) {
>>>>>>> 16cde28... First commit to git. Includes:
		printf ("Supervisor: erreur d'init. Exit !\n");
		exit (-1);	
	}
  printf("Supervisor: SIMULATION_ID %d\n", simu_id);
  
  init_behavior (simu_id);
  if (random_move) {
    init_random ("../../data/index_visited");
  }
<<<<<<< HEAD
  init_protocol ();
=======
  init_protocol (simu_id, final_forced);
>>>>>>> 16cde28... First commit to git. Includes:
  //emitter = robot_get_device("emitter");
  
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
  
  // can be use to select another controller for a robot
  // supervisor_robot_set_controller(robot, "braitenberg");

  // can be use to import another robot onto the maze
  // supervisor_import_node("icea_rat.wbt", -2);
  // run an empty step in order to update the world with the imported node
  // robot_step(0);
}

void supervisor_die () {
  int day,trial;
  die_protocol (&day, &trial);
  die_behavior (day, trial);
  die_random ();
  wb_supervisor_simulation_quit();
}

void run ()
{
  static int day=1, trial=1, total_step=1;
  static double trial_time=0;
  analyse_behavior (day, trial, trial_time, total_step);
  run_protocol (&day, &trial, &trial_time, &total_step);
}

int main ()
{
  srand48 (time (0));
  wb_robot_init ();
  init ();
  do {
    if (random_move && learning_done ()) {
      // we open all the doors not to disturbe the robot
      open_all ();
      run_random ();
    }
    else {
      run ();
    }
  } while (wb_robot_step (TIME_STEP) != -1);
  wb_robot_cleanup ();
  return 0;
}
