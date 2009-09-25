#ifndef CONTROLCELLS_HH
# define CONTROLCELLS_HH

#include <QObject>
#include "graphwidget.h"
#include "hippo.hh"

// pour afficher les cellules de lieux dans QT
#define SHOW_CELLS    false

class ControlCells : private QObject
{
  Q_OBJECT

public:
  ControlCells (Hippo& hippo, GraphWidget& widget);
  ~ControlCells () {};

  void update ();

private:
  GraphWidget* widget_;
  Hippo*       hippo_;

private slots:
  void slot_addcell (int no);
  void slot_reset ();
};

#endif
