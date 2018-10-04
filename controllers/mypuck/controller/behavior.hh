#ifndef BEHAVIOR_HH
# define BEHAVIOR_HH

#include "neurosolver.hh"
#include "device.hh"
#include "obstacleavoidance.hh"
#include <sstream>

class Action;;

class Behavior
{
public:
	Behavior ();
	~Behavior ();

	void synch ();
	Neurosolver& neurosolver_get () { return neurosolver_; }
	RobotDevice& robot_get () { return robot_; }
  	
private:
	void compute_next_action ();
	void do_action ();
	Action* select_action ();
	void e_greedy (const vector<double>& dirs, double* pa, stringstream& s);
	void q_greedy (const vector<double>& dirs, double* pa);
	void softmax (const vector<double>& dirs, double* pa);
	double qval (double angle) const;

private:
	RobotDevice robot_;
	Neurosolver neurosolver_;
	ObstacleAvoidance avoid_;
	bool action_done_;
	int wait_; // attente entre les décisions
	int wait_at_goal_;
	double current_;
};

#endif
