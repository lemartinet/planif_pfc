#include "controlrobot.hh"
#include "graphwidget.h"
#include "device.hh"
#include "cell.hh"
#include "hippo.hh"
#include "neurosolver.hh"
#include "columns.hh"
#include "column.hh"
#include "minicol.hh"
#include "params.hh"
#include "edge.h"

ControlRobot::ControlRobot (const RobotDevice& robot_device, const Hippo& hippo, Neurosolver& neuro, GraphWidget& widget) :
	robot_device_(robot_device), hippo_(hippo), neuro_(neuro), widget_(widget)  
{
	robot_ = widget.add_node (ROBOT, 1, robot_device.position_get (), 20, Qt::cyan);
	QObject::connect (&hippo_, SIGNAL (sig_addcell (int)), this, SLOT (slot_addcell (int)));
	show_inf_sup_or_state_ = 0;
	const Columns* cols = &(neuro_.cols_get ());
	QObject::connect (cols, SIGNAL (sig_addcol (int)), this, SLOT (slot_addcol (int)));
	QObject::connect (cols, SIGNAL (sig_addlink (int, int)), this, SLOT (slot_addlink (int, int)));
	QObject::connect (&widget_, SIGNAL (sig_node_clicked (int)), this, SLOT (slot_setgoalcol (int)));
}

void ControlRobot::update ()
{
	update_cols ();
	robot_->move (robot_device_.position_get ());
	update_cells ();
}

void ControlRobot::update_cells ()
{
	if (SHOW_CELLS) {
		int size = hippo_.size ();
		for (int i = 0; i < size; i++) {
			const Cell& cell = hippo_.cell_get (i);
			Node* node = widget_.node_get (CELL, cell.no_get ());
			node->move (cell.pos_get ());
			node->lightlevel_set (cell.output ());
		}
	}
}

void ControlRobot::slot_addcell (int no)
{
	Coord coord (0, 0);
	if (SHOW_CELLS) {
		widget_.add_node (CELL, no, coord, 15, Qt::darkYellow);
	}
}

void ControlRobot::update_col_appearance (Node& node, Column& col)
{
	if (show_inf_sup_or_state_ == 2) {
		node.color_set (COLOR_GOAL);
		node.lightlevel_set (col.sup_activation ());
    }
    else if (show_inf_sup_or_state_ == 1) {
    	node.color_set (COLOR_STATE);
    	node.lightlevel_set (col.inf_activation ());
    }
    else if (show_inf_sup_or_state_ == 0) {
    	node.color_set (COLOR_STATE);
    	node.lightlevel_set (col.state_activation ());
    }
}

void ControlRobot::update_mincol_appearance (Edge& edge, Minicol& mincol)
{
	if (mincol.spiking ()) {
		edge.arrow_activ (true);
	}
	else {
		edge.arrow_activ (false);
	}
	edge.color_set (COLOR_STATE);
//	edge.lightlevel_set (state_link_activation);
}

void ControlRobot::update_mincols_appearance ()
{
	const Columns& cols = neuro_.cols_get();
	int size = cols.minicol_size ();
	for (int i = 0; i < size; i++) {
		Minicol* minicol = cols.minicol_get (i);
		if (minicol == 0 || !minicol->recruited_get ()) {
			continue;
		}
		Edge* edge = widget_.edge_get (COL, minicol->from_get ().no_get (), minicol->to_get ().no_get ());
		if (edge) {
			update_mincol_appearance (*edge, *minicol);
		}
	}
}

void ControlRobot::highlight_current_mincol ()
{
	const Columns& cols = neuro_.cols_get();
	Minicol*  minicol = cols.best_minicol (0);
	
	if (minicol == 0 || !minicol->recruited_get ()){
		return;
	}
	Edge* edge = widget_.edge_get (COL, minicol->from_get ().no_get (), minicol->to_get ().no_get ());
	if (edge) {
		edge->color_set (COLOR_SPIKING);
		edge->lightlevel_set (1.0);
//		edge->lightlevel_set (mincol->state_link_activation ());
	}
}

void ControlRobot::update_cols ()
{
	int size = neuro_.cols_get ().size ();
	for (int i = 0; i < size; i++) {
		Column* col = neuro_.cols_get ().col_get (i);
		if (col->pos_get () == 0) {
			continue;
		}
		Node* node = widget_.node_get (COL, col->no_get ());
		node->move (*col->pos_get ());
		update_col_appearance (*node, *col);
	}
	update_mincols_appearance ();
	highlight_current_mincol ();
}

void ControlRobot::slot_addcol (int no)
{
  Coord   coord (0.0, 0.0);

  widget_.add_node (COL, no, coord, 20, Qt::darkYellow);
}

void ControlRobot::slot_addlink (int from, int to)
{
  widget_.add_edge (COL, from, to);
}

void ControlRobot::slot_setgoalcol (int no)
{
	Column* col = neuro_.cols_get ().nocol_get (no);
	if (neuro_.is_goal_position (col)) {
		neuro_.set_goal_weight (col, 0.0);
	}
	else {
		neuro_.set_goal_weight (col, 1.0);
	}
}
