#ifndef COLUMN_HH_
# define COLUMN_HH_

#include "neuron.hh"
#include <vector>

class ComputeUnit;
class Columns;
class Neuralnet;
class Coord;

using namespace std; 

class Column
{
public:
	Column (Neuralnet& net, Columns& columns, int no, const vector<ComputeUnit*>& hippo_pop);
	~Column ();
	
	void new_set (int level, const vector<ComputeUnit*>& hippo_pop, const Coord& pos);
	void new_set (int level, const vector<ComputeUnit*>& pop, const ComputeUnit& ego_action);
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
	void winner_set (bool winner) { winner_ = winner; }

	void synch (bool learn, const Coord& pos, const vector<ComputeUnit*>& pop_state);
	void synch (bool learn, const vector<ComputeUnit*>& pop_state, const ComputeUnit& ego_action);

private:
	void connect_pop_to_neuron (const vector<ComputeUnit*>& pop_state, bool newcol = false);
	void connect_pop_to_neuron (const vector<ComputeUnit*>& pop_state, const ComputeUnit& ego_action);
	void center_rf (const Coord& pos);

private:
	const int no_;
	Neuron& state_;
	Neuron& sup_;
	Neuron& inf_;
	Neuralnet& net_;
	Columns& columns_;
	int level_; // niveau de la colonne dans la carte
	bool winner_;
	Coord* pos_;
	double maxr;
};

bool operator== (const Column& c1, const Column& c2);

#endif
