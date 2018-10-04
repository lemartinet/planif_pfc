#include "params.hh"
#include "gui.hh"
#include "behavior.hh"
#include "device.hh"
#include "logger.hh"
#include <cstdlib>
#include <iostream>

int main (int argc, char** argv)
{
	srand (time (0));
	Params::load ("../../data/");
	Logger::open_logs ();
	
	Behavior* behavior = new Behavior;
	const int GUI = Params::get_int ("GUI");
	Gui* gui = GUI ? new Gui (argc, argv, *behavior) : 0;

	// perform a loop simulation steps
	const int TIME_STEP = Params::get_int ("TIME_STEP");
	do {
		behavior->synch ();
		if (GUI) {
			gui->synch ();
		}
	} while (behavior->robot_get ().step (TIME_STEP) != -1);

	delete gui;
	delete behavior;
	Logger::close_logs ();
	return 0;
}
