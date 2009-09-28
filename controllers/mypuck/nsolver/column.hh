#ifndef COLUMN_HH_
# define COLUMN_HH_

#include "neuron.hh"
#include <vector>
#include <queue>

class ComputeUnit;
class Columns;
class Coord;
class Cell;

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
	int level_get () const { return level_; }
	double maxr_get () { return maxr; }
	double lastT_recent () const;
	double lastT_old () const;

	void synch ();
	void center_rf (const Coord& pos, bool winner);
	double weight_change (const Cell* cell, double old_w, Column* winner);

private:
	const int no_;
	Neuron& state_;
	Neuron& sup_;
	Neuron& inf_;
	Columns& columns_;
	int level_; // niveau de la colonne dans la carte
	Coord* pos_;
	double maxr;
	
	vector<double> lastTrecent_;
	vector<double> lastTold_;
	int lastTidx_;
};

bool operator== (const Column& c1, const Column& c2);

#endif
