#ifndef COLUMN_HH_
# define COLUMN_HH_

#include "minicol.hh"
#include "action.hh"
#include "coord.hh"
#include "hippo.hh"

class Colomn
{
public:
  Colomn (Neuralnet& net, vector<ComputeUnit*>& pop, int no, int level, bool draw);

  ~Colomn ();

  inline
  double inf_activation () { return inf_->output (); }
  
  inline
  double state_activation () { return state_->output (); }

  inline
  double sup_activation () { return sup_->output (); }

  inline
  bool actived () { return state_->spiking (); }

  inline
  Neuron& inf_get () { return *inf_; }

  inline
  Neuron& sup_get () { return *sup_; }

  inline
  Neuron& state_get () { return *state_; }

  inline
  Neuron& goals_get () { return *goals_; }

  inline
  enum e_mode mode_get () { return mode_; }

  inline
  Minicol* minicol_get (int i) { return minicols_[i]; }

  inline
  int size () { return minicols_.size (); }

  inline
  int no_get () { return no_; }

  inline
  Minicol* last_mincol_get () { return lastmincol_; }

  inline
  string& path_get () { return path_; }

  inline
  Coord& pos_get () { return pos_; }

  inline
  bool goal_activity_setted () { return goals_->intra_setted (); }

  inline
  void goal_activity_set (double val) { goals_->intra_set (val); }

  inline
  void goal_activity_unset () { goals_->intra_unset (); }

  void mode_set (enum e_mode mode);
 
  Minicol* lateral_learning (Action* action, Colomn& dest, bool increase, string & message);

  void del_minicols (Colomn& dest);

  bool spiking ();
  
  Minicol* minicol_get (Action& action);

  void synch ();

  void draw (ostream& os);

  void draw_links (ostream& os);

  Minicol* minicol_get_nodest (int no);

  double weights_mean ();
  
  Minicol* best_minicol ();
  Minicol* best_mean_minicol ();
  
  inline
  int level_get () { return level_; }
  
  inline
  bool draw_get () { return draw_; }
  
  inline 
  void winner_set (bool winner) { winner_ = winner; }

private:
	void pos_update ();
	void lastmincol_update ();
	void connect_pop_to_neuron ();
	void update_links_pop_neuron ();

  int                no_;
  string             path_;
  enum e_mode        mode_;
  Neuron*            state_;
  Neuron*            goals_;
  Neuron*            sup_;
  Neuron*            inf_;
  Neuralnet*         net_;
  vector<ComputeUnit*>* pop_state_;
  //Hippo*             hippo_goals_;
  Minicol*           lastmincol_;
  Coord              pos_;
  vector<Minicol *>  minicols_;
  
    int level_; // niveau de la colonne dans la carte
    bool draw_; // dessiner ou pas la colonne
    bool winner_;
};

bool operator== (Colomn& c1, Colomn& c2);

#endif
