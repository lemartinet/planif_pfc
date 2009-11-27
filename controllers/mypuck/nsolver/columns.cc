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

Columns::Columns (const vector<ComputeUnit*>& hippo_pop, const ComputeUnit& ego)
{
	static const unsigned int SIZE_POP = Params::get_int("SIZE_POP");
	unsigned int lvl0_nb = SIZE_POP * 0.9;
	while (columns_.size() < lvl0_nb) {
		Column* col = new Column (*this, columns_.size(), 0);
		columns_.push_back (col);
		pop_.push_back (col);
	}
	while (columns_.size() < SIZE_POP) {
		Column* col = new Column (*this, columns_.size(), 1);
		columns_.push_back (col);
	}
	while (minicols_.size() < SIZE_POP) {
		Minicol* mc = new Minicol (*this, minicols_.size());
		minicols_.push_back(mc);
	}
	// ajoute une connexion aléatoire avec hippo et entre colonnes
//	winner_takes_all_lvl0 (hippo_pop);
//	winner_takes_all_lvl1 (pop_, ego);
	
	win_col_lvl_[0] = win_col_lvl_ [1] = 0;
	prec_col_lvl_[0] = prec_col_lvl_ [1] = 0;
	win_minicol_lvl_[0] = win_minicol_lvl_ [1] = 0;
	new_lvl1_ = false;
	old_win_lvl1_ = 0;
}

Columns::~Columns ()
{
	log_no_pop (0);
	log_no_pop (1);
	for_each (columns_.begin (), columns_.end (), bind (delete_ptr(), _1));
	for_each (minicols_.begin (), minicols_.end (), bind (delete_ptr(), _1));
	columns_.clear ();
	minicols_.clear ();
	for_each (neurons_.begin (), neurons_.end (), bind (delete_ptr(), _1));
	neurons_.clear();
}

void Columns::winner_col (int level)
{
	Column* best_col = 0;
	vector<Column*>::iterator it;
	for (it = columns_.begin (); it != columns_.end (); ++it) {
		if ((*it)->level_get () != level) {
			continue;
		}
//		else if (best_col == 0 || (best_col->state_activation () < (*it)->state_activation ())){
//		else if (best_col == 0 || 
//				((*it)->lastT_recent () > 0.3 && best_col->lastT_recent () < (*it)->lastT_recent ())){
		else if (best_col == 0 || best_col->lastT_recent () < (*it)->lastT_recent ()){
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
			if ((*it)->level_get () != level) {
				continue;
			}
			double sumw = (*it)->state_get().sum_wE_get();
			if (best_col == 0 || sumw < min_sumw) {
				min_sumw = sumw;
				best_col = *it;
			}
		}		
	}
//	cout << best_col->no_get() << " " << best_col->state_activation() << endl;
	win_col_lvl_[level] = best_col;
}

void Columns::winner_col (int level, const ComputeUnit& ego)
{
	Column* best_col = win_col_lvl_[level];
//	if (ego.spiking() && win_col_lvl_[level] != 0) 
//		return; 
//	if (!ego.spiking()) 
//		return; 
	vector<Column*>::iterator it;
	for (it = columns_.begin (); it != columns_.end (); ++it) {
		if ((*it)->level_get () != level) {
			continue;
		}
//		else if (best_col == 0 || (best_col->state_activation () < (*it)->state_activation ())){
//		else if (best_col == 0 || 
//				((*it)->lastT_recent () > 0.3 && best_col->lastT_recent () < (*it)->lastT_recent ())){
		else if (best_col == 0 || (best_col->lastT_recent () < (*it)->lastT_recent ())){
//		else if (best_col == 0 || (best_col->lastT_recent () < (*it)->lastT_recent ()
//					&& ((*it)->state_activation () > 0.2 || (*it)->state_activation () < 0.1))){
			best_col = *it;
		}
	}
	if (best_col && best_col->lastT_recent () < 0.3 && ego.spiking()) {
		new_lvl1_ = true;
		best_col = 0;
		double min_sumw = 0;
		// on initialise avec celle qui a les plus petits poids = qui n'a pas de selectivité
		for (it = columns_.begin (); it != columns_.end (); ++it) {
			if ((*it)->level_get () != level) {
				continue;
			}
			double sumw = (*it)->state_get().sum_wE_get();
			if (best_col == 0 || sumw < min_sumw) {
				min_sumw = sumw;
				best_col = *it;
			}
		}		
	}
	else {
		new_lvl1_ = false;	
	}
//	cout << best_col->no_get() << " " << best_col->state_activation() << endl;
	vector<Minicol*> best_mini = minicol_get(best_col->no_get());
	vector<Minicol*>::iterator itm = best_mini.begin();
	for (; itm != best_mini.end(); ++itm)
		cout << (*itm)->from_get().no_get() << "->" << (*itm)->to_get().no_get() << ":" << (*itm)->activation() << " ";
	if (best_mini.size() > 0)
		cout << endl;
	win_col_lvl_[level] = best_col;	
}

void Columns::winner_minicol (int level)
{
	Minicol* best_minicol = 0;
	vector<Minicol*>::iterator it;
	for (it = minicols_.begin (); it != minicols_.end (); it++) {
		if ((*it)->level_get () != level || !(*it)->recruited_get ()) {
			continue;
		}
		if (!best_minicol || (best_minicol->lastT_recent () < (*it)->lastT_recent ())) {
			best_minicol = *it;	
		}
	}
	win_minicol_lvl_[level] = best_minicol;
}

Minicol* Columns::add_minicol (const Action& action, Column& src, Column& dest, int level, Neuron* from0, Neuron* to0)
{
	static const int SIZE_POP = Params::get_int("SIZE_POP");
	if (nb_used_minicol_ >= SIZE_POP) {
		cout << "[Mypuck] Too much minicolumns" << endl;
		return 0;	
	}
	Minicol* mc = minicols_[nb_used_minicol_];
	mc->new_set (action, src, dest, level, from0, to0);
	nb_used_minicol_++;
	return mc;	
}

double Columns::nb_spiking_cols (int level) const
{
	double sum = 0;
	vector<Column*>::const_iterator it;
	for (it = columns_.begin (); it != columns_.end (); it++) {
		if ((*it)->level_get () == level && (*it)->lastT_recent () > 0.3) {
			sum += 1;	
		}
	}
	return sum;
}

double Columns::cells_activity (int level) const
{
	double sum = 0;
	vector<Column*>::const_iterator it;
	for (it = columns_.begin (); it != columns_.end (); it++) {
		if ((*it)->level_get () == level && (*it)->lastT_recent () > 0.1) {
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

bool Columns::synch (bool learn, const vector<ComputeUnit*>& hippo_pop, const ComputeUnit& ego, const Coord* pos)
{
	for_each (neurons_.begin (), neurons_.end (), bind (&Neuron::compute, _1));
	for_each (neurons_.begin (), neurons_.end (), bind (&Neuron::update, _1));
//	double maxval=0;
//	vector<Column*>::iterator it;
//	for (it = columns_.begin (); it != columns_.end (); it++) {
//		if ((*it)->level_get () == 0) {
//			double r = (*it)->state_get ().output ();
//			maxval = max(maxval, r);	
//		}
//	}
//	cout << maxval;

	const int GUI = Params::get_int ("GUI");
	vector<Column*>::iterator it;
	for (it = columns_.begin (); it != columns_.end (); ++it) {
//		if ((*it)->level_get () == 0) {
			(*it)->synch ();
			if (GUI && pos != 0) {
				(*it)->center_rf (*pos, *it == win_col_lvl_[0]);	
			}
//		}
//		else if ((*it)->level_get () == 1) {
//			(*it)->synch ();	
//		} 
	}
  	for_each (minicols_.begin (), minicols_.end (), bind (&Minicol::synch, _1));
  	
  	winner_col (0);
  	winner_col (1, ego);
  	winner_minicol (0);
  	winner_minicol (1);
// 	cout << "cells activity: " << cells_activity(0) << endl;
//	columns_.show_activities (0);
//	columns_.show_activities (1);
//	if (Behavior::behavior_get().nb_trial_get() >= 6){
//	cout << "egoaction: " << ego.output() << endl;
//	cout << "best lvl1: " << win_col_lvl_[1]->no_get () << " " 
//		<< win_col_lvl_[1]->state_get().output() << endl;
//	}
  	if (learn) {
  		winner_takes_all_lvl0 (hippo_pop);
  		winner_takes_all_lvl1 (pop_, ego);
  		return topology_learning ();	
  	}
  	else {
  		return false;	
  	}
}

bool Columns::topology_learning ()
{
  	bool col_changed = false;
  	Column* current_lvl0 = win_col_lvl_[0];
  	Column* prec_lvl0 = prec_col_lvl_[0];	
//	if (current_lvl0 && prec_lvl0 && prec_lvl0 != current_lvl0) {	
//		cout << prec_lvl0->no_get () << "/" << prec_lvl0->lastT_recent () << " -> "  
//			<< current_lvl0->no_get () << "/" << current_lvl0->lastT_recent () 
//			<< " total " << nb_spiking_cols (0) << endl;
//	}
	if (current_lvl0 && prec_lvl0 && prec_lvl0 != current_lvl0 
//		&& prec_lvl0->lastT_recent () * current_lvl0->lastT_recent () > 0.1
//		&& prec_lvl0->lastT_recent () * current_lvl0->lastT_recent () < 0.3
		&& nb_spiking_cols (0) < 10) {
//		cout << "update minicol" << endl;
    	col_changed = true;
      	Action action (Behavior::behavior_get().angle_get());
      	lateral_learning_lvl0 (*prec_lvl0, *current_lvl0, action, true);
  	}

	Column* current_lvl1 = win_col_lvl_[1];
	Column* prec_lvl1 = prec_col_lvl_[1];
	static bool learn_lvl1_lat = false;
	if (learn_lvl1_lat && col_changed) {
		cout << "lvl1 " << prec_lvl1->no_get() << "->" << current_lvl1->no_get()
				<< " / lvl0" << prec_lvl0->no_get() << "->" << current_lvl0->no_get() << endl;
		Action action(Behavior::behavior_get().angle_get());
		lateral_learning_lvl1(*prec_lvl1, *prec_lvl0, *current_lvl1, *current_lvl0, action);
		learn_lvl1_lat = false;
	} else if (current_lvl1 && prec_lvl1 && prec_lvl1 != current_lvl1 && current_lvl1->state_activation() > 0.3
			&& Behavior::behavior_get().nb_trial_get() >= 6) {
		learn_lvl1_lat = true;
	}

	prec_col_lvl_[0] = current_lvl0;
	if (current_lvl1->state_activation() > 0.3 && !learn_lvl1_lat) {
		old_win_lvl1_ = prec_col_lvl_[1];
		prec_col_lvl_[1] = current_lvl1;
	}
  	
  	return col_changed;	
}

void Columns::correct_transition ()
{
	if (win_col_lvl_[0] == 0) {
		return;	
	}
	Action a (Behavior::behavior_get().angle_get ());
	vector<Minicol*> all_minicols = minicol_get (win_col_lvl_[0]->no_get (), a);
	for (vector<Minicol*>::iterator it = all_minicols.begin (); it != all_minicols.end (); ++it) {
		Minicol* real_minicol = *it;
  		// on s'est cogne a un mur alors qu'on pouvait faire la transition
  		stringstream message;
  		message << "0 " << real_minicol->from_get().no_get()+1
  				<< " " << real_minicol->to_get().no_get()+1 << " " << a.angle_get();
  		Logger::log ("network", message.str (), true);
  		lateral_learning_lvl0 (const_cast<Column&>(real_minicol->from_get()),
  							const_cast<Column&>(real_minicol->to_get()), 
  							real_minicol->action_get(), false);
	}

	if (old_win_lvl1_ != 0) {
		cout << "correction: " << old_win_lvl1_->no_get() << "->" << win_col_lvl_[1]->no_get() << endl;
		Minicol* mini_lvl1 = minicol_get(old_win_lvl1_->no_get(), win_col_lvl_[1]->no_get());
		if (mini_lvl1 != 0)
			mini_lvl1->increase_lvl1_set(false);
	}
}

Column* Columns::nocol_get (int no) const
{
	vector<Column*>::const_iterator it;
	it = find_if (columns_.begin (), columns_.end (), bind (&Column::no_get, _1) == no);
	return (it == columns_.end ()) ? 0 : *it;
}

void Columns::lateral_learning_lvl0 (Column& from, Column& to, const Action& action, bool increase)
{
	stringstream message;
	Minicol* minicol = minicol_get (from.no_get (), to.no_get ());
  	if (minicol) {
  		// la minicolonne existe déjà : on modifie les poids
  		minicol->lateral_learning_lvl0(increase);
      	if (from.level_get () == 0) {
	      	// on moyenne l'orientation
	      	message << "1 " << from.no_get ()+1 << " " << to.no_get ()+1 << " " 
	      			<< minicol->action_get ().angle_get ();
	      	minicol->adapt_action (action);
//	      	message << " / " << minicol->action_get ().angle_get ();
      	}
  	}
  	else {
  		// on recrute une nouvelle minicolonne
      	minicol = add_minicol (action, from, to, from.level_get (), 0, 0);
      	minicol->lateral_learning_lvl0(true);
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

void Columns::lateral_learning_lvl1(Column& from_1, Column& from_0, Column& to_1, Column& to_0, const Action& action)
{
	stringstream message;
	Minicol* minicol_1 = minicol_get(from_1.no_get(), to_1.no_get());
	Minicol* minicol_0 = minicol_get(from_0.no_get(), to_0.no_get());
  	if (minicol_1 == 0 && from_1.no_get() != to_1.no_get()) {
      	minicol_1 = add_minicol(action, from_1, to_1, 1, &minicol_0->inf_get(), &to_0.sup_get());
      	static const double LVL1_MODULATION = Params::get_double ("LVL1_MODULATION");
      	if (minicol_0->sup_get().syn_mod_get() == 0)
      		minicol_0->sup_get().add_synapse_modulation (minicol_1->sup_get(), LVL1_MODULATION);
      	to_0.inf_get().add_synapse(to_1.inf_get(), 1);
    } else if (minicol_1 != 0) {
    	minicol_1->increase_lvl1_set(true);
    }
	if (message.str () != "") {
		Logger::log ("network", message.str (), true);
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
		message << (*it)->no_get ()+1 << " ";
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

vector<Minicol*> Columns::minicol_get (int from, const Action& action) const
{
	vector<Minicol*> res;
	vector<Minicol*>::const_iterator it;
	for (it = minicols_.begin (); it != minicols_.end (); it++) {
		if ((*it)->recruited_get () && (*it)->level_get () == 0
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

void Columns::winner_takes_all_lvl0 (const vector<ComputeUnit*>& pop_state)
{
	if (Behavior::behavior_get().nb_trial_get() < 3)
		return; 
	static const double RATE_PC_COL = Params::get_double ("RATE_PC_COL");	
//	static const double INIT_PC_COL = Params::get_double ("INIT_PC_COL");
	int nbunits = pop_state.size ();
	vector<Column*>::iterator it;
	for (it = columns_.begin (); it != columns_.end (); it++) {
//		if ((*it)->level_get () == 0 && (win_col_lvl_[0] == *it || newcol)) {
		if ((*it)->level_get () != 0) {
			continue;	
		}
		Neuron& state = (*it)->state_get ();
		for (int i = 0; i < nbunits; i++) {
			ComputeUnit* unit = pop_state.at (i);
			double* w = state.syn_get(*unit);
//			if (w == 0) {
//				double init_val = INIT_PC_COL * drand ();
			if (w == 0 && *it == win_col_lvl_[0]) {
				double init_val = unit->spiking () ? unit->output () : 0;
//				double init_val = 1 / (1 + exp(-10*(unit->output() - 0.3)));
				state.add_synapse (*unit, init_val);
			}
			else if (w != 0 && *it == win_col_lvl_[0]) {
//				if (*it == win_col_lvl_[0]) {
//					cout << "delta_w " << (*it)->weight_change (unit, s->w_get (), win_col_lvl_[0]);
//				}
				double new_w = (*it)->weight_change (unit, *w, win_col_lvl_[0]);
//				static const string LEARN_RULE = Params::get ("LEARN_RULE");
//				if (LEARN_RULE == "simple") {
					*w = *w + RATE_PC_COL * new_w;
//				}
//				else if (LEARN_RULE == "simple_seuil") {
//					*w = *w + RATE_PC_COL * new_w;
//					if (*w > 1) {
//						*w = 1;
//					}
//					else if (*w < 0) {
//						*w = 0;
//					}
//				}
//				else if (LEARN_RULE == "mult") {
//					// !!! j'ai *2 pour compenser l'appr plus lent du au *w
//					*w = *w + *w * 2 * RATE_PC_COL * new_w;
//				}
			}
		}
	}	
}

void Columns::winner_takes_all_lvl1 (const vector<Column*>& pop, const ComputeUnit& ego_action)
{
	if (Behavior::behavior_get().nb_trial_get() < 6)
		return; 
//	cout << "egoaction: " << ego_action.output() << endl;
	static const double RATE_COL_COL = Params::get_double ("RATE_COL_COL");
	int nbunits = pop.size ();
	vector<Column*>::iterator it;
	for (it = columns_.begin (); it != columns_.end (); it++) {
//		if ((*it)->level_get () != 1 || !ego_action.spiking ()) {
		if ((*it)->level_get () != 1) {
			continue;	
		}
		Neuron& state = (*it)->state_get ();
//		static const double EGO_MODULATION = Params::get_double ("EGO_MODULATION");
//		if (state.syn_mod_get() == 0)
//			state.add_synapse_modulation (ego_action, EGO_MODULATION);
		for (int i = 0; i < nbunits; i++) {
			Column* col = pop.at (i);
			Neuron& unit = col->state_get ();
			if (col != win_col_lvl_[0] || win_col_lvl_[1] != *it || (!new_lvl1_ && !ego_action.spiking())) {
				continue;	
			}
			double* w = state.syn_get(unit);
			double rj = unit.output ();
			//	cout << "nb_col0 " << nb_spiking_cols (0) << endl;
//			static const double THRESH_ADD_LVL1 = Params::get_double("THRESH_ADD_LVL1");
//			if (w == 0 && nb_spiking_cols (0) > THRESH_ADD_LVL1) {
			if (w == 0) {
//				double init_val = 1;
//				double init_val = unit.spiking () ? rj : 0;
				double init_val = 1 / (1 + exp(-10*(unit.output() - 0.3)));
				state.add_synapse (unit, init_val);
//    			cout << "new syn " << unit.no_get() << "-> " << (*it)->no_get() << " : " << rj << endl; 
			} 
			else if (w != 0) {
				*w = *w + RATE_COL_COL * (rj - *w > 0 ? 1 : 0) * rj * state.output () * ego_action.output();
//				cout << "update lien " << unit.no_get() << " (" << unit.level_get () 
//					 << ") -> " << no_ << " (" << state_.level_get () << ") : " << s->w_get () << endl;					
			} 
		}
	}
}


