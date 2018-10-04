#include "gui.hh"
#include "graphwidget.h"
#include "controlrobot.hh"
#include "boardwidget.hh"
#include "behavior.hh"

Gui::Gui (int& argc, char**& argv, Behavior& behavior) : QApplication (argc, argv)
{
  	eventloop_ = new QEventLoop ();
  	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
  	colswidget_ = new GraphWidget ();
  	robotcontrol_ = new ControlRobot (behavior.robot_get (), behavior.neurosolver_get ().hippo_get (), 
  		behavior.neurosolver_get (), *colswidget_);
	boardwidget_ = new BoardWidget (*colswidget_, *robotcontrol_);
  	boardwidget_->show ();
}

Gui::~Gui ()
{
	delete robotcontrol_;
	delete colswidget_;
	delete boardwidget_;
	delete eventloop_;
}

void Gui::synch ()
{
	if (robotcontrol_) {
		robotcontrol_->update ();
	}
	if (eventloop_) {
		eventloop_->processEvents(QEventLoop::AllEvents);
	}
}
