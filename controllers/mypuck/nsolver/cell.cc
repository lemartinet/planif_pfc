#include "cell.hh"
#include "math.hh"
#include "params.hh"
#include <fstream>
#include <iostream>
#include <numeric>
#include <algorithm>

#define NB_STEP 8

Cell::Cell () : ComputeUnit(PC), pos_(0), lastTidx_(0) 
{
}

Cell::Cell (const Coord& pos) : ComputeUnit(PC), pos_(new Coord (pos)), lastTidx_(0) 
{
}

bool sort_function (double* v1, double* v2)
{
	return v1[0] < v2[0] || (v1[0] == v2[0] && v1[1] < v2[1]); 	
}

Cell::Cell (const string& filename, const Coord& pos) : ComputeUnit(PC), pos_(new Coord (pos)), lastTidx_(0) 
{
	ifstream file (filename.c_str ());
	double x, y, r;
	while (file >> x >> y >> r) {
		r_.push_back (r);
		posr_.push_back (Coord (x,y));
	}
	file.close ();
	cout << "cell" << no_ << " loaded!" << endl;
}

Cell::~Cell () 
{
	if (pos_ != 0) {
		delete pos_;	
	}
}

void Cell::pos_set (const Coord& pos)
{
	if (pos_ == 0) {
		pos_ = new Coord(pos);
	}
	else {
		*pos_ = pos;	
	}
}

void Cell::compute (const Coord& pos, double peak)
{
	if (pos_ == 0) {
		output_ = 0;
	}
	else if (r_.size () == 0) {
		// Noise applied to hippocampus cells activity.
		static const double CELLS_NOISE_SIGMA = Params::get_double("CELLS_NOISE_SIGMA");
		// Ecart-type of hippocampus cells activation.
		static const double CELLS_SIGMA = Params::get_double("CELLS_SIGMA");
		static const double CELL_THRESH = Params::get_double("CELL_THRESH");
	  	double dist = (*pos_ - pos).norm ();
	
		//output_ = exp (-pow2 (dist) / (2 * CELLS_SIGMA * CELLS_SIGMA)) + bruit (2 * CELLS_NOISE_SIGMA);
		output_ = exp (-pow2 (dist) / (2 * CELLS_SIGMA * CELLS_SIGMA)) * (1 + bruit (2 * CELLS_NOISE_SIGMA));
		output_ = output_ < 0.0 ? 0.0 : output_;
		output_ = output_ > 1.0 ? 1.0 : output_;
		output_ = output_ > CELL_THRESH ? output_ : 0.0;
	}
	else {
		int min = 0;
		double dmin = (posr_[0] - pos).norm ();
		int rsize = r_.size ();
		for (int i = 1; i < rsize; i++) {
			double d = (posr_[i] - pos).norm ();
			if (d < dmin) {
				dmin = d;
				min = i;
			}
		}
		output_ = r_[min];
	}
	output_ *= peak;
//	cout << output_ << " ";	
	if (output_ < 0.05) {
		output_ = 0.05 + bruit(2 * 0.05);	
	}
	// on garde un historique de l'activation sur X time steps
	if (lastTrecent_.size () < NB_STEP) {
		lastTrecent_.push_back (output ());
	}
	else {
		lastTrecent_[lastTidx_] = output ();
		lastTidx_ = ++lastTidx_ % NB_STEP;
	}
}

void Cell::draw (ostream& os) const
{
	double cell_output = output (); 
    os << "c" << &cell_output << " [label=\"" << no_ << ":" << output () << "\"]" << endl;
}

const vector<double>& Cell::lastT_recent () const
{
//	double total = accumulate (lastTrecent_.begin(), lastTrecent_.end(), 0.0);
//	// on retire l'activité moyenne pendant le creux, et on moyenne sur les peaks 
//	return (total - NB_STEP/2 * 0.05) / (NB_STEP/2);
//	return * max_element (lastTrecent_.begin(), lastTrecent_.end());
	return lastTrecent_;
}
