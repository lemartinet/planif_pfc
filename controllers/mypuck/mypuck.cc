#include "params.hh"
#include "gui.hh"
#include "behavior.hh"
#include "device.hh"
#include "logger.hh"
#include <cstdlib>
#include <iostream>

int main (int argc, char** argv)
{
	RobotDevice* robot = new RobotDevice;
	srand (time (0));
	if (!Logger::open_logs ()) {
		cout << "Mypuck: erreur d'init. Exit !" << endl;
		exit (-1);	
	}
	Behavior* behavior = new Behavior(*robot);
	const int GUI = Params::get_int ("GUI");
	Gui* gui = GUI ? new Gui (argc, argv, *behavior) : 0;

	// perform a loop simulation steps
	const int TIME_STEP = robot->getBasicTimeStep();
	do {
		behavior->synch ();
		if (GUI) {
			gui->synch ();
		}
	} while (robot->step (TIME_STEP) != -1);

	if (gui) {
		delete gui;
	}
	delete behavior;
	Logger::close_logs ();
	delete robot;
	return 0;
}
