#include "cell.hh"
#include "math.hh"
#include "params.hh"
#include <ostream>

extern Params* params;

void Cell::compute (const Coord& pos)
{
	// Noise applied to hippocampus cells activity.
	static const double CELLS_NOISE_SIGMA = params->get_double("CELLS_NOISE_SIGMA");
	// Ecart-type of hippocampus cells activation.
	static const double CELLS_SIGMA = params->get_double("CELLS_SIGMA");
	static const double CELL_THRESH = params->get_double("CELL_THRESH");
  	double dist = (pos_ - pos).norm ();

	//output_ = exp (-pow2 (dist) / (2 * CELLS_SIGMA * CELLS_SIGMA)) + bruit (2 * CELLS_NOISE_SIGMA);
	output_ = exp (-pow2 (dist) / (2 * CELLS_SIGMA * CELLS_SIGMA)) * (1 + bruit (2 * CELLS_NOISE_SIGMA));
	output_ = output_ < 0.0 ? 0.0 : output_;
	output_ = output_ > 1.0 ? 1.0 : output_;
	output_ = output_ > CELL_THRESH ? output_ : 0.0;
}

void Cell::draw (ostream& os) const
{
	double cell_output = output (); 
    os << "c" << &cell_output << " [label=\"" << no_ << ":" << output () << "\"]" << endl;
}
