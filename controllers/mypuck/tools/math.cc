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
	return fabs (ecart_angulaire (a1, a2)) < ANGLE_THRESH;
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

void color_palette (double activity, int& r, int& g, int& b)
{
/*
 * 0 	0 	255 (blue)
 * 0 	255 255
 * 0 	255 0 (green)
 * 255 	255 0
 * 255 	0 	0 (red)
 * we have 1024 levels
 * here we compress the green zone so that we have
 * only 768 levels

	int level = static_cast<int>(activity * 767);
	if (level < 256) {
		*r = 0;
		*g = level;
		*b = 255;
	}
	else if (256 <= level && level < 512) {
		*r = level - 256;
		*g = 255;
		*b = 511 - level;
	}
	else if (512 <= level && level < 768) {
		*r = 255;
		*g = 767 - level;
		*b = 0;
	}
*/
	if (activity < 0.4) {
		r = 0; 
		g = 255 * 2.5*activity; 
		b = 255;
	}
	else if (0.4 <= activity && activity < 0.6) {
		r = 255 * 5*(activity-0.4); 
		g = 255; 
		b = 255 * (1-5*(activity-0.4));
	}
	else if (activity >= 0.6) {
		r = 255; 
		g = 255 * (1-2.5*(activity-0.6));
		b = 0;
	}
}
