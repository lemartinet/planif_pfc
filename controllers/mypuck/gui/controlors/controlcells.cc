#include "controlcells.hh"
#include "cell.hh"
#include "graphwidget.h"
#include "hippo.hh"

ControlCells::ControlCells (const Hippo& hippo, GraphWidget& widget) : widget_(widget), hippo_(hippo)
{
	QObject::connect (&hippo_, SIGNAL (sig_addcell (int)), this, SLOT (slot_addcell (int)));
}

void ControlCells::update ()
{
  int    size = hippo_.size ();
  Node*  node = 0;

  if (SHOW_CELLS)
    {
      for (int i = 0; i < size; i++)
	{
	  const Cell& cell = hippo_.cell_get (i);
	  node = widget_.node_get (CELL, cell.no_get ());
	  node->move (cell.pos_get ());
	  node->lightlevel_set (cell.output ());
	}
    }
}

void ControlCells::slot_addcell (int no)
{
  Coord coord (0, 0);

  if (SHOW_CELLS)
    widget_.add_node (CELL, no, coord, 15, Qt::darkYellow);
}
