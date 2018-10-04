#include "column.hh"
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <algorithm>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include "neuralnet.hh"
#include "hippo.hh"
#include "cell.hh"
#include "params.hh"
#include "mystr.hh"
#include "computeunit.hh"
#include "math.hh"
#include "columns.hh"

using namespace std;
using namespace boost::lambda;

bool operator== (const Column& c1, const Column& c2)
{
  return c1.no_get() == c2.no_get();
}

Column::Column (Neuralnet& net, const Columns& columns, const vector<ComputeUnit*>& pop, int no, 
	int level, bool draw, ComputeUnit* ego_action, const Coord& pos) :
	no_(no), state_(net.add_neuron_max (no, level)), 
	sup_(net.add_neuron_max (no, level)), inf_(net.add_neuron_max (no, level)), 
	net_(net), columns_(columns), pop_state_(pop), ego_action_(level == 1?ego_action:0),
	lastmincol_(0), level_(level), draw_(draw), winner_(true), pos_(pos), maxr(0),
	log_("cols", no_), logw_("weights", no_)
{
	connect_pop_to_neuron ();
//	cout << "col" << no_ << " lvl" << level_ << " created!" << endl;
}

Column::~Column ()
{
	for_each (minicols_.begin (), minicols_.end (), bind (delete_ptr(), _1));
	minicols_.clear ();
}

bool Column::lateral_learning (Action* action, const Column& dest, bool increase, string & message)
{  	
	Minicol* minicol = minicol_get_nodest (dest.no_get ());
  	
  	if (minicol) {
  		// la minicolonne existe déjà : on modifie les poids
  		net_.lateral_learning (minicol->inf_get (), dest.inf_get (), increase);
      	net_.lateral_learning (dest.sup_get (), minicol->sup_get (), increase);
      	
      	if (level_ == 0) {
	      	// on moyenne l'orientation
	      	message = "update minicol " + i2str (no_) + "->" + i2str (dest.no_get ()) + " : " 
	      			+ d2str (minicol->action_get ().angle_get ());
	      	minicol->adapt_action (action);
	      	message += " / " + d2str (minicol->action_get ().angle_get ());
      	}
		return false;
  	}
  	else {
  		// on recrute une nouvelle minicolonne
      	minicol = new Minicol (net_, action, *this, dest, no_, minicols_.size (), level_);
      	minicols_.push_back (minicol);
      	message = "new minicol " + i2str (no_) + "->" + i2str (dest.no_get ());
      	if (level_ == 0) {
      		message += " : " + d2str (action->angle_get ());
      	}
      	return true;
    }
}

bool Column::minicol_spiking () const
{
	vector<Minicol*>::const_iterator it;
	it = find_if (minicols_.begin (), minicols_.end (), 
		bind (&Minicol::spiking, _1));
	return (it == minicols_.end ()) ? false : true;
}

Minicol* Column::minicol_get (const Action& action) const
{
	vector<Minicol*>::const_iterator it;
	it = find_if (minicols_.begin (), minicols_.end (), 
		level_ == 0 && bind (&Minicol::action_get, _1) == action);
	return (it == minicols_.end ()) ? 0 : *it;
}

Minicol* Column::best_minicol () const
{
	vector<Minicol*>::const_iterator it;
//	cout << "col " << no_ << endl;
//	for (it = minicols_.begin (); it != minicols_.end (); it++) {
//		cout << (*it)->activation () << endl;	
//	}
	it = max_element (minicols_.begin (), minicols_.end (),
		bind (&Minicol::activation, _1) < bind (&Minicol::activation, _2));
	return (it == minicols_.end ()) ? 0 : *it;
}

Minicol* Column::best_mean_minicol () const
{
	vector<Minicol*>::const_iterator it;
	it = max_element (minicols_.begin (), minicols_.end (), 
		bind (&Minicol::mean_value_get, _1) < bind (&Minicol::mean_value_get, _2));
	return (it == minicols_.end ()) ? 0 : *it;
}

void Column::synch (bool learn, const Coord& pos)
{
	for_each (minicols_.begin (), minicols_.end (), bind (&Minicol::update_value, _1));

	Minicol* best = best_minicol ();
	if (best && best->spiking ()) {
		lastmincol_ = best;
	}
	
	if (learn) {
		connect_pop_to_neuron ();
	}
  	
	if (level_ == 0) {
		center_rf (pos);
	}
}

void Column::draw (ostream& os) const
{
	stringstream ss;
	string       val;
	ss << no_;
	ss >> val;
	os << "c" << val << " [label=\"{<n" << state_.no_get () << "> H} | {<n" << sup_.no_get () << "> Prec | {";
	(*minicols_.begin ())->draw (os);
	for_each (minicols_.begin () + 1, minicols_.end (), 
		(var (os) << " | ", bind (&Minicol::draw, _1, var (os))));
	os << "} | <n" << inf_.no_get () << "> Suiv} \"];" << endl;
}

Minicol* Column::minicol_get_nodest (int no) const
{
	vector<Minicol*>::const_iterator it;
	it = find_if (minicols_.begin (), minicols_.end (), 
		bind (&Column::no_get, bind (&Minicol::to_get, _1)) == no);
	return (it == minicols_.end ()) ? 0 : *it;
}

int Column::nb_spiking_cells (int level) const
{
	return count_if (pop_state_.begin (), pop_state_.end (), 
		bind (&ComputeUnit::level_get, _1) == level && bind (&ComputeUnit::spiking, _1)); 
}

void Column::connect_pop_to_neuron ()
{
	// TODO: pour lvl0 et lvl1, traiter le cas !winner_ pour dévaluer les poids ?

//	if (level_ == 0 && winner_ && nb_spiking_cells () >= 6) {
	if (level_ == 0 && winner_) {
		static const int LEARN_PC_COL = Params::get_int ("LEARN_PC_COL");		
		int nbunits = pop_state_.size ();
		for (int i = 0; i < nbunits; i++) {
			ComputeUnit* unit = pop_state_.at (i);
			double rj = unit->output ();
			Synapse* s = state_.syn_get(*unit);
			if (s == 0) {
//				double init_val = unit->spiking() ? (state_.output () > 0 ? state_.output () : rj) : 0;
				double init_val = unit->spiking() ? rj : 0;
				state_.add_synapse (*unit, init_val, false);
			}
			else if (s != 0 && LEARN_PC_COL == 1) {
				s->w_set (s->w_get () + 0.01 * (rj - s->w_get ()) * (state_.output ()));
				// code pour la regle BCM (marche pas)
//				double t = state_.thetaM_get (), y = state_.output ();
//				s->w_set (s->w_get () + 0.01 * 1 / t * rj * (y - t) * y);
//				state_.thetaM_set (t + 0.1 * (exp(y) - t));
			}			
		}
		// TODO: traiter le cas de nouvelles cellules de lieux fraichement ajoutees ?
		// en utilisant ri à la place de rj pour les inits ? ou ri * rj ?
	}
	else if (level_ == 1 && winner_ && ego_action_->spiking ()) {
		int nbcols = columns_.size ();
		for (int i = 0; i < nbcols; i++) {
			Column* col = columns_.col_get (i);
			if (col->level_get () >= level_get ()) {
				continue;
			}
			
			Neuron& unit = col->state_get ();
			Synapse* s = state_.syn_get(unit);
			double rj = unit.output ();
			if (s == 0) {
				static const double EGO_MODULATION = Params::get_double ("EGO_MODULATION");
				static const double LVL0_TO_LVL1 = Params::get_double ("LVL0_TO_LVL1");
				state_.add_synapse (unit, *ego_action_, unit.spiking() ? rj : 0, LVL0_TO_LVL1, EGO_MODULATION);
//    			cout << "new lien " << unit.no_col_get() << " (" << unit.level_get () 
//    				 << ") -> " << no_ << "(" << state_.level_get () << ") : " << rj << endl; 
			}
			else if (s != 0) {
				s->w_set (s->w_get () + 0.2 * (((rj - s->w_get ()) > 0)?1:0) * rj * state_.output ());
//				cout << "update lien " << unit.no_col_get() << " (" << unit.level_get () 
//					 << ") -> " << no_ << " (" << state_.level_get () << ") : " << s->w_get () << endl;					
			}
			else {
//				cout << "lien " << unit.no_col_get() << " (" << unit.level_get () << ") -> " 
//					 << no_ << " (" << state_.level_get () << ") : none" << endl;
			}
				
			Neuron& unit_sup = col->sup_get ();
			s = sup_.syn_get(unit_sup);
			if (s == 0 && unit.spiking ()) {
				static const double LVL1_MODULATION = Params::get_double ("LVL1_MODULATION");
				sup_.add_synapse (unit_sup, 1, true); // à remplacer par un apprentissage basé sur l'activité de col-> state
				unit_sup.add_synapse_modulation (sup_, LVL1_MODULATION);
			}
		}
	}
}

void Column::log (const string& time_string, const Coord& position, double angle, int day, int trial)
{
	ostringstream msg;
	msg << "# " << time_string << endl;
	msg << position.x_get () << " " << position.y_get () << " " << angle << " "
  				<< state_.output () << " " << inf_.output () << " " << sup_.output () << endl << endl;
  	log_.log (day, trial, msg.str ());
  	for_each (minicols_.begin (), minicols_.end (), bind (&Minicol::log, _1, time_string, position, angle, day, trial));
}

void Column::log_state_weights (const string& time_string, int day, int trial)
{
	ostringstream msg;
	msg << "# " << time_string << endl;
  	state_.print_weights (msg);
	msg << endl;
	logw_.log (day, trial, msg.str ());	
}

void Column::center_rf (const Coord& pos)
{
	if (state_.output () > maxr) {
		maxr = state_.output ();
		pos_ = pos;
	}
}
