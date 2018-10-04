#ifndef COLUMN_HH_
# define COLUMN_HH_

#include "minicol.hh"
#include "action.hh"
#include "coord.hh"
#include "hippo.hh"
#include "log.hh"

class ComputeUnit;
class Columns;
class Neuralnet;

class Column
{
public:
	Column (Neuralnet& net, const Columns& columns, const vector<ComputeUnit*>& pop, int no, 
		int level, bool draw, ComputeUnit* ego_action, const Coord& pos);
	~Column ();
	
	double inf_activation () const { return inf_.output (); }
	double state_activation () const { return state_.output (); }
	double sup_activation () const { return sup_.output (); }
	bool spiking () const { return state_.spiking (); }
	bool minicol_spiking () const;
	Neuron& inf_get () const { return inf_; }
	Neuron& sup_get () const { return sup_; }
	Neuron& state_get () const { return state_; }
	Minicol* minicol_get (int i) const { return minicols_[i]; }
	Minicol* minicol_get (const Action& action) const;
	Minicol* minicol_get_nodest (int no) const;
	int size () const { return minicols_.size (); }
	int no_get () const { return no_; }
	Minicol* last_mincol_get () const { return lastmincol_; }
	const Coord& pos_get () const { return pos_; }
	int level_get () const { return level_; }
	bool draw_get () const { return draw_; }
	void winner_set (bool winner) { winner_ = winner; }

	bool lateral_learning (Action* action, const Column& dest, bool increase, string & message);
	void synch (bool learn, const Coord& pos);
	int nb_spiking_cells (int level) const;

	void draw (ostream& os) const;
	void log (const string& time_string, const Coord& position, double angle, int day, int trial);
	void log_state_weights (const string& time_string, int day, int trial);

	Minicol* best_minicol () const;
	Minicol* best_mean_minicol () const;

private:
	void connect_pop_to_neuron ();
	void center_rf (const Coord& pos);

private:
	const int no_;
	Neuron& state_;
	Neuron& sup_;
	Neuron& inf_;
	Neuralnet& net_;
	const Columns& columns_;
	const vector<ComputeUnit*>& pop_state_;
	const ComputeUnit* ego_action_;
	Minicol* lastmincol_;
	vector<Minicol*> minicols_;
	const int level_; // niveau de la colonne dans la carte
	const bool draw_; // dessiner ou pas la colonne
	bool winner_;
	Coord pos_;
	double maxr;
	Log log_;
	Log logw_;
};

bool operator== (const Column& c1, const Column& c2);

#endif
