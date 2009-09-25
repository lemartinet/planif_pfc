#ifndef BEHAVIOR_HH
# define BEHAVIOR_HH

#include "action.hh"
#include "mode.hh"
#include "neurosolver.hh"


class Behavior
{
public:
	Behavior () : cpt_(0), left_near_(false), right_near_(false),
  				action_done_(true), wait_(0), nb_goal_reached_(0),
  				nb_free_(0), dirs_(0), go_random_(false) {};
  	~Behavior () {if (current_) delete current_;}
  	
  	void compute_next_action (RobotDevice& robot);
  	
  	void do_action (RobotDevice& robot);

  	inline
  	Neurosolver& neurosolver_get () { return neurosolver_; }

  	void reset ();
  	
  	inline
  	int get_nb_goal_reached () {return nb_goal_reached_;}
  	
private:
	int analyse_cross_road (RobotDevice& robot, bool& left, bool& straight, bool& right);
	
	void free_ways (RobotDevice& robot);
	
	Action* random_or_planif (RobotDevice& robot);
  	
  	double e_greedy ();
  	
  	void write_message (ofstream* file, string message);

private:
  	int cpt_;
  	Neurosolver neurosolver_;
	bool left_near_;
	bool right_near_;
	Action* current_;
	bool action_done_;
//	bool bloque_;
	int wait_; // attente entre les décisions
	int nb_goal_reached_;
	int nb_free_;
	double* dirs_;
	bool go_random_;
};

#endif
