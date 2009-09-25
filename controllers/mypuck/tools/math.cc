#include <stdlib.h>
#include <iostream>
#include <cmath>
#include "math.hh"

double pow2 (double v)
{
  return v * v;
}

double bruit (double sigma)
{
  return (drand48 () - 0.5) * sigma;
}

double ecart_angulaire (double reel, double voulu)
{
  double a = voulu - reel;

  if (a > PI)
    return a - 2 * PI;
  else if (a < -PI)
    return a + 2 * PI;
  else
    return a;
}

double mean_angles (double a1, double a2)
{
	double diff = ecart_angulaire (a2, a1);
//	return a1 - diff/2;	
	return a1 - diff/10;
}

double mean_value (double old_v, double new_v, double ratio)
{
	return (1 - ratio) * old_v + ratio * new_v; 
}
