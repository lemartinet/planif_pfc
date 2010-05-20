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

double Column::lastT_recent() const
{
	const vector<double>& last = state_.lastT_recent();
	return *max_element (last.begin(), last.end());
}

double Column::weight_change (const ComputeUnit* cell, double old_w, Column* winner)
{
//	// ancienne methode simple
//	return (cell->lastT_recent_max () - old_w) * state_.lastT_recent ()
//	// code pour la regle BCM (marche pas)
//	double t = state_.thetaM_get (), y = state_.output ();
//	state_.thetaM_set (t + 0.1 * (exp(y) - t));
//	return 1 / t * cell->output () * (y - t) * y);
		
	const vector<double>& cell_last = cell->lastT_recent();
	const vector<double>& state_last = state_.lastT_recent();
	const vector<double>& win_last = winner->state_get().lastT_recent();
	const int last_size = min(cell_last.size(), state_last.size());
	static const double SPARSE_P = Params::get_double ("SPARSE_P");
	double max_corr = 0;
	for (int i = 0; i < last_size; i++) {
		// pas de compétition pendant le sommeil
		if (Behavior::behavior_get().is_sleeping()) {
			const double val1 = cell_last[i] * state_last[i],
				val2 = (i < last_size - 1) ? cell_last[i] * state_last[i+1] : 0;
			max_corr = max(max(val1, val2), max_corr);
		} else if (this == winner) {
			const double val1 = (cell_last[i] - old_w) * state_last[i],
				val2 = (i < last_size - 1) ? (cell_last[i] - old_w) * state_last[i+1] : 0;
			max_corr = max(max(val1, val2), max_corr);
		} else {
			const double val1 = cell_last[i] * (win_last[i] * state_last[i] - SPARSE_P * SPARSE_P),
				val2 = (i < last_size - 1) ? cell_last[i] * (win_last[i+1] * state_last[i+1] - SPARSE_P * SPARSE_P) : 0;
			max_corr = max(max(val1, val2), max_corr);
		}
	}
	return max_corr * ((this == winner || Behavior::behavior_get().is_sleeping()) ? 1 : -0.5);

//	for (int i = 0; i < last_size; i++) {
//		// pas de compétition pendant le sommeil
//		if (Behavior::behavior_get().is_sleeping()) {
//			corr.push_back (cell_last[i] * state_last[i]);
//			if (i < last_size - 1) {
//				corr.push_back (cell_last[i] * state_last[i+1]);
//			}
//		} else if (this == winner) {
//			corr.push_back ((cell_last[i] - old_w) * state_last[i]);
//			if (i < last_size - 1) {
//				corr.push_back ((cell_last[i] - old_w) * state_last[i+1]);
//			}
//		} else {
//			const vector<double>& win_last = winner->state_get().lastT_recent();
//			corr.push_back(cell_last[i] * (win_last[i] * state_last[i] - SPARSE_P * SPARSE_P));
//			if (i < last_size - 1) {
//				corr.push_back(cell_last[i] * (win_last[i+1] * state_last[i+1] - SPARSE_P * SPARSE_P));
//			}
//		}
//	}
//	vector<double>::iterator it = max_element (corr.begin(), corr.end());
//	if (it != corr.end())
//		return ((this == winner || Behavior::behavior_get().is_sleeping()) ? 1 : -1) * *it;
//	else
//		return 0;
} 
