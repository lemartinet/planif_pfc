#include "columns.hh"
#include "column.hh"
#include "action.hh"
#include "neuron.hh"
#include "params.hh"
#include "coord.hh"
#include "minicol.hh"
#include "math.hh"
#include "cell.hh"
#include "device.hh"
#include "logger.hh"
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
	unsigned int lvl0_nb = SIZE_POP * 0.75;
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
	// ajoute une connexion aléatoire avec hippo
	winner_takes_all_lvl0 (hippo_pop);
	winner_takes_all_lvl1 (pop_, ego);
	
	win_col_lvl_[0] = win_col_lvl_ [1] = 0;
	prec_col_lvl_[0] = prec_col_lvl_ [1] = 0;
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
	for_each (neurons_.begin (), neurons_.end (), bind (delete_ptr(), _1));
	neurons_.clear();
}

void Columns::winner_col (int level)
{
	Column* best_col = 0;
	vector<Column*>::iterator it;
	for (it = columns_.begin (); it != columns_.end (); it++) {
		if ((*it)->level_get () != level) {
			continue;
		}
//		else if (best_col == 0 || (best_col->state_activation () < (*it)->state_activation ())){
		else if (best_col == 0 || (best_col->lastT_recent () < (*it)->lastT_recent ())){
//		else if (best_col == 0 || (best_col->lastT_recent () < (*it)->lastT_recent ()
//					&& ((*it)->state_activation () > 0.2 || (*it)->state_activation () < 0.1))){
			best_col = *it;
		}
	}
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

Minicol* Columns::add_minicol (const Action& action, Column& src, Column& dest, int level)
{
	static const int SIZE_POP = Params::get_int("SIZE_POP");
	if (nb_used_minicol_ >= SIZE_POP) {
		cout << "[Mypuck] Too much minicolumns" << endl;
		return 0;	
	}
	Minicol* mc = minicols_[nb_used_minicol_];
	mc->new_set (action, src, dest, level);
	nb_used_minicol_++;
	return mc;	
}

double Columns::nb_spiking_cells (int level) const
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

	vector<Column*>::iterator it;
	for (it = columns_.begin (); it != columns_.end (); it++) {
		if ((*it)->level_get () == 0) {
			(*it)->synch ();
			if (pos != 0) {
				(*it)->center_rf (*pos, *it == win_col_lvl_[0]);	
			}
		}
		else if ((*it)->level_get () == 1) {
			(*it)->synch ();	
		} 
	}
  	for_each (minicols_.begin (), minicols_.end (), bind (&Minicol::update_value, _1));
  	
  	winner_col (0);
  	winner_col (1);
  	winner_minicol (0);
  	winner_minicol (1);
  	if (learn) {
  		winner_takes_all_lvl0 (hippo_pop);
  		winner_takes_all_lvl1 (pop_, ego);
  		return topology_learning ();	
  	}
  	else {
  		return false;	
  	}

//  	cout << "nb_spiking_cells: " << nb_spiking_cells (0) << endl;
//	columns_.show_activities (0);
//	columns_.show_activities (1);

}

bool Columns::topology_learning ()
{
  	bool col_changed = false;
  	Column* current_lvl0 = win_col_lvl_[0];
  	Column* prec_lvl0 = prec_col_lvl_[0];	
//	if (current_lvl0 && prec_lvl0 && prec_lvl0 != current_lvl0) {	
//		cout << prec_lvl0->no_get () << "/" << prec_lvl0->lastT_recent () << " -> "  
//			<< current_lvl0->no_get () << "/" << current_lvl0->lastT_recent () 
//			<< " total " << nb_spiking_cells (0) << endl;
//	}
	if (current_lvl0 && prec_lvl0 && prec_lvl0 != current_lvl0 
//		&& prec_lvl0->lastT_recent () * current_lvl0->lastT_recent () > 0.1
//		&& prec_lvl0->lastT_recent () * current_lvl0->lastT_recent () < 0.3
		&& nb_spiking_cells (0) < 10) {
//		cout << "update minicol" << endl;
    	col_changed = true;
      	Action action (RobotDevice::robot_get ().angle_get());
      	lateral_learning (*prec_lvl0, *current_lvl0, action, true);
  	}
	prec_col_lvl_[0] = current_lvl0;
	  	
	Column* current_lvl1 = win_col_lvl_[1];
	Column* prec_lvl1 = prec_col_lvl_[1];
//	if (current_lvl1 && prec_lvl1 && prec_lvl1 != current_lvl1 && nb_spiking_cells (0) < 10) {
//		columns_.lateral_learning (*prec_lvl1, *current_lvl1, 0, true); attention pas 0 car réf
//	}
	prec_col_lvl_[1] = current_lvl1;
  	
  	return col_changed;	
}

void Columns::correct_transition ()
{
	if (win_col_lvl_[0] == 0) {
		return;	
	}
	Action a (RobotDevice::robot_get ().angle_get ());
	vector<Minicol*> all_minicols = minicol_get (win_col_lvl_[0]->no_get (), a);
	for (vector<Minicol*>::iterator it = all_minicols.begin (); it != all_minicols.end (); ++it) {
		Minicol* real_minicol = *it;
  		// on s'est cogne a un mur alors qu'on pouvait faire la transition
  		stringstream message;
  		message << "0 " << real_minicol->from_get().no_get()+1
  				<< " " << real_minicol->to_get().no_get()+1 << " " << a.angle_get();
  		Logger::log ("network", RobotDevice::robot_get ().cpt_total_get (), message.str ());
  		lateral_learning (const_cast<Column&>(real_minicol->from_get()), 
  							const_cast<Column&>(real_minicol->to_get()), 
  							real_minicol->action_get(), false);
	}
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

void Columns::lateral_learning (Column& from, Column& to, const Action& action, bool increase)
{
	stringstream message;
	Minicol* minicol = minicol_get (from.no_get (), to.no_get ());
  	if (minicol) {
  		// la minicolonne existe déjà : on modifie les poids
  		minicol->lateral_learning (increase);      	
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
      	minicol = add_minicol (action, from, to, from.level_get ());
      	message << "2 " << from.no_get ()+1 << " " << to.no_get ()+1
   				<< " " << action.angle_get ();
    }
	if (message.str () != "") {
		Logger::log ("network", RobotDevice::robot_get ().cpt_total_get (), message.str ());
	}
	
	// on ajoute la competition entre les minicols d'une meme col
	// cela permettrait de diminuer les poids parasites
	vector<Minicol*>::const_iterator it;
	for (it = minicols_.begin (); it != minicols_.end (); it++) {
		if ((*it)->recruited_get () && (*it)->from_get () == from && (*it) != minicol) {
			(*it)->lateral_learning (false, 0.1);
		}
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
	static const double RATE_PC_COL = Params::get_double ("RATE_PC_COL");	
//	static const double INIT_PC_COL = Params::get_double ("INIT_PC_COL");
	int nbunits = pop_state.size ();
	vector<Column*>::iterator it;
	for (it = columns_.begin (); it != columns_.end (); it++) {
//		if ((*it)->level_get () == 0 && win_col_lvl_[0] == *it && nb_spiking_cells () >= 6) {
//		if ((*it)->level_get () == 0 && (win_col_lvl_[0] == *it || newcol)) {
		if ((*it)->level_get () != 0) {
			continue;	
		}
		Neuron& state = (*it)->state_get ();
		for (int i = 0; i < nbunits; i++) {
			Cell* unit = dynamic_cast<Cell*>(pop_state.at (i));
			Synapse* s = state.syn_get(*unit);
//			if (s == 0) {
			if (s == 0 && *it == win_col_lvl_[0]) {
//				double init_val = INIT_PC_COL * drand ();
				double init_val = unit->spiking () ? unit->output () : 0;
				state.add_synapse (*unit, init_val, false);
			}
			else if (s != 0) {
//				if (*it == win_col_lvl_[0]) {
//					cout << "delta_w " << (*it)->weight_change (unit, s->w_get (), win_col_lvl_[0]);
//				}
				double new_w = (*it)->weight_change (unit, s->w_get (), win_col_lvl_[0]);
				static const string LEARN_RULE = Params::get ("LEARN_RULE");
				if (LEARN_RULE == "simple") {
					s->w_set (s->w_get () + RATE_PC_COL * new_w);
				}
				else if (LEARN_RULE == "simple_seuil") {
					s->w_set (s->w_get () + RATE_PC_COL * new_w);
					if (s->w_get () > 1) {
						s->w_set (1);
					}
					else if (s->w_get () < 0) {
						s->w_set (0);
					}
				}
				else if (LEARN_RULE == "mult") {
					// !!! j'ai *2 pour compenser l'appr plus lent du au *w
					s->w_set (s->w_get () + s->w_get () * 2 * RATE_PC_COL * new_w);
				}
			}
		}
	}	
}

void Columns::winner_takes_all_lvl1 (const vector<Column*>& pop, const ComputeUnit& ego_action)
{
	cout << "ego: " << ego_action.output() << endl;
	int nbunits = pop.size ();
	vector<Column*>::iterator it;
	for (it = columns_.begin (); it != columns_.end (); it++) {
		if ((*it)->level_get () != 1 || win_col_lvl_[1] != *it || !ego_action.spiking ()) {
//		if ((*it)->level_get () != 1) {
			continue;	
		}
//		if ((*it)->state_get ().spiking()) {
//			cout << (*it)->no_get () << " " << (*it)->state_get ().output() << endl;
//		}
		Neuron& state = (*it)->state_get ();
		Neuron& sup = (*it)->sup_get ();
		for (int i = 0; i < nbunits; i++) {
			Column* col = pop.at (i);
			Neuron& unit = col->state_get ();
			Synapse* s = state.syn_get(unit);
			double rj = unit.output ();
			//	cout << "nb_col0 " << columns_.nb_spiking_cells (0) << endl;
			// ca marche pas mal avec 1.3 = .7 + .3 + .3 par ex
//			static const double THRESH_ADD_LVL1 = Params::get_double("THRESH_ADD_LVL1");
//			if (s == 0 && nb_spiking_cells (0) > THRESH_ADD_LVL1) {
//			if (s == 0 && nb_spiking_cells (0) > 1) {
			if (s == 0) {
				static const double EGO_MODULATION = Params::get_double ("EGO_MODULATION");
				static const double LVL0_TO_LVL1 = Params::get_double ("LVL0_TO_LVL1");
				double init_val = unit.spiking () ? rj : 0;
				state.add_synapse (unit, ego_action, init_val, LVL0_TO_LVL1, EGO_MODULATION);
//    			cout << "new syn " << unit.no_get() << "-> " << (*it)->no_get() << " : " << rj << endl; 
			}
			else if (s != 0) {
				s->w_set (s->w_get () + 0.2 * (((rj - s->w_get ()) > 0)?1:0) * rj * state.output ());
//				cout << "update lien " << unit.no_get() << " (" << unit.level_get () 
//					 << ") -> " << no_ << " (" << state_.level_get () << ") : " << s->w_get () << endl;					
			}
			
			Neuron& unit_sup = col->sup_get ();
			s = sup.syn_get(unit_sup);
			if (s == 0 && unit.spiking ()) {
				static const double LVL1_MODULATION = Params::get_double ("LVL1_MODULATION");
				// TODO: à remplacer par un apprentissage basé sur l'activité de col-> state
				sup.add_synapse (unit_sup, 1, true); 
				unit_sup.add_synapse_modulation (sup, LVL1_MODULATION);
			}
		}
	}
}


