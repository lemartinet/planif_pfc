#include "boardwidget.hh"
#include "behavior.hh"
#include <iostream>

BoardWidget::BoardWidget (Behavior& behavior) : 
	robotgui_(behavior.robot_get (), behavior.neurosolver_get ().hippo_get (), behavior.neurosolver_get ())
{ 
	setupUi (this);

	robotgui_.widget_get ()->setParent (network);
	robotgui_.widget_get ()->move (230,10);
}

void BoardWidget::closeEvent (QCloseEvent *) {}

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

void BoardWidget::on_show_pc_clicked () 
{
	robotgui_.show_pc ();
}

void BoardWidget::on_show_col_clicked () 
{
	robotgui_.show_col ();
}
