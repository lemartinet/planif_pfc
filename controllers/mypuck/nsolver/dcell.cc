#include "dcell.hh"
#include "math.hh"
#include <cmath>
#include "params.hh"
#include <iostream>

using namespace std;

void DCell::compute (double current_angle)
{
	static const double ANGLE_THRESH = Params::get_double("ANGLE_THRESH");
	static const double NEURON_SPIKING_THRESH = Params::get_double("NEURON_SPIKING_THRESH");
	static const int HYPERPOL_PERIOD = Params::get_int ("HYPERPOL_PERIOD");
	static const double SIGMA1 = - pow2 (ANGLE_THRESH) / log (NEURON_SPIKING_THRESH);
	static const double SIGMA2 = SIGMA1;

	if (can_rest_) {
		if (resting_ > 0) {
			resting_--;
//			cout << "Resting:" << resting_ << endl;
		}
		else {
			double diff = ecart_angulaire (angle_, current_angle);
			output_ = exp (-pow2 (diff) / SIGMA1);
	//		output_ = (fabs(diff) < ANGLE_THRESH)? 0.95 + bruit(0.1) : 0.05 + bruit (0.1);
			if (! spiking ()) {
				resting_ = HYPERPOL_PERIOD;	
			}
		}
	}
	else {
		double diff = ecart_angulaire (angle_, current_angle);
		output_ = exp (-pow2 (diff) / SIGMA2);
	}
}
