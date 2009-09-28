#ifndef NEUROSOLVER_HH_
# define NEUROSOLVER_HH_

#include "columns.hh"
#include "hippo.hh"
#include "egoactionpop.hh"
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

	Action* best_action () const;
	double inf_get (double angle) const;
	bool synch (bool learning, bool decision_point);
	void sleep (int sleep_step);
	void goal_learning ();
	void set_goal_weight (Column* col, double val);
	bool is_goal_position (Column* col) const;
  
	void draw (ostream& os) const;

private:
	Hippo hippo_;
	EgoActionPop ego_pop_;
	AlloActionPop allo_pop_;
	ComputeUnit motivation_;
	Columns columns_;
	int ripples_;
	int learn_rythm_;
};

#endif
