#ifndef NEUROSOLVER_HH_
# define NEUROSOLVER_HH_

#include "columns.hh"
#include "hippo.hh"
#include "egoactionpop.hh"
#include "alloactionpop.hh"
#include "computeunit.hh"

class Behavior;
class RobotDevice;
class Column;

class Neurosolver
{
public:
	Neurosolver (const RobotDevice& robot);
	~Neurosolver ();
	
	const Columns& cols_get () const { return colomns_; }
	const Hippo& hippo_get () const { return hippo_; }
	void learn_set (bool learn) { learn_ = learn; if (learn == true) { prec_lvl0_=0; prec_lvl1_ = 0; } }

	Action* best_action () const;
	string learning_or_planing (int nb_goal_reached);
	bool synch (int nb_goal_reached, string & message);
	string correct_transition (bool bloque);
	void update_goal_position ();
	void learn_goal_position (Column* col);
	void unlearn_goal_position (Column* col);
	bool is_goal_position (Column* col);
  
	void draw (ostream& os) const;

private:
	const RobotDevice& robot_;
	Columns colomns_;
	Hippo hippo_;
	EgoActionPop ego_pop_;
	AlloActionPop allo_pop_;
	ComputeUnit motivation_;
	Column* prec_lvl0_;
	Minicol* previous_predicted_minicol_lvl0_;
	Column* prec_lvl1_;
	Minicol* previous_predicted_minicol_lvl1_;
	bool learn_;
	bool explo_done_;
	bool correction_done_;
};

#endif
