#include "dcell.hh"
#include "math.hh"
#include <cmath>
#include "params.hh"

extern Params* params;

void DCell::compute (double current_angle)
{
	static const double ANGLE_THRESH = params->get_double("ANGLE_THRESH");
	static const double NEURON_SPIKING_THRESH = params->get_double("NEURON_SPIKING_THRESH");
	static const double HYPERPOL_PERIOD = params->get_double ("HYPERPOL_PERIOD");
	static const double SIGMA1 = - pow2 (ANGLE_THRESH) / log (NEURON_SPIKING_THRESH);
	static const double SIGMA2 = SIGMA1;

	if (can_rest_) {
		if (resting_ > 0) {
			resting_--;
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
