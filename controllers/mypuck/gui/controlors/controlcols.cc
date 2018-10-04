#include "controlcols.hh"
#include "neurosolver.hh"
#include "columns.hh"
#include "column.hh"
#include "minicol.hh"
#include "params.hh"
#include <iostream>

ControlCols::ControlCols (Neurosolver& neuro, GraphWidget& widget)
{
	neuro_ = &neuro;
	widget_ = &widget;
	show_inf_sup_or_state_ = 0;
	
	const Columns* cols = &(neuro_->cols_get ());
	QObject::connect (cols, SIGNAL (sig_addcol (int)), this, SLOT (slot_addcol (int)));
	QObject::connect (cols, SIGNAL (sig_addlink (int, int)), this, SLOT (slot_addlink (int, int)));
	QObject::connect (widget_, SIGNAL (sig_node_clicked (NodeType, int)), this, SLOT (slot_setgoalcol (NodeType, int)));
}

void ControlCols::update_col_appearance (Node& node, Column& col)
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

void update_mincol_appearance (Edge& edge, Minicol& mincol)
{
  if (mincol.spiking ())
    edge.arrow_activ (true);
  else
    edge.arrow_activ (false);
      
  edge.color_set (COLOR_STATE);
//  edge.lightlevel_set (state_link_activation);
}

void update_mincols_appearance (GraphWidget& graph, Column& col)
{
  int        size = col.size ();
  Edge*      edge = 0;
  Minicol*   minicol = 0;

  for (int i = 0; i < size; i++)
    {
      minicol = col.minicol_get (i);
      edge = graph.edge_get (COL, col.no_get (), minicol->to_get ().no_get ());
      update_mincol_appearance (*edge, *minicol);
    }
}

void highlight_current_mincol (const Columns& cols, GraphWidget& graph)
{
	Minicol*  minicol = cols.best_minicol (0);
	
	if (!minicol){// || !minicol->spiking ()) {
		return;
	}
	Edge* edge = graph.edge_get (COL, minicol->from_get ().no_get (), minicol->to_get ().no_get ());
	edge->color_set (COLOR_SPIKING);
	edge->lightlevel_set (// mincol->state_link_activation () 
							1.0);
}

void ControlCols::update ()
{
  int       size = neuro_->cols_get ().size ();
  Column*   col = 0;
  Node*     node = 0;

  for (int i = 0; i < size; i++)
    {
      col = neuro_->cols_get ().col_get (i);
		if (!col->draw_get ()) {
			continue;
		}
      node = widget_->node_get (COL, col->no_get ());
      node->move (col->pos_get ());
      update_col_appearance (*node, *col);
      update_mincols_appearance (*widget_, *col);
    }

  highlight_current_mincol (neuro_->cols_get (), *widget_);
}

void ControlCols::slot_addcol (int no)
{
  Coord   coord (0.0, 0.0);

  widget_->add_node (COL, no, coord, 20, Qt::darkYellow);
}

void ControlCols::slot_addlink (int from, int to)
{
  widget_->add_edge (COL, from, to);
}

void ControlCols::slot_setgoalcol (NodeType type, int no)
{
	if (type != COL) {
		return;
	}

	Column* col = neuro_->cols_get ().nocol_get (no);
	if (neuro_->is_goal_position (col)) {
		neuro_->set_goal_weight (col, 0.0);
	}
	else {
		neuro_->set_goal_weight (col, 1.0);
	}
}
