#include "params.hh"
#include "behavior.hh"
#include "device.hh"
#include "logger.hh"
#include <cstdlib>
#include <iostream>

int main (int argc, char** argv)
{
	srand (time (0));
	if (!Logger::open_logs ()) {
		cout << "Mypuck: erreur d'init. Exit !" << endl;
		exit (-1);	
	}
	Behavior* behavior = new Behavior;

	// perform a loop simulation steps
	const int TIME_STEP = Params::get_int ("TIME_STEP");
	do {
		behavior->synch ();
	} while (const_cast<RobotDevice&>(RobotDevice::robot_get ()).step (TIME_STEP) != -1);

	delete behavior;
	Logger::close_logs ();
	return 0;
}
