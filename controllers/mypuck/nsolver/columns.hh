#ifndef COLUMNS_HH_
# define COLUMNS_HH_

#include <vector>
#include <ostream>
#include <sstream>
#include "computeunit.hh"

class Neuron;
class Column;
class Action;
class Minicol;
class Coord;

using namespace std;

class Columns
{
public:
	Columns (const vector<ComputeUnit*>& hippo_pop, const ComputeUnit& ego);
	~Columns ();
	
	Column* col_get (int i) const { return columns_[i]; }
	Minicol* minicol_get (int from, int to) const;
	vector<Minicol*> minicol_get (int from, const Action& action) const;
	Minicol* minicol_get (int i) const { return minicols_[i]; }
	int size () const { return columns_.size (); }
	int minicol_size () const { return minicols_.size (); }
	Column* nocol_get (int no) const;
	const vector<ComputeUnit*>& pop_get () const { return pop_; }
	vector<Column*> level_pop_get (int level) const;
	Neuron* neuron_get (int i) const { return neurons_[i]; }
	int neurons_size () const { return neurons_.size (); }
	
	Column* best_state_col (int level) const { return win_col_lvl_[level]; }
	Minicol* best_minicol (int level) const { return win_minicol_lvl_[level]; }
	Minicol* add_minicol (const Action& action, Column& src, Column& dest, int level);
	Neuron& add_neuron (nType type, double ip_step, double ip_mu, double a, double b);
	Neuron& add_neuron_max (nType type);
	bool synch (bool learn, const vector<ComputeUnit*>& hippo_pop, const ComputeUnit& ego, const Coord* pos = 0);
	void show_activities (int level) const;
	double nb_spiking_cells (int level) const;
	
	void net_draw (ostream& os) const;
	void draw (ostream& os) const;
	void log_no_pop (int level) const;
	
	void winner_takes_all_lvl0 (const vector<ComputeUnit*>& pop_state, bool newcol);
	void winner_takes_all_lvl1 (const vector<ComputeUnit*>& pop_state, const ComputeUnit& ego_action);
	void correct_transition ();
	void lateral_learning (Column& from, Column& to, const Action& action, bool increase);
	bool topology_learning ();
	
private:
	void winner_col (int level);
	void winner_minicol (int level);
	
private:
	int nb_used_minicol_;  
	vector<Column*> columns_;
	vector<Minicol*> minicols_;
	vector<ComputeUnit*> pop_;
	vector<Neuron*> neurons_;
	Column* win_col_lvl_[2];
	Column* prec_col_lvl_[2];
	Minicol* win_minicol_lvl_[2];
};

#endif
