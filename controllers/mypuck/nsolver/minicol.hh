#ifndef MINICOL_HH_
# define MINICOL_HH_

//#include "column.hh"
#include "neuralnet.hh"
#include "action.hh"
#include "mode.hh"

class Colomn;

class Minicol
{
public:
  Minicol (Neuralnet& net, Action* action, Colomn& src, Colomn& dest, string& path, int no_col, int no, int level);

  ~Minicol ();

  inline
  Neuron& sup_get () { return *sup_; }

  inline
  Neuron& inf_get () { return *inf_; }

  inline
  Action& action_get () { return *action_; }

  void    mode_set (enum e_mode mode);

  inline
  Colomn& from_get () { return *src_; }

  inline
  Colomn& to_get () { return *dest_; }

  inline
  double activation () { return inf_->output (); }

  inline
  double state_activation () { return inf_->output (); }

  inline
  double goal_activation () { return sup_->output (); }

  double state_link_activation ();

  double goal_link_activation ();

	inline
	bool spiking () { return inf_->spiking (); }

  void draw (ostream& os);

  void draw_links (ostream& os);
  
  void update_value ();
  
  inline
  double mean_value_get () { return mean_val_; }
  
  void adapt_action (Action * action);

private:
  string      path_;
  int         no_;
  Neuron*     sup_;
  Neuron*     inf_;
  Action*    action_;
  Neuralnet*  net_;
  Colomn*     src_;
  Colomn*     dest_;
  enum e_mode mode_;
  
  double mean_val_; // utiliser pour moyenner l'activite basse
  int level_; // niveau de la minicolonne dans la carte
};

#endif
