#ifndef MINICOL_HH_
# define MINICOL_HH_

#include "action.hh"
#include "neuron.hh"
#include <string>

class Column;
class Neuralnet;

class Minicol
{
public:
	Minicol (Neuralnet& net, Action* action, Column& src, Column& dest,	int no, int level);
	Minicol (Neuralnet& net, int no);
	~Minicol ();
	
	void new_set (Action* action, Column& src, Column& dest, int level);
	Neuron& sup_get () const { return sup_; }
	Neuron& inf_get () const { return inf_; }
	Action& action_get () const { return *action_; }
	const Column& from_get () const { return *src_; }
	const Column& to_get () const { return *dest_; }
	double mean_value_get () const { return mean_val_; }
	int level_get () const { return level_; }
	bool recruited_get () const { return recruited_; }
	
	double activation () const { return inf_.output (); }
	double state_activation () const { return inf_.output (); }
	double goal_activation () const { return sup_.output (); }
	bool spiking () const { return inf_.spiking (); }
	
	void draw (ostream& os) const;
	
	void update_value ();
	void adapt_action (Action* action);

private:
	const int no_;
	Neuron& sup_;
	Neuron& inf_;
	Action* action_;
	Neuralnet& net_;
	Column* src_;
	Column* dest_;
	double mean_val_; // utiliser pour moyenner l'activite basse
	int level_; // niveau de la minicolonne dans la carte
	bool recruited_;
};

#endif
