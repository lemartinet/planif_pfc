#ifndef COLUMN_HH_
# define COLUMN_HH_

#include "neuron.hh"

class ComputeUnit;
class Columns;
class Coord;

using namespace std; 

class Column
{
public:
	Column (Columns& columns, int no);
	~Column ();
	
	double inf_activation () const { return inf_.output (); }
	double state_activation () const { return state_.output (); }
	double sup_activation () const { return sup_.output (); }
	bool spiking () const { return state_.spiking (); }
	Neuron& inf_get () const { return inf_; }
	Neuron& sup_get () const { return sup_; }
	Neuron& state_get () const { return state_; }
	int no_get () const { return no_; }
	const Coord* pos_get () const { return pos_; }
	double maxr_get () { return maxr; }
	double lastT_recent () const { return state_.mean_recent(); }

	void synch ();
	void center_rf (const Coord& pos, bool winner);
	double weight_change (const ComputeUnit* cell, double old_w, Column* winner);

private:
	const int no_;
	Neuron& state_;
	Neuron& sup_;
	Neuron& inf_;
	Columns& columns_;
	Coord* pos_;
	double maxr;
};

bool operator== (const Column& c1, const Column& c2);

#endif
