#ifndef MINICOL_HH_
# define MINICOL_HH_

#include <vector>
#include "neuron.hh"

class Column;
class Columns;
class Action;

using namespace std;

class Minicol
{
public:
	Minicol (Columns& columns, int no);
	~Minicol ();
	
	void new_set (const Action& action, Column& src, Column& dest);
	Neuron& sup_get () const { return sup_; }
	Neuron& inf_get () const { return inf_; }
	Action& action_get () const { return *action_; }
	const Column& from_get () const { return *src_; }
	const Column& to_get () const { return *dest_; }
	double lastT_recent () const { return inf_.mean_recent(); }
	bool recruited_get () const { return recruited_; }
	
	double activation () const { return inf_.output (); }
	double state_activation () const { return inf_.output (); }
	double goal_activation () const { return sup_.output (); }
	bool spiking () const { return inf_.spiking (); }
	
	void synch ();
	void adapt_action (const Action& action);
	void lateral_learning (bool increase, double factor=1.0);

private:
	const int no_;
	Neuron& sup_;
	Neuron& inf_;
	Action* action_;
	Column* src_;
	Column* dest_;
	bool recruited_;
};

#endif
