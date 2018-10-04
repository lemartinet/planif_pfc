#include "columns.hh"
#include "column.hh"
#include "action.hh"
#include "neuron.hh"
#include "params.hh"
#include "coord.hh"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/if.hpp>

using namespace boost::lambda;

Columns::Columns () : cpt_(0), newcol_(0), learning_(false) {};

Columns::~Columns ()
{
	log_no_pop (0);
	log_no_pop (1);
	for_each (columns_.begin (), columns_.end (), bind (delete_ptr(), _1));
}

Column* Columns::best_state_col (int level) const
{
	Column* best_col = 0;
	vector<Column*>::const_iterator it;
	for (it = columns_.begin (); it != columns_.end (); it++) {
		if ((*it)->level_get () != level) {
			continue;
		}
		if (!best_col || (best_col->state_activation () < (*it)->state_activation ())) {
			best_col = *it;	
		}
	}
	return best_col;
}

Minicol* Columns::best_minicol (int level) const
{
	Minicol* best_minicol = 0;
	vector<Column*>::const_iterator it;
	for (it = columns_.begin (); it != columns_.end (); it++) {
		if ((*it)->level_get () != level) {
			continue;
		}
		Minicol* tmp = (*it)->best_minicol ();
		if (!best_minicol || (tmp && best_minicol->activation () < tmp->activation ())) {
			best_minicol = tmp;	
		}
	}
	return best_minicol;
}

Column& Columns::add_colomn (int level, const vector<ComputeUnit*>& pop, bool draw, ComputeUnit* ego_action, const Coord& pos)
{
	Column* col = new Column (net_, *this, pop, cpt_, level, draw, ego_action, pos);
	cpt_++;
	columns_.push_back (col);
	pop_.push_back (&col->state_get());
	newcol_ = col;
	
	if (draw) {
		emit sig_addcol (col->no_get ());
	}
	
	return *col;
}

double Columns::nb_spiking_cells (int level) const
{
//	return count_if (pop_.begin (), pop_.end (), 
//		bind (&ComputeUnit::level_get, _1) == level && bind (&ComputeUnit::spiking, _1));
	double sum = 0;
	vector<ComputeUnit*>::const_iterator it;
	for (it = pop_.begin (); it != pop_.end (); it++) {
		if ((*it)->level_get () == level) {
			sum += (*it)->output ();	
		}
	}
	return sum;
}

void Columns::synch (bool learn, const Coord& pos)
{
	if (learning_) {
    	net_.synch_learn ();
  	}
  	else {
    	net_.synch ();
  	}
  	for_each (columns_.begin (), columns_.end (), bind (&Column::synch, _1, learn, pos));
  	newcol_ = 0;
  	
  	winner_set (0);
  	winner_set (1);
//  	cout << "nb_spiking_cells: " << nb_spiking_cells (0) << endl;
}

void Columns::draw (ostream& os) const
{
	for_each (columns_.begin (), columns_.end (), bind (&Column::draw, _1, var (os)));
}

Column* Columns::nocol_get (int no) const
{
	vector<Column*>::const_iterator it;
	it = find_if (columns_.begin (), columns_.end (), bind (&Column::no_get, _1) == no);
	return (it == columns_.end ()) ? 0 : *it;
}

void Columns::lateral_learning (Column& from, const Column& to, Action* action, bool increase, string & message)
{
	// fonction intermediaire uniquement utile pour l'affichage (sig_addlink)
	bool new_minicol = from.lateral_learning (action, to, increase, message);
	if (new_minicol) {
		emit sig_addlink (from.no_get (), to.no_get ());
		//cout << "from " << from.no_get () << " to " << to.no_get () << " " << action->angle_get() << endl;
	}
}

void Columns::show_activities (int level) const
{
	bool written = false;
	int size = columns_.size ();
	for (int i = 0; i < size; i++) {
		if (columns_.at (i)->level_get () == level) {
			cout << i << "/" << columns_.at (i)->state_activation () << " ";
			written = true;	
		}
	}
	if (written) {
		cout << endl;
	}
}

void Columns::winner_set (int level)
{
	int winner = -1;
	Column* col = best_state_col (level);
	if (col && col->state_get ().spiking ()) {
		winner = col->no_get ();
	}
	vector<Column*>::iterator it;
	for (it = columns_.begin (); it != columns_.end (); it++) {
		if ((*it)->no_get () == winner) {
			(*it)->winner_set (true);
		}
		else if ((*it)->level_get () == level) {
			(*it)->winner_set (false);	
		}
	}
//	cout << "winner level " << level << " is " << winner << endl;
}

vector<Column*> Columns::level_pop_get (int level) const
{
	vector<Column*> level_pop;
	vector<Column*>::const_iterator it;
	for (it = columns_.begin (); it != columns_.end (); it++) {
		if ((*it)->level_get () == level) {
			level_pop.push_back (*it);	
		}
	}
	return level_pop;
}

void Columns::log_no_pop (int level) const
{
	ostringstream message;
	vector<Column*> lvl = level_pop_get (level);
	vector<Column*>::const_iterator it;
	for (it = lvl.begin (); it != lvl.end (); it++) {
		message << (*it)->no_get () << " ";
	}
	ostringstream filename;
	filename << Params::get_path () << "lvl" << level << ".txt";
	ofstream f (filename.str ().c_str ());
	f << message.str () << endl;
}
