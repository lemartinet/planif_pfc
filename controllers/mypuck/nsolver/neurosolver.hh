#ifndef NEUROSOLVER_HH_
# define NEUROSOLVER_HH_

#include "columns.hh"
#include "hippo.hh"
#include "egoactionpop.hh"
#include "alloactionpop.hh"
#include "computeunit.hh"

class Behavior;
class RobotDevice;

class Neurosolver
{
public:
  Neurosolver () : colomns_(), ego_pop_(), allo_pop_(), motivation_ (1, 0), learn_(false), 
  	explo_done_(false), last_orientation_(0) { prec_lvl0_ = 0; prec_lvl1_ = 0; }

  ~Neurosolver () {};

//   inline
//   void erase_goal (Coord& goal) { goals_.erase_signal (goal); }

//   inline
//   void erase_goals () { goals_.erase_signals (); }

//   inline
//   void add_goal (Coord& goal) { goals_.add_signal (goal); }

  inline
  Colomns& cols_get () { return colomns_; }

  inline
  Hippo& hippo_get () { return state_; }

  inline
  void learn_set (bool learn) { learn_ = learn; if (learn == true) { prec_lvl0_=0; prec_lvl1_ = 0; } }

//   inline
//   Hippo& goals_get () { return goals_; }

  void draw (ostream& os);

  Action* best_action ();
  
  void reset ();
  
  Action* planning ();
  
  string learning_or_planing (RobotDevice& robot, int nb_goal_reached);
  bool synch (RobotDevice& robot, bool go_random, int nb_goal_reached, string & message);
  string correct_transition (bool bloque);
  void learn_goal_position (RobotDevice& robot);
  

private:
	Colomns colomns_;
	Hippo state_;
	EgoActionPop ego_pop_;
	AlloActionPop allo_pop_;
	ComputeUnit motivation_;
	//Hippo goals_;
	Colomn* prec_lvl0_;
	Minicol* previous_predicted_minicol_lvl0_;
	Colomn* prec_lvl1_;
	Minicol* previous_predicted_minicol_lvl1_;
	bool learn_;
	bool explo_done_;
	double last_orientation_;
	bool correction_done_;
};

#endif
