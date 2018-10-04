#include "params.hh"
#include "behavior.hh"
#include "gui.hh"
#include "device.hh"
#include <iostream>

int main (int argc, char** argv)
{
	srand (time (0));
	Params::load ("../../data/");
	
	Behavior* behavior = new Behavior;
	const int GUI = Params::get_int ("GUI");
	Gui* gui = GUI ? new Gui (argc, argv, *behavior) : 0;
	behavior->robot_get ().output_mypuck ("Mypuck: init ok");

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
	return 0;
}
