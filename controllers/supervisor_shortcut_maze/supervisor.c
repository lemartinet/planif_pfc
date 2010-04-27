/*
 * File:         mypuck_supervisor.c
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

bool read_simulation_params (char* param_file_path, int* time_step, int* id, int* write_step, int* shortcut) 
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
    else if (strcmp (key, "STEP_LOG") == 0) {
      *write_step = atoi(value);
    }
    else if (strcmp (key, "TIME_STEP") == 0) {
      *time_step = atoi(value);
    }
    else if (strcmp (key, "INDIRECT_SHORTCUT") == 0) {
      *shortcut = atoi(value);
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
  int simu_id = 1, write_step = 1, shortcut = 0;
  if (!read_simulation_params ("../../data/params.txt", 
        &TIME_STEP, &simu_id, &write_step, &shortcut)) {
		printf ("Supervisor: erreur d'init. Exit !\n");
		exit (-1);	
	}
  printf("Supervisor: SIMULATION_ID %d\n", simu_id);
  
  init_behavior (simu_id, write_step);
  init_protocol (simu_id, shortcut);  
  
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
