#ifndef MINICOL_HH_
# define MINICOL_HH_

#include <string>
#include <sstream>
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
	
	void new_set (const Action& action, Column& src, Column& dest, int level);
	Neuron& sup_get () const { return sup_; }
	Neuron& inf_get () const { return inf_; }
	Action& action_get () const { return *action_; }
	const Column& from_get () const { return *src_; }
	const Column& to_get () const { return *dest_; }
	double lastT_recent () const;
	int level_get () const { return level_; }
	bool recruited_get () const { return recruited_; }
	
	double activation () const { return inf_.output (); }
	double state_activation () const { return inf_.output (); }
	double goal_activation () const { return sup_.output (); }
	bool spiking () const { return inf_.spiking (); }
	
	void draw (ostream& os) const;
	
	void update_value ();
	void adapt_action (const Action& action);
	void lateral_learning (bool increase);

private:
	const int no_;
	Neuron& sup_;
	Neuron& inf_;
	Action* action_;
	Column* src_;
	Column* dest_;
	int level_; // niveau de la minicolonne dans la carte
	bool recruited_;
	
	vector<double> lastT_;
	int lastTidx_;
};

#endif
