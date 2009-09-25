#include "controlrobot.hh"
#include "device.hh"

ControlRobot::ControlRobot (RobotDevice& robot_device, GraphWidget& widget)
{
  robot_device_ = &robot_device;
  widget_ = &widget;
  robot_ = widget.add_node (ROBOT, 1, robot_device.position_get (), 20, Qt::cyan);
}

void ControlRobot::update ()
{
  robot_->move (robot_device_->position_get ());
}

