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
	Columns (const vector<ComputeUnit*>& hippo_pop);
	~Columns ();
	
	Minicol* minicol_get (int from, int to) const;
	vector<Minicol*> minicol_get (int from, const Action& action) const;
	vector<Minicol*> minicol_get (int from) const;
	Column* nocol_get (int no) const;
	const vector<Column*>& pop_col_get () const { return columns_; }
	const vector<Minicol*>& pop_minicol_get () const { return minicols_; }
	Column* best_state_col () const { return win_col_lvl_; }
	Minicol* best_minicol () const { return win_minicol_lvl_; }
	Neuron& add_neuron (nType type);
	Neuron& add_neuron_max (nType type);
	bool synch (bool learn, const vector<ComputeUnit*>& hippo_pop, const Coord* pos = 0);
	void correct_transition ();

private:	
	Minicol* add_minicol (const Action& action, Column& src, Column& dest);
	void show_activities () const;
	double nb_spiking_cols () const;
	double nb_spiking_cells (const vector<ComputeUnit*>& hippo_pop) const;
	double cells_activity () const;
	void winner_takes_all (const vector<ComputeUnit*>& pop_state);
	void lateral_learning (Column& from, Column& to, const Action& action, bool increase);
	bool topology_learning ();
	void winner_col ();
	void winner_minicol ();
	
private:
	int nb_used_minicol_;  
	vector<Column*> columns_;
	vector<Minicol*> minicols_;
	vector<Neuron*> neurons_;
	Column* win_col_lvl_;
	Column* prec_col_lvl_;
	Minicol* win_minicol_lvl_;
};

#endif
