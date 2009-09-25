#ifndef COLUMNS_HH_
# define COLUMNS_HH_

#include <QObject>
#include <assert.h>
#include "column.hh"

class ComputeUnit;
class Neuron;

class Colomns : public QObject
{
  Q_OBJECT

public:
  Colomns ();

  ~Colomns ();

  inline
  Colomn* col_get (int i) { return colomns_[i]; }
  
  inline
  int size () { return colomns_.size (); }

  inline
  enum e_mode mode_get () { return mode_; }

  inline
  Minicol* newmincol_get () { return newmincol_; }

  inline
  Colomn* newcol_get () { return newcol_; }

  inline
  void net_draw (ostream& os) { net_.draw_graph (os); }

  inline
  void learning_set (bool learn) { learning_ = learn; }

  inline
  bool learning_get () { return learning_; }

  bool lateral_learning (Colomn& from, Colomn& to, Action* action, bool increase, string & message);

  void mode_set (enum e_mode mode);

  Colomn* best_state_col (int level);

  Colomn& add_colomn (int level, vector<ComputeUnit*>& pop, bool draw);

  void del_colomn (Colomn* col);

  void synch ();

  void propagate ();

  void draw (ostream& os);

  void draw_links (ostream& os);

  Colomn* nocol_get (int no);

  void reset ();
  
  inline
  vector<ComputeUnit*>& pop_get () { return pop_; }
  
  void show_activities (int level);
  
  void winner_set (int col_winner);
  void winner_reset ();

private:
  int               cpt_;
  enum e_mode       mode_;
  vector<Colomn*>  colomns_;
  vector<ComputeUnit*> pop_;
  Neuralnet         net_;
  Colomn*           newcol_;
  Minicol*          newmincol_;
  bool              learning_;

signals:
  void sig_addcol (int no);
  void sig_addlink (int from, int to);
  void sig_delcol (int no);
};

#endif
