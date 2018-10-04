#ifndef MINICOL_HH_
# define MINICOL_HH_

#include "action.hh"
#include "neuron.hh"

class Column;
class Neuralnet;

class Minicol
{
public:
	Minicol (Neuralnet& net, Action* action, const Column& src, const Column& dest, const string& path, 
				int no_col, int no, int level);
	~Minicol ();
	
	Neuron& sup_get () const { return sup_; }
	Neuron& inf_get () const { return inf_; }
	Action& action_get () const { return *action_; }
	const Column& from_get () const { return src_; }
	const Column& to_get () const { return dest_; }
	double mean_value_get () const { return mean_val_; }
	
	double activation () const { return inf_.output (); }
	double state_activation () const { return inf_.output (); }
	double goal_activation () const { return sup_.output (); }
	bool spiking () const { return inf_.spiking (); }
	
	void draw_links (ostream& os) const;
	void draw (ostream& os) const;
	
	void update_value ();
	void adapt_action (Action* action);

private:
	const int no_;
	Neuron& sup_;
	Neuron& inf_;
	Action* action_;
	Neuralnet& net_;
	const Column& src_;
	const Column& dest_;
	double mean_val_; // utiliser pour moyenner l'activite basse
	const int level_; // niveau de la minicolonne dans la carte
};

#endif
