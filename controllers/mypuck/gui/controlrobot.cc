#include "controlrobot.hh"
#include "device.hh"
#include "cell.hh"
#include "hippo.hh"
#include "neurosolver.hh"
#include "columns.hh"
#include "column.hh"
#include "minicol.hh"
#include "params.hh"
#include "edge.h"

ControlRobot::ControlRobot (const RobotDevice& robot_device, const Hippo& hippo, Neurosolver& neuro) :
	robot_device_(robot_device), hippo_(hippo), neuro_(neuro)
{
	robot_ = widget_.add_node (ROBOT, 1, 20);
	show_inf_sup_or_state_ = 0;
	QObject::connect (&widget_, SIGNAL (sig_node_clicked (int)), this, SLOT (slot_setgoalcol (int)));
	show_pc_ = false;
	show_col_ = true;
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
	if (!show_pc_) {
		return;
	}
	int size = hippo_.size ();
	for (int i = 0; i < size; i++) {
		const Cell& cell = hippo_.cell_get (i);
		if (cell.pos_get () == 0) {
			continue;
		}	
		Node* node = widget_.node_get (CELL, cell.no_get ());
		if (node == 0 && cell.spiking ()) {
			node = widget_.add_node (CELL, cell.no_get (), 15);
			node->show ();
		}
		if (node != 0) {
			node->move (*(cell.pos_get ()));
			node->color_set (cell.output ());
		}
	}
}

void ControlRobot::update_cols ()
{
	if (!show_col_) {
		return;
	}
	int size = neuro_.cols_get ().size ();
	for (int i = 0; i < size; i++) {
		Column* col = neuro_.cols_get ().col_get (i);
		if (col->pos_get () == 0) {
			continue;
		}
		Node* node = widget_.node_get (COL, col->no_get ());
		if (node == 0 && col->state_activation() > 0.2) {
			node = widget_.add_node (COL, col->no_get (), 20);
		}
		if (node != 0) {
			node->move (*col->pos_get ());
			update_col_appearance (*node, *col);
		}
	}
}

void ControlRobot::update_mincols ()
{
//	if (!show_col_) {
//		return;
//	}
//	const Columns& cols = neuro_.cols_get();
//	int size = cols.minicol_size ();
//	for (int i = 0; i < size; i++) {
//		Minicol* minicol = cols.minicol_get (i);
//		if (!minicol->recruited_get ()) {
//			continue;
//		}
//		Edge* edge = widget_.edge_get (COL, minicol->from_get ().no_get (), minicol->to_get ().no_get ());
//		if (edge == 0) {
//			edge = widget_.add_edge (COL, minicol->from_get ().no_get (), minicol->to_get ().no_get ());
//		}
//		if (edge) {
//			update_mincol_appearance (*edge, *minicol);
//		}
//	}
//	highlight_current_mincol ();
}

void ControlRobot::update_col_appearance (Node& node, Column& col)
{
	if (show_inf_sup_or_state_ == 2) {
		node.color_set (col.sup_activation ());
    }
    else if (show_inf_sup_or_state_ == 1) {
    	node.color_set (col.inf_activation ());
    }
    else if (show_inf_sup_or_state_ == 0) {
    	node.color_set (col.state_activation ());
    }
}

void ControlRobot::update_mincol_appearance (Edge& edge, Minicol& mincol)
{
	edge.color_set (mincol.state_activation ());
	edge.best_set (false);
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
		edge->best_set (true);
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

void ControlRobot::show_pc () 
{ 
	show_pc_ = !show_pc_;
	if (!show_pc_) {
		widget_.type_del (CELL);
	}
	else {
		int size = hippo_.size ();
		for (int i = 0; i < size; i++) {
			const Cell& cell = hippo_.cell_get (i);
			if (cell.pos_get () == 0) {
				continue;
			}	
			Node* node = widget_.add_node (CELL, cell.no_get (), 15);
			node->show ();
		}		
	}
}

void ControlRobot::show_col () 
{ 
	show_col_ = !show_col_;
	if (!show_col_) {
		widget_.type_del (COL);
		widget_.edge_type_del (COL);
	}
	else {
		int size = neuro_.cols_get ().size ();
		for (int i = 0; i < size; i++) {
			Column* col = neuro_.cols_get ().col_get (i);
			if (col->pos_get () == 0) {
				continue;
			}
			if (col->maxr_get () > 0.2) {
				Node* node = widget_.add_node (COL, col->no_get (), 20);
				node->show ();
			}
		}
		
		const Columns& cols = neuro_.cols_get();
		size = cols.minicol_size ();
		for (int i = 0; i < size; i++) {
			Minicol* minicol = cols.minicol_get (i);
			if (!minicol->recruited_get ()) {
				continue;
			}
			Edge* edge = widget_.add_edge (COL, minicol->from_get ().no_get (), minicol->to_get ().no_get ());
			edge->show ();
		}
	}
}
