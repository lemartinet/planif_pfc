#include "controlcells.hh"
#include "cell.hh"

ControlCells::ControlCells (Hippo& hippo, GraphWidget& widget)
{
  widget_ = &widget;
  hippo_ = &hippo;

  QObject::connect (hippo_, SIGNAL (sig_addcell (int)), this, SLOT (slot_addcell (int)));
  QObject::connect (hippo_, SIGNAL (sig_reset (void)), this, SLOT (slot_reset (void)));
}

// void ControlCells::update ()
// {
//   //ajouter derniere cellule ajoutee
//   Cell*  cell = hippo_->lastadded_get ();
//   Node*  node = 0;
//   int    size = hippo_->size ();

//   if (hippo_->reset_get ())
//     widget_->reset (CELL);

//   if (cell)
//     widget_->add_node (CELL, cell->no_get (), cell->pos_get (), 15, Qt::darkYellow);

//   for (int i = 0; i < size; i++)
//     {
//       cell = hippo_->cell_get (i);
//       node = widget_->node_get (CELL, cell->no_get ());
//       node->lightlevel_set (cell->output ());
//     }
// }

void ControlCells::update ()
{
  int    size = hippo_->size ();
  Cell*  cell = 0;
  Node*  node = 0;

  if (SHOW_CELLS)
    {
      for (int i = 0; i < size; i++)
	{
	  cell = hippo_->cell_get (i);
	  node = widget_->node_get (CELL, cell->no_get ());
	  node->move (cell->pos_get ());
	  node->lightlevel_set (cell->output ());
	}
    }
}

void ControlCells::slot_addcell (int no)
{
  Coord coord (0, 0);

  if (SHOW_CELLS)
    widget_->add_node (CELL, no, coord, 15, Qt::darkYellow);
}

void ControlCells::slot_reset ()
{
  widget_->reset (CELL);
}
