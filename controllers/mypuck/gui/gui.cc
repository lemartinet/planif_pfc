#include "gui.hh"
#include <QtGui>
#include "controlrobot.hh"
#include "controlcells.hh"
#include "boardwidget.hh"
#include "behavior.hh"

Gui::Gui (int& argc, char**& argv, Behavior& behavior)
{
  	app_ = new QApplication (argc, argv);
  	eventloop_ = new QEventLoop ();
  	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
  	boardwidget_ = new BoardWidget (*this);
  	colswidget_ = new GraphWidget ();
  	boardwidget_->show ();
  	//colswidget_->show ();
  	colscontrol_ = new ControlCols (behavior.neurosolver_get (), *colswidget_);
  	robotcontrol_ = new ControlRobot (behavior.robot_get (), *colswidget_);
  	cellscontrol_ = new ControlCells (behavior.neurosolver_get ().hippo_get (), *colswidget_);
}

Gui::~Gui ()
{
	delete cellscontrol_;
	delete robotcontrol_;
	delete colscontrol_;
	delete colswidget_;
	delete boardwidget_;
	delete eventloop_;
	delete app_;
}

void Gui::synch ()
{
	if (colscontrol_) {
		colscontrol_->update ();
	}
	if (robotcontrol_) {
		robotcontrol_->update ();
	}
	if (cellscontrol_) {
		cellscontrol_->update ();
	}
	
	if (eventloop_) {
		eventloop_->processEvents(QEventLoop::AllEvents);
	}
}
