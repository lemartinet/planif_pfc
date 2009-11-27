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
	
	Minicol* minicol_get (int from, int to) const;
	vector<Minicol*> minicol_get (int from, const Action& action) const;
	vector<Minicol*> minicol_get (int from) const;
	Column* nocol_get (int no) const;
	const vector<Column*>& pop_col_get () const { return columns_; }
	const vector<Minicol*>& pop_minicol_get () const { return minicols_; }
	Column* best_state_col (int level) const { return win_col_lvl_[level]; }
	Minicol* best_minicol (int level) const { return win_minicol_lvl_[level]; }
	Neuron& add_neuron (nType type);
	Neuron& add_neuron_max (nType type);
	bool synch (bool learn, const vector<ComputeUnit*>& hippo_pop, const ComputeUnit& ego, const Coord* pos = 0);
	void correct_transition ();

private:	
	Minicol* add_minicol (const Action& action, Column& src, Column& dest, int level, Neuron* from0, Neuron* to0);
	vector<Column*> level_pop_get (int level) const;
	void show_activities (int level) const;
	double nb_spiking_cols (int level) const;
	double nb_spiking_cells (const vector<ComputeUnit*>& hippo_pop) const;
	double cells_activity (int level) const;
	void log_no_pop (int level) const;
	void winner_takes_all_lvl0 (const vector<ComputeUnit*>& pop_state);
	void winner_takes_all_lvl1 (const vector<Column*>& pop, const ComputeUnit& ego_action);
	void lateral_learning_lvl0 (Column& from, Column& to, const Action& action, bool increase);
	void lateral_learning_lvl1(Column& from_1, Column& from_0, Column& to_1, Column& to_0, const Action& action);
	bool topology_learning ();
	void winner_col (int level);
	void winner_col (int level, const ComputeUnit& ego);
	void winner_minicol (int level);
	
private:
	int nb_used_minicol_;  
	vector<Column*> columns_;
	vector<Minicol*> minicols_;
	vector<Column*> pop_;
	vector<Neuron*> neurons_;
	Column* win_col_lvl_[2];
	Column* prec_col_lvl_[2];
	Minicol* win_minicol_lvl_[2];
	bool new_lvl1_;
	Column* old_win_lvl1_;
};

#endif
