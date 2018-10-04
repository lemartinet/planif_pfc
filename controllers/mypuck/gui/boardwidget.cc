#include "boardwidget.hh"
#include "graphwidget.h"
#include "controlrobot.hh"
#include <iostream>

BoardWidget::BoardWidget (GraphWidget& widget, ControlRobot& robotgui) : widget_(widget), robotgui_(robotgui)
{ 
	setupUi (this); 
}

void BoardWidget::closeEvent (QCloseEvent *) {}

void BoardWidget::on_execution_behavior_run_clicked () {}

void BoardWidget::on_execution_behavior_pause_clicked () {}

void BoardWidget::on_execution_behavior_reset_clicked () {}

void BoardWidget::on_execution_view_activities_clicked () 
{
  	widget_.show ();
}

void BoardWidget::on_show_state_clicked () 
{
	robotgui_.show_state ();
}

void BoardWidget::on_show_inf_clicked () 
{
	robotgui_.show_inf ();
}

void BoardWidget::on_show_sup_clicked () 
{
	robotgui_.show_sup ();
}
