/**
 * Math customized toolbox interface.
 * @file math.hh
 */
#ifndef MATH_HH_
# define MATH_HH_

#define PI 3.14159

double pow2 (double v);

/**
 * Noise function.
 * Random values between +- \a sigma.
 * @param sigma Strenght of noise.
 * @return Noise value
 */
double bruit (double sigma);

double ecart_angulaire (double reel, double voulu);

double mean_angles (double a1, double a2);

double mean_value (double old_v, double new_v, double ratio);

#endif