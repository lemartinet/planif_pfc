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
	Columns ();
	~Columns ();
	
	Column* col_get (int i) const { return columns_[i]; }
	int size () const { return columns_.size (); }
	Column* newcol_get () const { return newcol_; }
	void learning_set (bool learn) { learning_ = learn; }
	bool learning_get () const { return learning_; }
	Column* nocol_get (int no) const;
	const vector<ComputeUnit*>& pop_get () const { return pop_; }
	vector<Column*> level_pop_get (int level) const;
	
	void lateral_learning (Column& from, const Column& to, Action* action, bool increase, string & message);
	Column* best_state_col (int level) const;
	Minicol* best_minicol (int level) const;
	Column& add_colomn (int level, const vector<ComputeUnit*>& pop, bool draw, ComputeUnit* egoaction, const Coord& pos);
	void synch (bool learn, const Coord& pos);
	void show_activities (int level) const;
	void winner_set (int level);
	double nb_spiking_cells (int level) const;
	
	void net_draw (ostream& os) const { net_.draw_graph (os); }
	void draw (ostream& os) const;
	void log_no_pop (int level) const;

private:
	int cpt_;
	vector<Column*> columns_;
	vector<ComputeUnit*> pop_;
	Neuralnet net_;
	Column* newcol_;
	bool learning_;

signals:
	void sig_addcol (int no);
	void sig_addlink (int from, int to);
};

#endif
