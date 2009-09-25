/*
 * File:         world_manager.c
 * Date:         23 Mar 09
 * Description:  Supervise the simulation
 * Author:       fabien.rohrer@cyberbotics.com
 */

#include <webots/robot.h>
#include <webots/supervisor.h>
#include <webots/servo.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define TIME_STEP 32
#define MAX_DOORS 255
#define RAND_UPPER_BOUND 100
#define MAX_POSITION 0.2
#define MAX_FORCE 0.01

// modes
#define RANDOM 0
#define ALL_OPEN 1
#define ALL_CLOSE 2
#define Y_MAZE 3
#define NO_CONSTRAINTS 4

int main(int argc, char **argv)
{
  wb_robot_init();
  
  int mode = Y_MAZE;
  
  WbDeviceTag doors[MAX_DOORS]; 
  char door_text[8];
  int doors_counter=0;
  
  do {
    sprintf(door_text,"door%d",doors_counter);
    doors[doors_counter] = wb_robot_get_device(door_text);
    doors_counter++;
  } while (doors[doors_counter-1]!=0 || doors_counter==MAX_DOORS);
  
  int number_of_doors = doors_counter-1;
  
  int i;
  for (i=0; i<number_of_doors; i++) {
    wb_servo_set_motor_force(doors[i],MAX_FORCE);
    wb_servo_set_position(doors[i],MAX_POSITION);
  }
  
  srand(time(0));
  int r0 = rand() % RAND_UPPER_BOUND;
  
  int step_counter = 0;
  wb_robot_keyboard_enable(TIME_STEP);
  
  
  do {
    int k = wb_robot_keyboard_get_key();
    switch (k) {
      case 'R':
        mode = RANDOM;
        break;
      case 'O':
        mode = ALL_OPEN;
        break;
      case 'C':
        mode = ALL_CLOSE;
        break;
      case 'Y':
        mode = Y_MAZE;
        break;
      default:
        break;
    }
  
    switch (mode){
      case RANDOM:
        if (step_counter > r0 ) {
          step_counter=0;
          r0 = rand() % RAND_UPPER_BOUND;
          int r1 = rand() % number_of_doors;
          int r2 = rand() % number_of_doors;
          printf("open door%d\n",r1);
          printf("close door%d\n",r2);
          wb_servo_set_position(doors[r1],MAX_POSITION);
          wb_servo_set_position(doors[r2],0.0);
        } else step_counter++;
        break;
      case ALL_OPEN:
        for (i=0; i<number_of_doors; i++) {
          wb_servo_set_position(doors[i],MAX_POSITION);
        }
        mode = NO_CONSTRAINTS;
        break;
      case ALL_CLOSE:
        for (i=0; i<number_of_doors; i++) {
          wb_servo_set_position(doors[i],0.0);
        }
        mode = NO_CONSTRAINTS;
        break;
      case Y_MAZE:
        for (i=0; i<number_of_doors; i++) {
          if (i==1 || i==3 || i==5 ||
              i==7 || i==9 || i==11 ||
              i==13 || i==15 || i==17)
            wb_servo_set_position(doors[i],MAX_POSITION);
          else wb_servo_set_position(doors[i],0.0);
        }
        mode = NO_CONSTRAINTS;
        break;
      default:
        mode = NO_CONSTRAINTS;
        break;
    }
  } while (wb_robot_step(TIME_STEP) != -1);
  
  wb_robot_cleanup();
    
  return 0;
}
