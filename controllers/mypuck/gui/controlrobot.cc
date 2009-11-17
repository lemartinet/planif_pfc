#include "controlrobot.hh"
#include "cell.hh"
#include "hippo.hh"
#include "neurosolver.hh"
#include "columns.hh"
#include "column.hh"
#include "minicol.hh"
#include "params.hh"
#include "edge.h"
#include "behavior.hh"

ControlRobot::ControlRobot (const Hippo& hippo, Neurosolver& neuro) :
	hippo_(hippo), neuro_(neuro), refresh_(0)
{
	robot_ = widget_.add_node (ROBOT, 1);
	show_inf_sup_or_state_ = 0;
	show_pc_ = false;
	show_col_ = true;
	show_col2_ = false;
}

void ControlRobot::update ()
{
	update_cols ();
	update_mincols ();
	robot_->move (Behavior::behavior_get().position_get ());
	update_cells ();
	// maj de la position tous les 66 step * 15ms = 1s
	refresh_ = ++refresh_ % 66;
}

void ControlRobot::update_cells ()
{
	if (!show_pc_) {
		return;
	}
	const vector<ComputeUnit*>& hp_pop = hippo_.pop_get ();
	vector<ComputeUnit*>::const_iterator it;
	for (it = hp_pop.begin(); it != hp_pop.end(); it++) {
		const Cell& cell = dynamic_cast<const Cell&>(**it);
		if (cell.pos_get () == 0) {
			continue;
		}	
		Node* node = widget_.node_get (CELL, cell.no_get ());
		if (node == 0 && cell.spiking ()) {
			node = widget_.add_node (CELL, cell.no_get ());
			node->show ();
		}
		if (node != 0) {
			if (refresh_ == 0) {
				node->move (*(cell.pos_get ()));
			}
			node->color_set (cell.output ());
			node->update();
		}
	}
}

void ControlRobot::update_cols ()
{
	const vector<Column*>& cols = neuro_.cols_get().pop_col_get();
	vector<Column*>::const_iterator it;
	for (it = cols.begin(); it != cols.end(); it++) {
		Column* col = *it;
		Node* node = widget_.node_get (COL, col->no_get ());
		if (node == 0 && col->state_activation() > 0.2) {
			node = widget_.add_node (COL, col->no_get ());
			QObject::connect (node, SIGNAL (sig_node_clicked (int)), this, SLOT (slot_setgoalcol (int)));
			node->move(*col->pos_get());
		}
	}
	const vector<ComputeUnit*>& hp_pop = hippo_.pop_get ();
	for (it = cols.begin(); it != cols.end(); it++) {
		Column* col = *it;
		if (col->level_get() == 0 && !show_col_) {
			Node* node = widget_.node_get (COL, col->no_get ());	
			if (node != 0) {
				node->hide();
			}
			continue;
		}
		else if (col->level_get() == 1 && !show_col2_) {
			Node* node = widget_.node_get (COL, col->no_get ());	
			if (node != 0) {
				node->hide();
			}
			continue;
		}
		Node* node = widget_.node_get (COL, col->no_get ());	
		if (node != 0) {
			if (refresh_ == 0) {
				double* max_w = col->state_get ().max_syn_get (); 
				if (max_w != 0 && *max_w > 0.5) {
					if (col->level_get () == 0) {
					Coord moy (0,0);
					double norm = 0;
					vector<ComputeUnit*>::const_iterator itpc;
					for (itpc = hp_pop.begin(); itpc != hp_pop.end(); itpc++) {
						const Cell& cell = dynamic_cast<const Cell&>(**itpc);	
						double* w = col->state_get ().syn_get (cell.no_get());
						if (*w < 0.5 || cell.pos_get () == 0) {
							continue;
						}
						Coord tmp = *cell.pos_get ();
						tmp *= *w;
						moy += tmp;
						norm += *w;
					}
					moy /= norm;
					node->move (moy);
					}
					else {
						node->move(*col->pos_get());
					}
					node->show ();
				}
				else {
					node->hide ();
				}
			}
			update_col_appearance (*node, *col);
		}
	}
}

void ControlRobot::update_mincols ()
{
	const vector<Minicol*>& minicols = neuro_.cols_get().pop_minicol_get();
	vector<Minicol*>::const_iterator it;
	for (it = minicols.begin(); it != minicols.end(); it++) {
		Minicol* minicol = *it;
		if (!minicol->recruited_get ()) {
			continue;
		}
		Edge* edge = widget_.edge_get (COL, minicol->from_get ().no_get (), minicol->to_get ().no_get ());
		if (edge == 0) {
			edge = widget_.add_edge (COL, minicol->from_get ().no_get (), minicol->to_get ().no_get ());
		}
		if (edge != 0) {
			edge->best_set (false);
			Node* node1 = widget_.node_get (COL, minicol->from_get ().no_get ());
			Node* node2 = widget_.node_get (COL, minicol->to_get ().no_get ());
			if (node1 != 0 && node2 != 0 && node1->isVisible () && node2->isVisible ()) {
				edge->color_set (minicol->state_activation ());
//				edge->update();
				edge->show ();
			}
			else {
				edge->hide ();
			}
		}
	}
	highlight_current_mincol ();
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
	node.update();
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
	neuro_.gui_goal_set(no);
}

void ControlRobot::show_pc () 
{ 
	show_pc_ = !show_pc_;
	if (!show_pc_) {
		widget_.type_del (CELL);
	}
	else {
		const vector<ComputeUnit*>& hp_pop = hippo_.pop_get ();
		vector<ComputeUnit*>::const_iterator it;
		for (it = hp_pop.begin(); it != hp_pop.end(); it++) {
			const Cell& cell = dynamic_cast<const Cell&>(**it);
			if (cell.pos_get () == 0) {
				continue;
			}	
			Node* node = widget_.add_node (CELL, cell.no_get ());
			node->show ();
		}		
	}
}

void ControlRobot::show_col () 
{ 
	show_col_ = !show_col_;
//	if (!show_col_) {
//		widget_.type_del (COL);
//		widget_.edge_type_del (COL);
//	}
//	else {
//		int size = neuro_.cols_get ().size ();
//		for (int i = 0; i < size; i++) {
//			Column* col = neuro_.cols_get ().col_get (i);
//			if (col->pos_get () == 0) {
//				continue;
//			}
//			if (col->maxr_get () > 0.2) {
//				Node* node = widget_.add_node (COL, col->no_get (), 20);
//				node->show ();
//			}
//		}
//		
//		const Columns& cols = neuro_.cols_get();
//		size = cols.minicol_size ();
//		for (int i = 0; i < size; i++) {
//			Minicol* minicol = cols.minicol_get (i);
//			if (!minicol->recruited_get ()) {
//				continue;
//			}
//			Edge* edge = widget_.add_edge (COL, minicol->from_get ().no_get (), minicol->to_get ().no_get ());
//			edge->show ();
//		}
//	}
}

void ControlRobot::show_col2 () 
{ 
	show_col2_ = !show_col2_;
}
