#include "cell.hh"
#include "math.hh"
#include "params.hh"
#include <fstream>
#include <iostream>


Cell::Cell () : pos_(Coord(100,100))
{
	// on a pris un centre de champ recepteur en dehors du maze
}

Cell::Cell (const Coord& pos) : pos_(pos)
{
}

bool sort_function (double* v1, double* v2)
{
	return v1[0] < v2[0] || (v1[0] == v2[0] && v1[1] < v2[1]); 	
}

Cell::Cell (const string& filename, const Coord& pos) : pos_(pos)
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
	if (output_ < 0.05) {
		output_ = 0.05 + bruit(2 * 0.05);	
	}
}

void Cell::draw (ostream& os) const
{
	double cell_output = output (); 
    os << "c" << &cell_output << " [label=\"" << no_ << ":" << output () << "\"]" << endl;
}
