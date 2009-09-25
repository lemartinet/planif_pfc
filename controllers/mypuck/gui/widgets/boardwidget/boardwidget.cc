#include "gui.hh"
#include "boardwidget.hh"
#include <iostream>


void BoardWidget::closeEvent (QCloseEvent *) {}

void BoardWidget::on_execution_behavior_run_clicked () {}

void BoardWidget::on_execution_behavior_pause_clicked () {}

void BoardWidget::on_execution_behavior_reset_clicked () {}

void BoardWidget::on_execution_view_activities_clicked () 
{
  	gui_->colswidget_get ()->show ();
}

void BoardWidget::on_show_state_clicked () 
{
	gui_->colscontrol_get ()->show_state ();
}

void BoardWidget::on_show_inf_clicked () 
{
	gui_->colscontrol_get ()->show_inf ();
}

void BoardWidget::on_show_sup_clicked () 
{
	gui_->colscontrol_get ()->show_sup ();
}
