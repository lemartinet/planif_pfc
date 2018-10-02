#include "randommove.h"
#include "mypuck_supervisor.h"
#include <webots/supervisor.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * This module is intended to work with mypuck_tolman.wbt
 */


////////////////////////////////////////////
// Functions to generate random moves
////////////////////////////////////////////

int wait_random_move = 0;
int nb_random_move = 0;
int size_places = 0;
int** places = NULL;

// geometry of the maze (à mettre dans les params)
float xMin = -0.68;  
float yMin = -1.7;
float Step = 0.049;

// load a vector places such as places[i] = {i,j}
// with i,j = row,col of the discretized world, visited
// by the animat
void init_random (char* places_path) {
  FILE* places_file = fopen (places_path, "r");
  char key[128];
	char value[128];
  int i;
	printf ("Supervisor: loading indexes of places from file: %s\n", places_path);
  
	if (places_file == NULL) {
    printf ("Supervisor: unable to open: %s\n", places_path);
		return;
  }
  int unused_ret = fscanf (places_file, "%s %s", key, value);
  size_places = atoi (value);
  places = malloc (size_places * sizeof(int*));
	for (i = 0; i < size_places; i++) {
    unused_ret = fscanf (places_file, "%s %s", key, value);
    places[i] = malloc (2 * sizeof(int));
    places[i][0] = atoi(key);
    places[i][1] = atoi(value);
  }

  // we put elements of places in a random order
  for (i = 0; i < size_places; i++) {
    int d = (int)(size_places * drand48 ());
    int tmp1 = places[d][0], tmp2 = places[d][1];
    places[d][0] = places[i][0];
    places[d][1] = places[i][1];
    places[i][0] = tmp1;
    places[i][1] = tmp2;
  }
}

void move_robots () {
  int x_case = places[nb_random_move % size_places][0];
  int y_case = places[nb_random_move % size_places][1];
  double x = xMin + x_case * Step;// + Step * drand48 ();
  double y = yMin + y_case * Step;// + Step * drand48 ();
  double new_pos [] = {x, 0, y};
  wb_supervisor_field_set_sf_vec3f (t_field, new_pos);
}

void die_random () {
  int i;
  if (places == NULL) {
    return;
  }
  for (i = 0; i < size_places; i++) {
    free (places[i]);
  }
  free (places);
}

void run_random () {
  // we move randomly the robot in the maze after
  // the 6th trial of the 1st day
  if (wait_random_move == 0 && nb_random_move < 10000) {
    wait_random_move = 9;
    nb_random_move++;
    move_robots();
  }
  else if (nb_random_move == 10000) {
    wb_supervisor_simulation_quit ();
  }
  else {
    wait_random_move--;
  }
  
  run ();
}
