#include "column.hh"
#include "coord.hh"
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

Column::Column (Columns& columns, int no, const vector<ComputeUnit*>& hippo_pop) :
	no_(no), state_(columns.add_neuron_max (STATE)), 
	sup_(columns.add_neuron_max (MAXSUP)), inf_(columns.add_neuron_max (LAT)), 
	columns_(columns), level_(0), winner_(false), pos_(0), maxr(0) 
{
	stringstream s1, s2, s3;
	s1 << state_.no_get () << " " << no;
	Logger::log("col", s1.str());
	s2 << sup_.no_get () << " " << no;
	Logger::log("col", s2.str());
	s3 << inf_.no_get () << " " << no;
	Logger::log("col", s3.str());

	// ajoute une connexion aléatoire avec hippo
	connect_pop_to_neuron (hippo_pop, true);
}

Column::~Column ()
{
	if (pos_ != 0) {
		delete pos_;
	}
}

void Column::synch (bool learn, const vector<ComputeUnit*>& pop_state)
{
	if (learn) {
		connect_pop_to_neuron (pop_state);
	}
}

void Column::synch (bool learn, const Coord& pos, const vector<ComputeUnit*>& pop_state)
{
	synch (learn, pop_state);
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
		static const double RATE_PC_COL = Params::get_double ("RATE_PC_COL");	
		int nbunits = pop_state.size ();
		for (int i = 0; i < nbunits; i++) {
			ComputeUnit* unit = pop_state.at (i);
			double rj = unit->output ();
			Synapse* s = state_.syn_get(*unit);
			if (s == 0) {
				double init_val = 0.1 * drand ();
				state_.add_synapse (*unit, init_val, false);
			}
			else if (s != 0) {
				s->w_set (s->w_get () + RATE_PC_COL * (rj - s->w_get ()) * (state_.output ()));
				// code pour la regle BCM (marche pas)
//				double t = state_.thetaM_get (), y = state_.output ();
//				s->w_set (s->w_get () + 0.01 * 1 / t * rj * (y - t) * y);
//				state_.thetaM_set (t + 0.1 * (exp(y) - t));
			}			
		}
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
	if (pos_ == 0) {
		pos_ = new Coord(pos);
	}
	else if (state_.output () > maxr) {
		maxr = state_.output ();
		*pos_ = pos;
	}
}
