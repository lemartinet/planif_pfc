#ifndef GUI_H
# define GUI_H

#include <QtGui>

class BoardWidget;
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
	QEventLoop*    eventloop_;
};

#endif
