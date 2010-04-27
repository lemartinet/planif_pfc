#include <iostream>
#include <QThread>
#include "params.hh"
#include "behavior.hh"
#include "device.hh"
#include "logger.hh"

class Computation: public QThread
{
public:
	Computation() {
		robot = new RobotDevice;
		srand (time (0));
		if (!Logger::open_logs ()) {
			cout << "Mypuck: erreur d'init. Exit !" << endl;
			QApplication::exit(-1);
		}
		behavior = new Behavior(*robot);
	}

	Behavior& behavior_get() { return *behavior; }

private:
	void run() {
		// perform a loop simulation steps
		const int TIME_STEP = robot->getBasicTimeStepParams();
		do {
			behavior->synch ();
		} while (robot->step (TIME_STEP) != -1);
		delete behavior;
		Logger::close_logs ();
		delete robot;
		QApplication::exit();
	}

private:
	Behavior* behavior;
	RobotDevice* robot;
};
