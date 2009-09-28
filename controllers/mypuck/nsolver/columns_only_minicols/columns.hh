#ifndef COLUMNS_HH_
# define COLUMNS_HH_

#include <QObject>
#include <assert.h>
#include "column.hh"

class Hippo;

class Columns : public QObject
{
  Q_OBJECT

public:
  Columns (Hippo& state) : cpt_(0), state_(&state), newcol_(0), newmincol_(0), learn_(true) {};
  Columns () : cpt_(0), state_(0), newcol_(0), newmincol_(0), learn_(true) {};

  ~Columns ();

  inline
  Column* col_get (int i) { return columns_[i]; }
  
  inline
  int size () { return columns_.size (); }

  inline
  Minicol* newmincol_get () { return newmincol_; }

  inline
  Column* newcol_get () { return newcol_; }

  inline
  void net_draw (ostream& os) { net_.draw_graph (os); }

  void learn_set (bool learn);

  inline
  bool learn_get () { return learn_; }

  bool lateral_learning (Column& from, Column& to, Action* action, bool increase, string & message);

  Column* best_state_col ();

  Column& add_column ();

  void del_column (Column* col);

  void synch ();

  void propagate ();

  void draw (ostream& os);

  void draw_links (ostream& os);

  Column* nocol_get (int no);

  void reset ();

  void inhib_all ();

private:
  int               cpt_;
  vector<Column *>  columns_;
  Neuralnet         net_;
  Hippo*            state_;
  //Hippo*            goals_;
  Column*           newcol_;
  Minicol*          newmincol_;
  //bool              reset_;
  bool              learn_;

signals:
  void sig_addcol (int no);
  void sig_addlink (int from, int to);
  void sig_delcol (int no);
};

#endif
