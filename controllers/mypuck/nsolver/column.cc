#include "column.hh"
#include "coord.hh"
#include "neuralnet.hh"
#include "hippo.hh"
#include "cell.hh"
#include "params.hh"
#include "mystr.hh"
#include "computeunit.hh"
#include "math.hh"
#include "columns.hh"
#include "logger.hh"
#include <sstream>
#include <iostream>

bool operator== (const Column& c1, const Column& c2)
{
  return c1.no_get() == c2.no_get();
}

Column::Column (Neuralnet& net, Columns& columns, int no, const vector<ComputeUnit*>& hippo_pop) :
	no_(no), state_(net.add_neuron_max ()), sup_(net.add_neuron_max ()), inf_(net.add_neuron_max ()), 
	net_(net), columns_(columns), level_(0), winner_(false), pos_(0), maxr(0) 
{
	Logger::add ("state", &state_);
	Logger::add ("lat", &inf_);
	Logger::add ("maxsup", &sup_);
	Logger::add ("weights", &state_);
	// ajoute une connexion aléatoire avec hippo
	connect_pop_to_neuron (hippo_pop, true);
}

Column::~Column ()
{
	if (pos_ != 0) {
		delete pos_;
	}
}

void Column::new_set (int level, const vector<ComputeUnit*>& hippo_pop, const Coord& pos)
{
	level_ = level;
	pos_ = new Coord(pos);
	winner_ = true;
	connect_pop_to_neuron (hippo_pop);
}

void Column::new_set (int level, const vector<ComputeUnit*>& pop, const ComputeUnit& ego_action)
{
	level_ = level;
	winner_ = true;
	connect_pop_to_neuron (pop, ego_action);
}

void Column::synch (bool learn, const Coord& pos, const vector<ComputeUnit*>& pop_state)
{
	if (learn) {
		connect_pop_to_neuron (pop_state);
	}
	center_rf (pos);
}

void Column::synch (bool learn, const vector<ComputeUnit*>& pop_state, const ComputeUnit& ego_action)
{
	if (learn) {
		connect_pop_to_neuron (pop_state, ego_action);
	}
}

void Column::connect_pop_to_neuron (const vector<ComputeUnit*>& pop_state, bool newcol)
{
	// TODO: pour lvl0 et lvl1, traiter le cas !winner_ pour dévaluer les poids ?
//	if (level_ == 0 && winner_ && nb_spiking_cells () >= 6) {
	if (level_ == 0 && (winner_ || newcol)) {
		cout << "col " << no_ << " is learning" << endl;
		static const int LEARN_PC_COL = Params::get_int ("LEARN_PC_COL");	
		int nbunits = pop_state.size ();
		for (int i = 0; i < nbunits; i++) {
			ComputeUnit* unit = pop_state.at (i);
			double rj = unit->output ();
			Synapse* s = state_.syn_get(*unit);
			if (s == 0) {
				double init_val;
				if (newcol) {
					init_val = 0.1 * drand ();
				}
				else {
//					init_val = unit->spiking() ? (state_.output () > 0 ? state_.output () : rj) : 0;
					init_val = unit->spiking() ? rj : 0;
					cout << "one shot" << endl;
				}
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
}

void Column::connect_pop_to_neuron (const vector<ComputeUnit*>& pop_state, const ComputeUnit& ego_action)
{
	// TODO: on utilise pas pop_state mais la pop des colonnes !!! A changer !
	if (level_ == 1 && winner_ && ego_action.spiking ()) {
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
				state_.add_synapse (unit, ego_action, unit.spiking() ? rj : 0, LVL0_TO_LVL1, EGO_MODULATION);
//    			cout << "new lien " << unit.no_get() << " (" << unit.level_get () 
//    				 << ") -> " << no_ << "(" << state_.level_get () << ") : " << rj << endl; 
			}
			else if (s != 0) {
				s->w_set (s->w_get () + 0.2 * (((rj - s->w_get ()) > 0)?1:0) * rj * state_.output ());
//				cout << "update lien " << unit.no_get() << " (" << unit.level_get () 
//					 << ") -> " << no_ << " (" << state_.level_get () << ") : " << s->w_get () << endl;					
			}
			else {
//				cout << "lien " << unit.no_get() << " (" << unit.level_get () << ") -> " 
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

void Column::center_rf (const Coord& pos)
{
	if (pos_ != 0 && state_.output () > maxr) {
		maxr = state_.output ();
		*pos_ = pos;
	}
}
