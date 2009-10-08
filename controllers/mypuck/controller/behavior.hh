#ifndef BEHAVIOR_HH
# define BEHAVIOR_HH

#include "neurosolver.hh"
#include "device.hh"
#include "obstacleavoidance.hh"
#include <sstream>

class Action;

enum behabiorAutomate { GO_ON, DECIDE, DECIDED, GOAL, BLOCK, SLEEP };

class Behavior
{
public:
	Behavior (RobotDevice& robot);
	~Behavior ();

	void synch ();
	Neurosolver& neurosolver_get () { return neurosolver_; }
	string automate_state ();
	
	int cpt_total_get () const { return cpt_total_; }
	bool manually_moved () const { return manually_moved_; }
	double angle_get () const { return robot_.angle_get(); }
	const Coord& position_get () const { return robot_.position_get(); }
	bool goal_reached () const { return robot_.goal_reached(); }
	
	static const Behavior& behavior_get() { return *behavior_; }
  	
private:
	void compute_next_action ();
	void do_action ();
	double select_action (const vector<double>& dirs);
	bool e_greedy (const vector<double>& dirs, double* pa, stringstream& s);
	bool q_greedy (const vector<double>& dirs, double* pa);
	bool softmax (const vector<double>& dirs, double* pa);
	double qval (double angle) const;

private:
	static Behavior* behavior_; 
	RobotDevice& robot_;
	Neurosolver neurosolver_;
	ObstacleAvoidance avoid_;
	double current_;
	int wait_; // temporisation de l'automate	
	behabiorAutomate automate_; // état de l'automate
	int cpt_trial_;
	int cpt_total_;
	int nb_trial_;
	bool manually_moved_;
};

#endif
