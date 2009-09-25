#include <iostream>
#include "controlcols.hh"
#include "graphwidget.h"
#include "params.hh"

extern Params* params;

ControlCols::ControlCols (Colomns& cols, GraphWidget& widget)
{
  cols_ = &cols;
  widget_ = &widget;
  show_inf_sup_or_state_ = 0;

  QObject::connect (cols_, SIGNAL (sig_addcol (int)), this, SLOT (slot_addcol (int)));
  QObject::connect (cols_, SIGNAL (sig_addlink (int, int)), this, SLOT (slot_addlink (int, int)));
  QObject::connect (cols_, SIGNAL (sig_delcol (int)), this, SLOT (slot_delcol (int)));
  QObject::connect (widget_, SIGNAL (sig_node_clicked (NodeType, int)), this, SLOT (slot_setgoalcol (NodeType, int)));
}

void ControlCols::update_col_appearance (Node& node, Colomn& col)
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
  //double   state_link_activation = mincol.state_link_activation ();
//  double   goal_link_activation = mincol.goal_link_activation ();

  if (mincol.spiking ())
    edge.arrow_activ (true);
  else
    edge.arrow_activ (false);
      
  edge.color_set (COLOR_STATE);
//  edge.lightlevel_set (state_link_activation);
}

void update_mincols_appearance (GraphWidget& graph, Colomn& col)
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

void highlight_current_mincol (Colomns& cols, GraphWidget& graph)
{
  Edge*     edge = 0;
  Colomn*   col = 0;  
  Minicol*  mincol = 0;

  col = cols.best_state_col (0);
  if (!col)
    return;
  mincol = col->best_mean_minicol ();
  if (!mincol)
    return;
  edge = graph.edge_get (COL, col->no_get (), mincol->to_get ().no_get ());

  edge->color_set (COLOR_SPIKING);
  edge->lightlevel_set (// mincol->state_link_activation ()
                        1.0);
}

void ControlCols::update ()
{
  int       size = cols_->size ();
  Colomn*   col = 0;
  Node*     node = 0;

  for (int i = 0; i < size; i++)
    {
      col = cols_->col_get (i);
		if (!col->draw_get ()) {
			continue;
		}
      node = widget_->node_get (COL, col->no_get ());
      assert (node);
      node->move (col->pos_get ());
      update_col_appearance (*node, *col);
      update_mincols_appearance (*widget_, *col);
    }

  highlight_current_mincol (*cols_, *widget_);
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

void ControlCols::slot_delcol (int no)
{
  widget_->del_node (COL, no);
}

void ControlCols::slot_setgoalcol (NodeType type, int no)
{
  if (type != COL)
    return;

  Colomn*   col = cols_->nocol_get (no);
  if (col->goal_activity_setted ()) {
    col->goal_activity_unset ();
  }
  else {
  	static const double GOAL_ACTIVITY = params->get_double("GOAL_ACTIVITY");
    col->goal_activity_set (GOAL_ACTIVITY);
  }
}

