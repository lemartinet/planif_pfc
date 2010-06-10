#include "column.hh"
#include "coord.hh"
#include "params.hh"
#include "mystr.hh"
#include "computeunit.hh"
#include "math.hh"
#include "columns.hh"
#include "logger.hh"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <numeric>
#include "behavior.hh"

bool operator== (const Column& c1, const Column& c2)
{
  return c1.no_get() == c2.no_get();
}

Column::Column (Columns& columns, int no) :
	no_(no), state_(columns.add_neuron_max (STATE)), 
	sup_(columns.add_neuron_max (MAXSUP)), inf_(columns.add_neuron_max (LAT)), 
	columns_(columns), pos_(0), maxr(0)
{
	stringstream s1, s2, s3;
	s1 << state_.no_get ()+1 << " " << no+1;
	Logger::log("col", s1.str());
	s2 << sup_.no_get ()+1 << " " << no+1;
	Logger::log("col", s2.str());
	s3 << inf_.no_get ()+1 << " " << no+1;
	Logger::log("col", s3.str());
}

Column::~Column ()
{
	if (pos_ != 0) {
		delete pos_;
	}
}

void Column::synch ()
{
	state_.update_recent();
}

void Column::center_rf (const Coord& pos, bool winner)
{
	if (pos_ == 0) {
		pos_ = new Coord(pos);
	}
	else if (state_.output () > maxr) {
		maxr = state_.output ();
		*pos_ = pos;
	}
	else if (winner) {
		// pr obliger à remettre à jour la position
		maxr -= 0.001 * maxr;	
	}
}

double Column::weight_change (const ComputeUnit* cell, double old_w, Column* winner)
{
//	// ancienne methode simple
//	return (cell->lastT_recent_max () - old_w) * state_.lastT_recent ()
//	// code pour la regle BCM (marche pas)
//	double t = state_.thetaM_get (), y = state_.output ();
//	state_.thetaM_set (t + 0.1 * (exp(y) - t));
//	return 1 / t * cell->output () * (y - t) * y);
		
	static const double SPARSE_P = Params::get_double ("SPARSE_P");
	double sum_corr = 0;
	// pas de compétition pendant le sommeil
	if (Behavior::behavior_get().is_sleeping()) {
		sum_corr = cell->mean_recent() * state_.mean_recent();
	} else if (this == winner) {
		const double m = state_.mean_recent();
		sum_corr = cell->mean_recent() * m - old_w * m * m;
	} else {
		sum_corr = winner->state_get().mean_recent() * state_.mean_recent()
				- SPARSE_P * SPARSE_P;
	}
	return sum_corr * ((this == winner || Behavior::behavior_get().is_sleeping()) ? 1 : -0.5);
} 
