#ifndef GUI_H
# define GUI_H

#include <QtGui>

class GraphWidget;
class BoardWidget;
class ControlRobot;
class Behavior;
class QApplication;
class QEventLoop;

class Gui : public QApplication
{
public:
	Gui (int& argc, char**& argv, Behavior& behavior);
	~Gui ();
	
	void synch ();
	  
private:
	BoardWidget*   boardwidget_;
	GraphWidget*   colswidget_;
	ControlRobot*  robotcontrol_;
	QEventLoop*    eventloop_;
};

#endif
