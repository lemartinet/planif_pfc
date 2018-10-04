#ifndef BEHAVIOR_HH
# define BEHAVIOR_HH

#include "neurosolver.hh"
#include "device.hh"
#include "obstacleavoidance.hh"

class Action;

class Behavior
{
public:
	Behavior ();
	~Behavior ();

	void synch ();
	Neurosolver& neurosolver_get () { return neurosolver_; }
	RobotDevice& robot_get () { return robot_; }
	int get_nb_goal_reached () {return nb_goal_reached_;}
  	
private:
	void compute_next_action ();
	void do_action ();
	int analyse_cross_road (bool& left, bool& straight, bool& right);
	void free_ways ();
	Action* random_or_planif ();
	double e_greedy ();
	void write_message (ofstream* file, string message);

private:
	int cpt_;
	RobotDevice robot_;
	Neurosolver neurosolver_;
	ObstacleAvoidance avoid_;
	bool left_near_;
	bool right_near_;
	Action* current_; // TODO : mettre un double plutôt que la classe Action !
	bool action_done_;
//	bool bloque_;
	int wait_; // attente entre les décisions
	int nb_goal_reached_;
	int nb_free_;
	double* dirs_;
	bool go_random_;
};

#endif
