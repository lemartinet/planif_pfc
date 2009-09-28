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
	Behavior ();
	~Behavior ();

	void synch ();
	Neurosolver& neurosolver_get () { return neurosolver_; }
	string automate_state ();
  	
private:
	void compute_next_action ();
	void do_action ();
	double select_action (const vector<double>& dirs);
	bool e_greedy (const vector<double>& dirs, double* pa, stringstream& s);
	bool q_greedy (const vector<double>& dirs, double* pa);
	bool softmax (const vector<double>& dirs, double* pa);
	double qval (double angle) const;

private:
	RobotDevice robot_;
	Neurosolver neurosolver_;
	ObstacleAvoidance avoid_;
	double current_;
	int wait_; // temporisation de l'automate	
	behabiorAutomate automate_; // état de l'automate
};

#endif
