#include "cell.hh"
#include "math.hh"
#include "params.hh"
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>

using namespace std;
using namespace boost::lambda;

Cell::Cell (const Coord& pos) : ComputeUnit(-1), pos_(pos), log_(new Log("cells", no_)) 
{
}

bool sort_function (double* v1, double* v2)
{
	return v1[0] < v2[0] || (v1[0] == v2[0] && v1[1] < v2[1]); 	
}

Cell::Cell (const string& filename, const Coord& pos) : ComputeUnit(-1), pos_(pos), log_(0) 
{
	ifstream file (filename.c_str ());
	double x, y, r;
	while (file >> x >> y >> r) {
		r_.push_back (r);
		posr_.push_back (Coord (x,y));
	}
	file.close ();
	cout << "cell" << no_ << " loaded!" << endl;
//	ostringstream fileout;
//	fileout << Params::get_path () << "cells/" << no_ << ".txt";
//	log_file_ = new ofstream (fileout.str ().c_str ());	
}

Cell::~Cell () 
{
	if (log_) {
		delete log_;
	}
}

void Cell::compute (const Coord& pos)
{
	if (r_.size () == 0) {
		// Noise applied to hippocampus cells activity.
		static const double CELLS_NOISE_SIGMA = Params::get_double("CELLS_NOISE_SIGMA");
		// Ecart-type of hippocampus cells activation.
		static const double CELLS_SIGMA = Params::get_double("CELLS_SIGMA");
		static const double CELL_THRESH = Params::get_double("CELL_THRESH");
	  	double dist = (pos_ - pos).norm ();
	
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
}

void Cell::draw (ostream& os) const
{
	double cell_output = output (); 
    os << "c" << &cell_output << " [label=\"" << no_ << ":" << output () << "\"]" << endl;
}

void Cell::log (const string& time_string, const Coord& position, double angle, int day, int trial) const
{
	if (log_ == 0) {
		return;
	}
	ostringstream msg;
	msg << "# " << time_string << endl;
	msg << position.x_get () << " " << position.y_get () << " " << angle << " " << output () << endl << endl;
	log_->log (day, trial, msg.str ());
}
