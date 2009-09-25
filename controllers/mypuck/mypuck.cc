#include <iostream>
#include <fstream>
#include <sstream>
#include <device/robot.h>
#include "mypuck.hh"
#include "params.hh"
#include "behavior.hh"
#include "device.hh"
#include "gui.hh"
#include "testy.hh"

// valeur en dure dans :
//- ici
//- behavior.cc
//- graphwidget.cc

int ARGC;
char** ARGV;
RobotDevice* robot;
Behavior* behavior; 
Gui* gui;
Testy* testy;
Params* params;

void reset(void)
{
	delete params;
	params = new Params ("../../data/params.txt");
  	//params->show ();
	
	delete robot;
  	robot = new RobotDevice;
  	robot->init ();
  	
  	delete behavior;
  	behavior = new Behavior;
  	behavior->reset ();
  	
  	static const int GUI = params->get_int("GUI");
  	if (GUI) {
  		delete gui;
  		gui = new Gui;
  		gui->init (ARGC, ARGV, *behavior, *robot);
  	}
  	
  	delete testy;
  	testy = new Testy(*robot, *behavior, behavior->neurosolver_get());
  	testy->init ();
}

int run (int)
{
	static const int TIME_STEP = params->get_int("TIME_STEP");
	static const int GUI = params->get_int("GUI");

  	robot->synch (); 
  	behavior->compute_next_action (*robot);
  	behavior->do_action (*robot); 

	if (GUI) {
  		gui->update ();
  		gui->draw ();
	}

  	testy->synch ();

  	return TIME_STEP;
}

void die()
{
	testy->end_simu();
/*	delete params;
	delete robot;
	delete behavior;
	delete gui;
	delete testy;
*/
}

int main (int argc, char** argv)
{
	ARGC = argc;
  	ARGV = argv;
  	
  	srand48 (time (0));

  	robot_live(reset);
  	robot_die(die);
  	robot_run(run);

  	return 0;
}
