#ifndef BEHAVIOR_HH
# define BEHAVIOR_HH

#include "neurosolver.hh"
#include "device.hh"
#include <sstream>

class Action;

enum behabiorAutomate { GO_ON, INTERSECT, DECIDE, DECIDED, GOAL, BLOCK, SLEEP };

class Behavior
{
public:
	Behavior (RobotDevice& robot);
	~Behavior ();

	void synch ();
	Neurosolver& neurosolver_get () { return neurosolver_; }
	string automate_state ();
	
	int cpt_total_get () const { return cpt_total_; }
	int nb_trial_get () const { return nb_trial_; }
	int nb_path_get () const { return nb_path_; }
	double angle_get () const { return robot_.angle_get(); }
	const Coord& position_get () const { return robot_.position_get(); }
	bool goal_reached () const { return robot_.goal_reached(); }
	bool is_sleeping() const { return automate_ == SLEEP; }
	
	static Behavior& behavior_get() { return *behavior_; }
  	
private:
	double select_action (const vector<double>& dirs);
	bool no_exploration (const vector<double>& dirs, double* pa, stringstream& s);
	bool e_greedy (const vector<double>& dirs, double* pa, stringstream& s);
	bool q_greedy (const vector<double>& dirs, double* pa);
	bool softmax (const vector<double>& dirs, double* pa);
	double qval (double angle) const;
	const double* best_action() const;

private:
	static Behavior* behavior_; 
	RobotDevice& robot_;
	double next_action_;
	int wait_; // temporisation de l'automate	
	enum behabiorAutomate automate_; // état de l'automate
	int cpt_trial_;
	int cpt_total_;
	int nb_trial_;
	int nb_path_;
	vector<double> action_list_;
	vector<double> value_list_;
	Neurosolver neurosolver_;
};

#endif
