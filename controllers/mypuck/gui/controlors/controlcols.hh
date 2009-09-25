#ifndef CONTROLCOLS_H
# define CONTROLCOLS_H

#include <QObject>
#include "columns.hh"
#include "graphwidget.h"

#define COLOR_STATE          Qt::darkYellow
#define COLOR_GOAL           Qt::darkRed
#define COLOR_SPIKING        Qt::darkBlue

class ControlCols : private QObject
{
  Q_OBJECT

public:
  ControlCols (Colomns& cols, GraphWidget& widget);
  ~ControlCols () {};

  void update ();
  
  inline
  void show_sup () { show_inf_sup_or_state_ = 2; }
  
  inline
  void show_inf () { show_inf_sup_or_state_ = 1; }
  
  inline
  void show_state () { show_inf_sup_or_state_ = 0; } 
  
private:
  void update_col_appearance (Node& node, Colomn& col);
  
private:
  Colomns*      cols_;
  GraphWidget*  widget_;
  int show_inf_sup_or_state_;

private slots:
  void slot_addcol (int no);
  void slot_addlink (int from, int to);
  void slot_delcol (int no);
  void slot_setgoalcol (NodeType type, int no);
};

#endif
