#ifndef GUI_H
# define GUI_H

#include "graphwidget.h"
#include "controlcols.hh"

class BoardWidget;
class ControlRobot;
class ControlCells;
class QApplication;
class QEventLoop;

class Behavior;
class RobotDevice;

class Gui
{
public:
	Gui (int& argc, char**& argv, Behavior& behavior);
	virtual ~Gui ();
	
	void synch ();
	
	GraphWidget* colswidget_get () { return colswidget_; }
	  
	ControlCols* colscontrol_get () { return colscontrol_; }

private:
	BoardWidget*   boardwidget_;
	GraphWidget*   colswidget_;
	ControlCols*   colscontrol_;
	ControlRobot*  robotcontrol_;
	ControlCells*  cellscontrol_;
	QApplication*  app_;
	QEventLoop*    eventloop_;
};

#endif
