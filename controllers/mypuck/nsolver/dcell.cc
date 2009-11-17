#include "dcell.hh"
#include "math.hh"
#include <cmath>
#include "params.hh"

void DCell::compute (double current_angle)
{
	static const double ANGLE_THRESH = Params::get_double("ANGLE_THRESH");
	static const double NEURON_SPIKING_THRESH = Params::get_double("NEURON_SPIKING_THRESH");
	static const double SIGMA1 = - pow2 (ANGLE_THRESH) / log (NEURON_SPIKING_THRESH);

	double diff = ecart_angulaire (angle_, current_angle);
	output_ = exp (-pow2 (diff) / SIGMA1);
}
