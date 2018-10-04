#include <stdlib.h>
#include <iostream>
#include <cmath>
#include "math.hh"
#include "params.hh"

double pow2 (double v)
{
  return v * v;
}

double drand ()
{
	return static_cast<double>(rand ()) / RAND_MAX;	
}

double bruit (double sigma)
{
  return (drand () - 0.5) * sigma;
}

double sigmoid (double x, double a, double b)
{
	return 1.0 / (1.0 + exp (-a * (x + b)));
}

double pi_pi (double angle)
{
	double m = fmod (fmod (angle, 2*PI) + 2*PI, 2*PI);
	m += m > PI ? -2*PI : 0;
	return m;
}

double ecart_angulaire (double reel, double voulu)
{
  double a = fmod (voulu, 2*PI) - fmod (reel, 2*PI);

  if (a > PI)
    return a - 2 * PI;
  else if (a < -PI)
    return a + 2 * PI;
  else
    return a;
}

bool angle_equal (double a1, double a2)
{
	static const double ANGLE_THRESH = Params::get_double("ANGLE_THRESH");
	if (fabs (ecart_angulaire (a1, a2)) < ANGLE_THRESH) {
		return true;
	}
	else {
  		return false;
	}
}

double mean_angles (double a1, double a2)
{
	double diff = ecart_angulaire (a1, a2);
	return a1 + diff/10;
}

double mean_value (double old_v, double new_v, double ratio)
{
	return (1 - ratio) * old_v + ratio * new_v; 
}
