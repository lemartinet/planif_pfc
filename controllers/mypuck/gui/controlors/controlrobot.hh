#ifndef CONTROLROBOT_HH
# define CONTROLROBOT_HH

#include "graphwidget.h"

class RobotDevice;

class ControlRobot
{
public:
  ControlRobot (RobotDevice& robot_device, GraphWidget& widget);
  ~ControlRobot () {};

  void update ();

private:
  RobotDevice*  robot_device_;
  GraphWidget*  widget_;
  Node*         robot_;
};

#endif
