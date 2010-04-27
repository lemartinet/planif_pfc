#include "columns.hh"
#include "column.hh"
#include "action.hh"
#include "neuron.hh"
#include "params.hh"
#include "coord.hh"
#include "minicol.hh"
#include "math.hh"
#include "logger.hh"
#include "behavior.hh"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/if.hpp>

using namespace boost::lambda;

Columns::Columns (const vector<ComputeUnit*>& hippo_pop)
{
	static const unsigned int SIZE_POP = Params::get_int("SIZE_POP");
	unsigned int nb = SIZE_POP * 1.0;
	while (columns_.size() < nb) {
		Column* col = new Column (*this, columns_.size());
		columns_.push_back (col);
	}
	while (columns_.size() < SIZE_POP) {
		Column* col = new Column (*this, columns_.size());
		columns_.push_back (col);
	}
	while (minicols_.size() < SIZE_POP) {
		Minicol* mc = new Minicol (*this, minicols_.size());
		minicols_.push_back(mc);
	}
	// ajoute une connexion aléatoire avec hippo et entre colonnes
//	winner_takes_all (hippo_pop);
	
	win_col_lvl_ = 0;
	prec_col_lvl_ = 0;
	win_minicol_lvl_ = 0;
}

Columns::~Columns ()
{
	for_each (columns_.begin (), columns_.end (), bind (delete_ptr(), _1));
	for_each (minicols_.begin (), minicols_.end (), bind (delete_ptr(), _1));
	columns_.clear ();
	minicols_.clear ();
	for_each (neurons_.begin (), neurons_.end (), bind (delete_ptr(), _1));
	neurons_.clear();
}

void Columns::winner_col ()
{
	Column* best_col = 0;
	vector<Column*>::iterator it;
	for (it = columns_.begin (); it != columns_.end (); ++it) {
//		if (best_col == 0 || (best_col->state_activation () < (*it)->state_activation ())){
//		if (best_col == 0 ||
//				((*it)->lastT_recent () > 0.3 && best_col->lastT_recent () < (*it)->lastT_recent ())){
		if (best_col == 0 || best_col->lastT_recent () < (*it)->lastT_recent ()){
//		else if (best_col == 0 || (best_col->lastT_recent () < (*it)->lastT_recent ()
//					&& ((*it)->state_activation () > 0.2 || (*it)->state_activation () < 0.1))){
			best_col = *it;
		}
	}
	if (best_col->lastT_recent () < 0.3) {
		best_col = 0;
		double min_sumw = 0;
		// on initialise avec celle qui a les plus petits poids = qui n'a pas de selectivité
		for (it = columns_.begin (); it != columns_.end (); ++it) {
			double sumw = (*it)->state_get().sum_wE_get();
			if (best_col == 0 || sumw < min_sumw) {
				min_sumw = sumw;
				best_col = *it;
			}
		}		
	}
//	cout << best_col->no_get() << " " << best_col->state_activation() << endl;
	win_col_lvl_ = best_col;
}

void Columns::winner_minicol ()
{
	Minicol* best_minicol = 0;
	vector<Minicol*>::iterator it;
	for (it = minicols_.begin (); it != minicols_.end (); it++) {
		if (!(*it)->recruited_get ()) {
			continue;
		}
		if (!best_minicol || (best_minicol->lastT_recent () < (*it)->lastT_recent ())) {
			best_minicol = *it;	
		}
	}
	win_minicol_lvl_ = best_minicol;
}

Minicol* Columns::add_minicol (const Action& action, Column& src, Column& dest)
{
	static const int SIZE_POP = Params::get_int("SIZE_POP");
	if (nb_used_minicol_ >= SIZE_POP) {
		cout << "[Mypuck] Too much minicolumns" << endl;
		return 0;	
	}
	Minicol* mc = minicols_[nb_used_minicol_];
	mc->new_set (action, src, dest);
	nb_used_minicol_++;
	return mc;	
}

double Columns::nb_spiking_cols () const
{
	double sum = 0;
	vector<Column*>::const_iterator it;
	for (it = columns_.begin (); it != columns_.end (); it++) {
		if ((*it)->lastT_recent () > 0.3) {
			sum += 1;	
		}
	}
	return sum;
}

double Columns::cells_activity () const
{
	double sum = 0;
	vector<Column*>::const_iterator it;
	for (it = columns_.begin (); it != columns_.end (); it++) {
		if ((*it)->lastT_recent () > 0.1) {
			sum += (*it)->lastT_recent ();	
		}
	}
	return sum;
}

double Columns::nb_spiking_cells (const vector<ComputeUnit*>& hippo_pop) const
{
//	return count_if (hippo_pop.begin (), hippo_pop.end (), bind (&ComputeUnit::spiking, _1)); 
	double sum = 0;
	vector<ComputeUnit*>::const_iterator it;
	for (it = hippo_pop.begin (); it != hippo_pop.end (); it++) {
		if ((*it)->spiking()) {
//			sum += (*it)->output();	
			sum += 1;
		}
	}
	return sum;
}

bool Columns::synch (bool learn, const vector<ComputeUnit*>& hippo_pop, const Coord* pos)
{
	for_each (neurons_.begin (), neurons_.end (), bind (&Neuron::compute, _1));
	for_each (neurons_.begin (), neurons_.end (), bind (&Neuron::update, _1));
//	double maxval=0;
//	vector<Column*>::iterator it;
//	for (it = columns_.begin (); it != columns_.end (); it++) {
//		double r = (*it)->state_get ().output ();
//		maxval = max(maxval, r);
//	}
//	cout << maxval;

	const int GUI = Params::get_int ("GUI");
	vector<Column*>::iterator it;
	for (it = columns_.begin (); it != columns_.end (); ++it) {
		(*it)->synch ();
		if (GUI && pos != 0) {
			(*it)->center_rf (*pos, *it == win_col_lvl_);
		}
	}
  	for_each (minicols_.begin (), minicols_.end (), bind (&Minicol::synch, _1));
  	
  	winner_col ();
  	winner_minicol ();
// 	cout << "cells activity: " << cells_activity() << endl;
//	columns_.show_activities();
  	if (learn) {
  		winner_takes_all(hippo_pop);
  		return topology_learning ();	
  	}
  	else {
  		return false;	
  	}
}

bool Columns::topology_learning ()
{
  	bool col_changed = false;
  	Column* current = win_col_lvl_;
  	Column* prec = prec_col_lvl_;
//	if (current && prec && prec != current) {
//		cout << prec->no_get () << "/" << prec->lastT_recent () << " -> "
//			<< current->no_get () << "/" << current->lastT_recent ()
//			<< " total " << nb_spiking_cols() << endl;
//	}
	if (current && prec && prec != current
//		&& prec->lastT_recent () * current->lastT_recent () > 0.1
//		&& prec->lastT_recent () * current->lastT_recent () < 0.3
		&& nb_spiking_cols () < 10) {
//		cout << "update minicol" << endl;
    	col_changed = true;
      	Action action (Behavior::behavior_get().angle_get());
      	lateral_learning (*prec, *current, action, true);
  	}
	prec_col_lvl_ = current;
  	return col_changed;	
}

void Columns::correct_transition ()
{
	if (win_col_lvl_ == 0) {
		return;	
	}
	Action a (Behavior::behavior_get().angle_get ());
	vector<Minicol*> all_minicols = minicol_get (win_col_lvl_->no_get (), a);
	for (vector<Minicol*>::iterator it = all_minicols.begin (); it != all_minicols.end (); ++it) {
		Minicol* real_minicol = *it;
  		// on s'est cogne a un mur alors qu'on pouvait faire la transition
  		stringstream message;
  		message << "0 " << real_minicol->from_get().no_get()+1
  				<< " " << real_minicol->to_get().no_get()+1 << " " << a.angle_get();
  		Logger::log ("network", message.str (), true);
  		lateral_learning (const_cast<Column&>(real_minicol->from_get()),
  							const_cast<Column&>(real_minicol->to_get()), 
  							real_minicol->action_get(), false);
	}
}

Column* Columns::nocol_get (int no) const
{
	vector<Column*>::const_iterator it;
	it = find_if (columns_.begin (), columns_.end (), bind (&Column::no_get, _1) == no);
	return (it == columns_.end ()) ? 0 : *it;
}

void Columns::lateral_learning (Column& from, Column& to, const Action& action, bool increase)
{
	stringstream message;
	Minicol* minicol = minicol_get (from.no_get (), to.no_get ());
  	if (minicol) {
  		// la minicolonne existe déjà : on modifie les poids
  		minicol->lateral_learning(increase);
		// on moyenne l'orientation
		message << "1 " << from.no_get ()+1 << " " << to.no_get ()+1 << " "
				<< minicol->action_get ().angle_get ();
		minicol->adapt_action (action);
//	      	message << " / " << minicol->action_get ().angle_get ();
  	}
  	else {
  		// on recrute une nouvelle minicolonne
      	minicol = add_minicol (action, from, to);
      	minicol->lateral_learning(true);
      	message << "2 " << from.no_get ()+1 << " " << to.no_get ()+1
   				<< " " << action.angle_get ();
    }
	if (message.str () != "") {
		Logger::log ("network", message.str (), true);
	}
	
//	// on ajoute la competition entre les minicols d'une meme col
//	// cela permettrait de diminuer les poids parasites
//	vector<Minicol*>::const_iterator it;
//	for (it = minicols_.begin (); it != minicols_.end (); it++) {
//		if ((*it)->recruited_get () && (*it)->from_get () == from && (*it) != minicol) {
//			(*it)->lateral_learning (false, 0.05);
//		}
//	}
}

void Columns::show_activities () const
{
	bool written = false;
	int size = columns_.size ();
	for (int i = 0; i < size; i++) {
		cout << i << "/" << columns_.at (i)->state_activation () << " ";
		written = true;
	}
	if (written) {
		cout << endl;
	}
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

vector<Minicol*> Columns::minicol_get (int from, const Action& action) const
{
	vector<Minicol*> res;
	vector<Minicol*>::const_iterator it;
	for (it = minicols_.begin (); it != minicols_.end (); it++) {
		if ((*it)->recruited_get ()
			&& (*it)->from_get ().no_get () == from && (*it)->action_get () == action) {
			res.push_back(*it);
		}
	}
	return res;
//	Minicol* big_one = 0;
//	double max_w = 0;
//	for (it = res.begin (); it != res.end (); it++) {
//		double w = (*it)->sup_get ().max_syn_get ()->w_get (); 
//		if (w > max_w) {
//			max_w = w;
//			big_one = *it;	
//		}
//	}
//	return big_one;	
}

vector<Minicol*> Columns::minicol_get (int from) const
{
	vector<Minicol*> res;
	vector<Minicol*>::const_iterator it;
	for (it = minicols_.begin (); it != minicols_.end (); it++) {
		if ((*it)->recruited_get () && (*it)->from_get ().no_get () == from) {
			res.push_back(*it);
		}
	}
	return res;
}

Neuron& Columns::add_neuron (nType type)
{
  Neuron* res = new Neuron (type, false);
  neurons_.push_back (res);
  return *res;
}

Neuron& Columns::add_neuron_max (nType type)
{
  Neuron* res = new Neuron (type);
  neurons_.push_back (res);
  return *res;
}

void Columns::winner_takes_all (const vector<ComputeUnit*>& pop_state)
{
	if (Behavior::behavior_get().nb_trial_get() < 1)
		return; 
	static const double RATE_PC_COL = Params::get_double ("RATE_PC_COL");	
//	static const double INIT_PC_COL = Params::get_double ("INIT_PC_COL");
	int nbunits = pop_state.size ();
	vector<Column*>::iterator it;
	for (it = columns_.begin (); it != columns_.end (); it++) {
//		if (win_col_lvl_ == *it || newcol) {
		Neuron& state = (*it)->state_get ();
		for (int i = 0; i < nbunits; i++) {
			ComputeUnit* unit = pop_state.at (i);
			double* w = state.syn_get(*unit);
//			if (w == 0) {
//				double init_val = INIT_PC_COL * drand ();
			if (w == 0 && *it == win_col_lvl_) {
				double init_val = unit->spiking () ? unit->output () : 0;
//				double init_val = 1 / (1 + exp(-10*(unit->output() - 0.3)));
				state.add_synapse (*unit, init_val);
			}
			else if (w != 0 && *it == win_col_lvl_) {
//				if (*it == win_col_lvl_) {
//					cout << "delta_w " << (*it)->weight_change (unit, s->w_get (), win_col_lvl_);
//				}
				double new_w = (*it)->weight_change (unit, *w, win_col_lvl_);
				static const string LEARN_RULE = Params::get ("LEARN_RULE");
				if (LEARN_RULE == "simple") {
					*w = *w + RATE_PC_COL * new_w;
				}
				else if (LEARN_RULE == "simple_seuil") {
					*w = *w + RATE_PC_COL * new_w;
					if (*w > 1) {
						*w = 1;
					}
					else if (*w < 0) {
						*w = 0;
					}
				}
				else if (LEARN_RULE == "mult") {
					// !!! j'ai *2 pour compenser l'appr plus lent du au *w
					*w = *w + *w * 2 * RATE_PC_COL * new_w;
				}
			}
		}
	}	
}
