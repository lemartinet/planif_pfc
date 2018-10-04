#ifndef NEUROSOLVER_HH_
# define NEUROSOLVER_HH_

#include "columns.hh"
#include "hippo.hh"
#include "egoactionpop.hh"
#include "alloactionpop.hh"
#include "computeunit.hh"

#define TIMESTEP_WEIGHTS 50
#define TIMESTEP_OUTPUTS 10
#define TIMESTEP_CELLS 10

class Behavior;
class RobotDevice;
class Column;

class Neurosolver
{
public:
	Neurosolver (const RobotDevice& robot);
	~Neurosolver ();
	
	const Columns& cols_get () const { return columns_; }
	const Hippo& hippo_get () const { return hippo_; }
	void learn_set (bool learn);

	Action* best_action () const;
	double inf_get (double angle) const;
	bool synch ();
	void correct_transition (bool bloque);
	void goal_learning ();
	void set_goal_weight (Column* col, double val);
	bool is_goal_position (Column* col);
	void state_learning ();
	bool topology_learning ();  
  
	void draw (ostream& os) const;
	void log () const;

private:
	const RobotDevice& robot_;
	Columns columns_;
	Hippo hippo_;
	EgoActionPop ego_pop_;
	AlloActionPop allo_pop_;
	ComputeUnit motivation_;
	Column* prec_lvl0_;
	Column* prec_lvl1_;
	Column* current_lvl0_;
	Column* current_lvl1_;
	bool learn_;
	bool explo_done_;
	bool correction_done_;
	int no_learning_timer_;
};

#endif
