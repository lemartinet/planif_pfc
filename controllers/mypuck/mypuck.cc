#include "mypuck.hh"
#include "params.hh"
#include "behavior.hh"
#include "gui.hh"
#include "testy.hh"
//#include <webots/Robot.hpp>
#include <device/robot.h>

int ARGC;
char** ARGV;
Behavior* behavior; 
Gui* gui;
Testy* testy;
Params* params;

void reset (void)
{
	params = new Params ("../../data/params.txt");
  	//params->show ();
  	  	
  	behavior = new Behavior;

  	static const int GUI = params->get_int("GUI");
  	if (GUI) {
  		gui = new Gui (ARGC, ARGV, *behavior);
  	}
  	
  	testy = new Testy(*behavior);
  	printf("Mypuck: reset ok\n");
}

int run (int)
{
	static const int TIME_STEP = params->get_int("TIME_STEP");
	static const int GUI = params->get_int("GUI");

  	behavior->synch ();
	if (GUI) {
  		gui->synch ();
	}
  	testy->synch ();

  	return TIME_STEP;
}

void die ()
{
	delete params;
	delete behavior;
	delete testy;
	delete gui;
	printf("Mypuck: die ok\n");
}

int main (int argc, char** argv)
{
	ARGC = argc;
  	ARGV = argv;
  	
  	srand48 (time (0));

//	Robot* robot = Robot::getInstance ();
//	robot->live (reset);
//	robot->die (die);
//	robot->run (run);

  	robot_live(reset);
  	robot_die(die);
  	robot_run(run);

  	return 0;
}
