#ifndef NEUROSOLVER_HH_
# define NEUROSOLVER_HH_

#include "columns.hh"
#include "hippo.hh"
#include "alloactionpop.hh"
#include "computeunit.hh"

class Behavior;
class Column;

class Neurosolver
{
public:
	Neurosolver ();
	~Neurosolver ();
	
	const Columns& cols_get () const { return columns_; }
	const Hippo& hippo_get () const { return hippo_; }
	void get_actions(vector<double>& angles, vector<double>& values);
	void synch (bool learning, bool decision_point, bool goal_found, bool bloque, bool sleeping, int sleep_step);
	void gui_goal_set(int no_col);
	bool shortcut(double available, int& nb_action);
	
private:
	void set_goal_weight (Column* col, double val);
	bool is_goal_position (Column* col) const;
	void sleep (int sleep_step);
	void goal_learning ();
  
private:
	Hippo hippo_;
	AlloActionPop allo_pop_;
	ComputeUnit motivation_;
	Columns columns_;
	int ripples_;
	int learn_rythm_;
};

#endif
