#include "params.hh"
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

	// perform a loop simulation steps
	const int TIME_STEP = robot->getBasicTimeStep();
	do {
		behavior->synch ();
	} while (robot->step (TIME_STEP) != -1);

	delete behavior;
	Logger::close_logs ();
	delete robot;
	return 0;
}
