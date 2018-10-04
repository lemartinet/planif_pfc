#ifndef CONTROLCELLS_HH
# define CONTROLCELLS_HH

#include <QObject>

class GraphWidget;
class Hippo;

// pour afficher les cellules de lieux dans QT
#define SHOW_CELLS    false

class ControlCells : private QObject
{
  Q_OBJECT

public:
  ControlCells (const Hippo& hippo, GraphWidget& widget);
  ~ControlCells () {};

  void update ();

private:
  GraphWidget& widget_;
  const Hippo& hippo_;

private slots:
  void slot_addcell (int no);
};

#endif
