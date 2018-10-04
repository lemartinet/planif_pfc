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

#define COLOR_STATE          Qt::darkYellow
#define COLOR_GOAL           Qt::darkRed
#define COLOR_SPIKING        Qt::darkBlue

ControlRobot::ControlRobot (const RobotDevice& robot_device, const Hippo& hippo, Neurosolver& neuro, GraphWidget& widget) :
	robot_device_(robot_device), hippo_(hippo), neuro_(neuro), widget_(widget)  
{
	robot_ = widget.add_node (ROBOT, 1, 20, Qt::cyan);
	show_inf_sup_or_state_ = 0;
	QObject::connect (&widget_, SIGNAL (sig_node_clicked (int)), this, SLOT (slot_setgoalcol (int)));
//	show_pc_ = false;
}

void ControlRobot::update ()
{
	update_cols ();
	update_mincols ();
	robot_->move (robot_device_.position_get ());
	update_cells ();
}

void ControlRobot::update_cells ()
{
	static const int SHOW_CELLS = Params::get_int("SHOW_CELLS");
	if (SHOW_CELLS) {
		int size = hippo_.size ();
		for (int i = 0; i < size; i++) {
			const Cell& cell = hippo_.cell_get (i);
			if (cell.pos_get () == 0) {
				continue;
			}	
			Node* node = widget_.node_get (CELL, cell.no_get ());
			if (node == 0 && cell.spiking ()) {
				node = widget_.add_node (CELL, cell.no_get (), 15, Qt::darkYellow);
			}
//			if (node != 0 && show_pc_) {
			if (node != 0) {
				node->move (cell->pos_get ());
				node->lightlevel_set (cell.output ());
//				node->color_set (cell->output ());
//				node->show ();
			}
//			else if (node != 0 && !show_pc_) {
//				node->hide ();
//			}
		}
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
		if (node == 0 && col->state_activation() > 0.2) {
			node = widget_.add_node (COL, col->no_get (), 20, Qt::darkYellow);
		}
		if (node != 0) {
			node->move (*col->pos_get ());
			update_col_appearance (*node, *col);
		}
	}
}

void ControlRobot::update_mincols ()
{
	const Columns& cols = neuro_.cols_get();
	int size = cols.minicol_size ();
	for (int i = 0; i < size; i++) {
		Minicol* minicol = cols.minicol_get (i);
		if (!minicol->recruited_get ()) {
			continue;
		}
		Edge* edge = widget_.edge_get (COL, minicol->from_get ().no_get (), minicol->to_get ().no_get ());
		if (edge == 0) {
			edge = widget_.add_edge (COL, minicol->from_get ().no_get (), minicol->to_get ().no_get ());
		}
		if (edge) {
			update_mincol_appearance (*edge, *minicol);
		}
	}
	highlight_current_mincol ();
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
//	if (show_inf_sup_or_state_ == 2) {
//		node.color_set (col.mean_sup_activation ());
//    }
//    else if (show_inf_sup_or_state_ == 1) {
//    	node.color_set (col.mean_inf_activation ());
//    }
//    else if (show_inf_sup_or_state_ == 0) {
//    	node.color_set (col.mean_inf_activation ());
//    }
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

//	edge.color_set (mincol.mean_value ());
//	edge.best_set (false);
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

//		edge->best_set (true);
	}
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
