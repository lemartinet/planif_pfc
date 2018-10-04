#ifndef COLUMNS_HH_
# define COLUMNS_HH_

#include <QObject>
#include "neuralnet.hh"

class ComputeUnit;
class Neuron;
class Column;
class Action;
class Minicol;
class Coord;

class Columns : public QObject
{
	Q_OBJECT

public:
	Columns (const vector<ComputeUnit*>& hippo_pop);
	~Columns ();
	
	Column* col_get (int i) const { return columns_[i]; }
	Minicol* minicol_get (int from, int to) const;
	Minicol* minicol_get (int from, const Action& action) const;
	Minicol* minicol_get (int i) const { return minicols_[i]; }
	int size () const { return columns_.size (); }
	int minicol_size () const { return minicols_.size (); }
	Column* newcol_get () const { return newcol_; }
	void learning_set (bool learn) { learning_ = learn; }
	bool learning_get () const { return learning_; }
	Column* nocol_get (int no) const;
	const vector<ComputeUnit*>& pop_get () const { return pop_; }
	vector<Column*> level_pop_get (int level) const;
	
	void lateral_learning (Column& from, Column& to, Action* action, bool increase, stringstream& message);
	Column* best_state_col (int level) const { return win_col_lvl_[level]; }
	Minicol* best_minicol (int level) const { return win_minicol_lvl_[level]; }
	Column& add_colomn (int level, const vector<ComputeUnit*>& hippo_pop, const Coord& pos);
	Column& add_colomn (int level, const ComputeUnit& ego_action);
	Minicol& add_minicol (Action* action, Column& src, Column& dest, int level);
	void synch (bool learn, const Coord& pos, const vector<ComputeUnit*>& hippo_pop, const ComputeUnit& ego);
	void show_activities (int level) const;
	double nb_spiking_cells (int level) const;
	
	void net_draw (ostream& os) const { net_.draw_graph (os); }
	void draw (ostream& os) const;
	void log_no_pop (int level) const;
	
private:
	void winner_col (int level);
	void winner_minicol (int level);
	
private:
	int nb_used_col_;
	int nb_used_minicol_;  
	vector<Column*> columns_;
	vector<Minicol*> minicols_;
	vector<ComputeUnit*> pop_;
	Neuralnet net_;
	Column* newcol_;
	bool learning_;
	Column* win_col_lvl_[2];
	Minicol* win_minicol_lvl_[2];

signals:
	void sig_addcol (int no);
	void sig_addlink (int from, int to);
};

#endif
