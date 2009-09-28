#ifndef MINICOL_HH_
# define MINICOL_HH_

//#include "column.hh"
#include "neuralnet.hh"
#include "coord.hh"

const int MEAN_SIZE = 10;
//const int COMPET_SIZE = 10;

class Column;
class Hippo;
class Action;

class Minicol
{
public:
	Minicol (Hippo* hippo, Neuralnet& net, Action * action, Column& src, Column * dest, string& path, int no_col, int no);
	Minicol (Minicol* mini_src, Hippo* hippo, Neuralnet& net, Action * action, Column& src, Column * dest, string& path, int no_col, int no);
	~Minicol ();

  inline
  Neuron& sup_get () { return *sup_; }

  inline
  Neuron& inf_get () { return *inf_; }

  inline
  Action* action_get () { return action_; }

	void learn_set (bool learn);

  inline
  Column* from_get () { return src_; }

  inline
  Column* to_get () { return dest_; }
  
	inline
	Coord& pos_get () { return pos_; }

  inline
  double inf_activation () { return inf_->output (); }

  inline
  double sup_activation () { return sup_->output (); }

  void draw (ostream& os);

  void draw_links (ostream& os);
  
//  static void increment_counter () {if (counter >= COMPET_SIZE-1) {counter = 0;} else {counter++;}}
  static void increment_counter_value () {if (counter_value >= MEAN_SIZE-1) {counter_value = 0;} else {counter_value++;}}
//  void set_active (bool ok) { ok ? activ_[counter]=1 : activ_[counter]=0; }
//  int power_active () { int res = 0; for(int i = 0; i < COMPET_SIZE; i++){ res += activ_[i];} return res;}
  void update_value () { /*last_values_[counter_value] = inf_->output ();*/ val = 0.9 * val + 0.1 * inf_->output (); }
//  double mean_value () { double res = 0; for(int i = 0; i < MEAN_SIZE; i++){ res += last_values_[i];} return res/MEAN_SIZE;} 
  double mean_value () { return val; }
  
	void adapt_action (Action * action);
  
	double weights_mean ();
	bool goal_activity_setted ();
	void goal_activity_set (double val);
	void goal_activity_unset ();
	void inhib (bool val);
	void synch ();

private:
	void update_IP ();
	void connect_hippo_to_minicol ();
	void update_links_hippo_neuron ();
	void pos_update ();
	void copy_inf_connections (Minicol* src);
	void copy_sup_connections (Minicol* src);

private:
	Hippo* hippo_;
	string path_;
	int no_;
	Neuron*     sup_;
	Neuron*     inf_;
	Action*    action_;
	Neuralnet*  net_;
	Column*     src_;
	Column*     dest_;
	bool learn_;
    Coord pos_;
  
//  static int counter;
//  int activ_[COMPET_SIZE];
  static int counter_value;
  double last_values_[MEAN_SIZE];
  double val;
};

#endif
