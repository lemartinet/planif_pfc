#ifndef GUI_H
# define GUI_H

#include <QtGui>
#include "graphwidget.h"
#include "controlcols.hh"
#include "controlrobot.hh"
#include "controlcells.hh"
#include "columns.hh"
#include "boardwidget.hh"

class Behavior;

class Gui
{
public:
  Gui () : boardwidget_(0), colswidget_(0), colscontrol_(0), robotcontrol_(0), cellscontrol_(0), app_(0), eventloop_(0) {};
  ~Gui ();

  void init (int& argc, char**& argv, Behavior& behavior, RobotDevice& robot);
  void draw ();
  void update ();

  inline
  GraphWidget* colswidget_get () { return colswidget_; }
  
  inline
  ControlCols* colscontrol_get () { return colscontrol_; }

private:
  void clean ();

  BoardWidget*   boardwidget_;
  GraphWidget*   colswidget_;
  ControlCols*   colscontrol_;
  ControlRobot*  robotcontrol_;
  ControlCells*  cellscontrol_;
  QApplication*  app_;
  QEventLoop*    eventloop_;
};

#endif
