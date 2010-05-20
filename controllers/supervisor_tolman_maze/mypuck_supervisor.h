#ifndef MYPUCKSUPERVISOR_H_
#define MYPUCKSUPERVISOR_H_

#include <webots/supervisor.h>

int TIME_STEP;

// The robot & its positions and orientation
WbNodeRef robot;
WbFieldRef t_field;
WbFieldRef r_field;

void supervisor_die ();

#endif /*MYPUCKSUPERVISOR_H_*/
