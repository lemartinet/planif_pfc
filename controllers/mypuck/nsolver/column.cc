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
//#include <numeric>
#include <algorithm>

#define NB_STEP 8

bool operator== (const Column& c1, const Column& c2)
{
  return c1.no_get() == c2.no_get();
}

Column::Column (Columns& columns, int no) :
	no_(no), state_(columns.add_neuron_max (STATE)), 
	sup_(columns.add_neuron_max (MAXSUP)), inf_(columns.add_neuron_max (LAT)), 
	columns_(columns), level_(0), pos_(0), maxr(0), lastTidx_(0)
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
	// on garde un historique de l'activation sur X time steps
	if (lastTrecent_.size () < NB_STEP) {
		lastTrecent_.push_back (state_.output ());
		lastTold_.push_back (state_.output ());
	}
	else {
		lastTold_[lastTidx_] = lastTrecent_[lastTidx_];
		lastTrecent_[lastTidx_] = state_.output ();
		lastTidx_ = ++lastTidx_ % NB_STEP;
	}
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

double Column::lastT_recent () const
{
//	double total = accumulate (lastTrecent_.begin(), lastTrecent_.end(), 0.0);
//	// on retire l'activité moyenne pendant le creux, et on moyenne sur les peaks 
//	return (total - NB_STEP/2 * 0.05) / (NB_STEP/2);
	return *max_element (lastTrecent_.begin(), lastTrecent_.end());
}

double Column::lastT_old () const
{
//	double total = accumulate (lastTold_.begin(), lastTold_.end(), 0.0);
//	// on retire l'activité moyenne pendant le creux, et on moyenne sur les peaks 
//	return (total - NB_STEP/2 * 0.05) / (NB_STEP/2);
	return *max_element (lastTold_.begin(), lastTold_.end());
}

double Column::weight_change (const Cell* cell, double old_w, Column* winner)
{
	if (lastTrecent_.size () < NB_STEP) {
		return 0;
	}
	if (this == winner) {
		// ancienne methode simple
	//	return (cell->lastT_recent_max () - old_w) * lastT_recent ()
		// code pour la regle BCM (marche pas)
	//	double t = state_.thetaM_get (), y = state_.output ();
	//	state_.thetaM_set (t + 0.1 * (exp(y) - t));
	//	return 1 / t * cell->output () * (y - t) * y);
		
		vector<double> corr;
		const vector<double>& cell_last = cell->lastT_recent();
		for (int i = 0; i < NB_STEP; i++) {
			corr.push_back ((cell_last[i] - old_w) * lastTrecent_[i]);
			if (i < NB_STEP - 1) {
				corr.push_back ((cell_last[i] - old_w) * lastTrecent_[i+1]);
			}	
		}
		return *max_element (corr.begin(), corr.end());
	}
	else {
		vector<double> corr;
		const vector<double>& cell_last = cell->lastT_recent();
		static const double SPARSE_P = Params::get_double ("SPARSE_P");
		for (int i = 0; i < NB_STEP; i++) {
			corr.push_back (cell_last[i] * (winner->lastTrecent_[i] * lastTrecent_[i] - SPARSE_P * SPARSE_P));
			if (i < NB_STEP - 1) {
				corr.push_back (cell_last[i] * (winner->lastTrecent_[i+1] * lastTrecent_[i+1] - SPARSE_P * SPARSE_P));
			}	
//			corr.push_back (winner->lastTrecent_[i] * lastTrecent_[i] - SPARSE_P * SPARSE_P);
		}
		return -2 * *max_element (corr.begin(), corr.end());
	}
} 
