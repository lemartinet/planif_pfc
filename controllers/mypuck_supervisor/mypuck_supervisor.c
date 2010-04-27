/*
 * File:         mypuck_supervisor.c
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
#include <webots/robot.h>

const char* robot_name = "ICEASIM";

bool read_simulation_params (char* param_file_path, int* id, int* random_move, int* final_forced, int* write_step, int* sleep_step)
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
    else if (strcmp (key, "FINAL_FORCED") == 0) {
      *final_forced = atoi(value);
    }
    else if (strcmp (key, "STEP_LOG") == 0) {
      *write_step = atoi(value);
    }
    else if (strcmp (key, "SLEEP_STEP") == 0) {
      *sleep_step = atoi(value);
    }
  }
  fclose (param_file);
  return true;
}


/////////////////////////////////
// Main functions
/////////////////////////////////

void init (int* random_move)
{
  int simu_id = 1, final_forced = 0, write_step = 1, sleep_step = 0;
  if (!read_simulation_params ("../../data/params.txt", 
        &simu_id, random_move, &final_forced, &write_step, &sleep_step)) {
		printf ("Supervisor: erreur d'init. Exit !\n");
		exit (-1);	
  }
  TIME_STEP = wb_robot_get_basic_time_step();
  printf("Supervisor: SIMULATION_ID %d\n", simu_id);
  
  init_behavior (simu_id, write_step);
  if (*random_move) {
    init_random ("../../data/index_visited");
  }
  init_protocol (simu_id, final_forced, sleep_step);  
  
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
  die_protocol ();
  die_behavior ();
  die_random ();
  wb_supervisor_simulation_quit();
}

void run ()
{
  run_behavior ();
  run_protocol ();
}

int main ()
{
  srand48 (time (0));
  wb_robot_init ();
  int random_move = 0;
  init (&random_move);
  do {
    if (random_move && learning_done ()) {
      // we open all the doors not to disturbe the robot
      open_all ();
      run_random ();
    }
    run ();
  } while (wb_robot_step (TIME_STEP) != -1);
  wb_robot_cleanup ();
  return 0;
}
