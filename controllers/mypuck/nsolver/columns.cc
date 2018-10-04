#include "columns.hh"
#include "column.hh"
#include "action.hh"
#include "neuron.hh"
#include "params.hh"
#include "coord.hh"
#include "minicol.hh"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/if.hpp>

using namespace boost::lambda;

Columns::Columns (const vector<ComputeUnit*>& hippo_pop) : nb_used_col_(0), newcol_(0), learning_(false) 
{
	static const unsigned int SIZE_POP = Params::get_int("SIZE_POP");
	while (columns_.size() < SIZE_POP) {
		// TODO: a adapter à lvl1
		Column* col = new Column (net_, *this, columns_.size(), hippo_pop);
		columns_.push_back (col);
		pop_.push_back (&col->state_get());
		Minicol* mc = new Minicol (net_, minicols_.size());
		minicols_.push_back(mc);
	}
	win_col_lvl_[0] = win_col_lvl_ [1] = 0;
	win_minicol_lvl_[0] = win_minicol_lvl_ [1] = 0;
}

Columns::~Columns ()
{
	log_no_pop (0);
	log_no_pop (1);
	for_each (columns_.begin (), columns_.end (), bind (delete_ptr(), _1));
	for_each (minicols_.begin (), minicols_.end (), bind (delete_ptr(), _1));
	columns_.clear ();
	minicols_.clear ();
}

void Columns::winner_col (int level)
{
	Column* best_col = 0;
	vector<Column*>::iterator it;
	for (it = columns_.begin (); it != columns_.end (); it++) {
		if ((*it)->level_get () != level) {
			continue;
		}
		else if (best_col == 0 || (best_col->state_activation () < (*it)->state_activation ())){
			best_col = *it;
		}
		(*it)->winner_set (false);
	}
	if (best_col != 0) {
		best_col->winner_set (true);
	}
	win_col_lvl_[level] = best_col;
//	cout << "winner level " << level << " is " << best_col->no_get () << endl;
}

void Columns::winner_minicol (int level)
{
	Minicol* best_minicol = 0;
	vector<Minicol*>::iterator it;
	for (it = minicols_.begin (); it != minicols_.end (); it++) {
		if ((*it)->level_get () != level || !(*it)->recruited_get ()) {
			continue;
		}
		if (!best_minicol || (best_minicol->activation () < (*it)->activation ())) {
			// on peut utiliser aussi mean_value_get () au lieu de activation ()
			best_minicol = *it;	
		}
	}
	win_minicol_lvl_[level] = best_minicol;
}

Column& Columns::add_colomn (int level, const vector<ComputeUnit*>& hippo_pop, const Coord& pos)
{
	static const int SIZE_POP = Params::get_int("SIZE_POP");
	if (nb_used_col_ >= SIZE_POP) {
		cout << "[Mypuck] Too much columns" << endl;	
	}
	newcol_ = columns_[nb_used_col_];
	newcol_->new_set (level, hippo_pop, pos);
	emit sig_addcol (newcol_->no_get ());
	nb_used_col_++;
	return *newcol_;
}

Column& Columns::add_colomn (int level, const ComputeUnit& ego_action)
{
	static const int SIZE_POP = Params::get_int("SIZE_POP");
	if (nb_used_col_ >= SIZE_POP) {
		cout << "[Mypuck] Too much columns" << endl;	
	}
	newcol_ = columns_[nb_used_col_];
	newcol_->new_set (level, pop_, ego_action);
	nb_used_col_++;
	return *newcol_;
}

Minicol& Columns::add_minicol (Action* action, Column& src, Column& dest, int level)
{
	static const int SIZE_POP = Params::get_int("SIZE_POP");
	if (nb_used_minicol_ >= SIZE_POP) {
		cout << "[Mypuck] Too much minicolumns" << endl;	
	}
	Minicol* mc = minicols_[nb_used_minicol_];
	mc->new_set (action, src, dest, level);
	nb_used_minicol_++;
	return *mc;	
}

double Columns::nb_spiking_cells (int level) const
{
	double sum = 0;
	vector<Column*>::const_iterator it;
	for (it = columns_.begin (); it != columns_.end (); it++) {
		if ((*it)->level_get () == level) {
			sum += (*it)->state_get ().output ();	
		}
	}
	return sum;
}

void Columns::synch (bool learn, const Coord& pos, const vector<ComputeUnit*>& hippo_pop, const ComputeUnit& ego)
{
	if (learning_) {
    	net_.synch_learn ();
  	}
  	else {
    	net_.synch ();
  	}
	vector<Column*>::iterator it;
	for (it = columns_.begin (); it != columns_.end (); it++) {
		if ((*it)->level_get () == 0) {
			(*it)->synch (learn, pos, hippo_pop);	
		}
		else if ((*it)->level_get () == 1) {
			(*it)->synch (learn, pop_, ego);	
		} 
	}
  	for_each (minicols_.begin (), minicols_.end (), bind (&Minicol::update_value, _1));
  	newcol_ = 0;
  	
  	winner_col (0);
  	winner_col (1);
  	winner_minicol (0);
  	winner_minicol (1);
//  	cout << "nb_spiking_cells: " << nb_spiking_cells (0) << endl;
}

void Columns::draw (ostream& os) const
{
	os << endl;
//	for_each (columns_.begin (), columns_.end (), bind (&Column::draw, _1, var (os)));
	// issu de Column::draw ()
//	stringstream ss;
//	string       val;
//	ss << no_;
//	ss >> val;
//	os << "c" << val << " [label=\"{<n" << state_.no_get () << "> H} | {<n" << sup_.no_get () << "> Prec | {";
//	(*minicols_.begin ())->draw (os);
//	for_each (minicols_.begin () + 1, minicols_.end (), 
//		(var (os) << " | ", bind (&Minicol::draw, _1, var (os))));
//	os << "} | <n" << inf_.no_get () << "> Suiv} \"];" << endl;
}

Column* Columns::nocol_get (int no) const
{
	vector<Column*>::const_iterator it;
	it = find_if (columns_.begin (), columns_.end (), bind (&Column::no_get, _1) == no);
	return (it == columns_.end ()) ? 0 : *it;
}

void Columns::lateral_learning (Column& from, Column& to, Action* action, bool increase, stringstream& message)
{
	bool new_minicol = false;	
	Minicol* minicol = minicol_get (from.no_get (), to.no_get ());
  	if (minicol) {
  		// la minicolonne existe déjà : on modifie les poids
  		net_.lateral_learning (minicol->inf_get (), to.inf_get (), increase);
      	net_.lateral_learning (to.sup_get (), minicol->sup_get (), increase);
      	
      	if (from.level_get () == 0) {
	      	// on moyenne l'orientation
	      	message << "update_minicol " << from.no_get () << "->" << to.no_get () << " : " 
	      			<< minicol->action_get ().angle_get ();
	      	minicol->adapt_action (action);
	      	message << " / " << minicol->action_get ().angle_get ();
      	}
  	}
  	else {
  		// on recrute une nouvelle minicolonne
      	add_minicol(action, from, to, from.level_get ());
      	message << "new_minicol " << from.no_get () << "->" << to.no_get ();
      	if (from.level_get () == 0) {
      		message << " : " << action->angle_get ();
      	}
      	new_minicol = true;
    }	
	
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

Minicol* Columns::minicol_get (int from, int to) const
{
	vector<Minicol*>::const_iterator it;
	for (it = minicols_.begin (); it != minicols_.end (); it++) {
		if ((*it)->recruited_get () && (*it)->from_get ().no_get () == from && (*it)->to_get ().no_get () == to) {
			return *it;
		}
	}
	return 0;
}

Minicol* Columns::minicol_get (int from, const Action& action) const
{
	vector<Minicol*>::const_iterator it;
	for (it = minicols_.begin (); it != minicols_.end (); it++) {
		if ((*it)->recruited_get () && (*it)->level_get () == 0
			&& (*it)->from_get ().no_get () == from && (*it)->action_get () == action) {
			return *it;
		}
	}
	return 0;	
}
