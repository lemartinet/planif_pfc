#include "egoaction.hh"
#include "math.hh"
#include <cmath>
#include "params.hh"

void EgoAction::compute (double orientation, int nb_path)
{
	static const double ANGLE_THRESH = Params::get_double("ANGLE_THRESH");
//	static const double NEURON_SPIKING_THRESH = Params::get_double("NEURON_SPIKING_THRESH");
//	static const double SIGMA1 = - pow2 (ANGLE_THRESH) / log (NEURON_SPIKING_THRESH);
	static const int HYPERPOL_PERIOD = Params::get_int ("HYPERPOL_PERIOD");
	static const int TIME_STEP = Params::get_int ("TIME_STEP");
	
	while (angles_.size () < (500.0 / TIME_STEP)) {
		// on étudie la variation de l'orientation sur 500 ms
		angles_.push (orientation);
	}
	// calcul de l'action ego actuelle
	double diff = ecart_angulaire (angles_.front (), orientation);
	angles_.pop ();
	angles_.push (orientation);

	if (resting_ > 0) {
		resting_--;
	}
	else {
		diff = ecart_angulaire (angle_, diff);
		if (fabs(diff) >= ANGLE_THRESH || nb_path > 1) {
			// on se met en pause pendant HYPERPOL_PERIOD ms
			resting_ = HYPERPOL_PERIOD / TIME_STEP;
			output_ =  0;
		}
		else
			output_ =  1;
	}
}
