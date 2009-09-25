#include "dcell.hh"
#include "math.hh"
#include <cmath>

void DCell::compute (double current_angle)
{
	double diff = ecart_angulaire (angle_, current_angle);
	output_ = (fabs(diff) < PI/4)? 0.9 + bruit(0.1) : 0.1 + bruit (0.1);
}
