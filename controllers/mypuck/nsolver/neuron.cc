#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include "neuron.hh"
#include "math.hh"
#include "params.hh"

extern Params* params;
double IP_STEP = 200.0;
double IP_MU = 0.9;

Neuron::Neuron (double Vr, double Vf, string& path, int no_col, int no, bool max, double ip_step, 
				double ip_mu, double a, double b, int level)
{
	// Noise on resting potential.
	static const double EPSILON_VR = params->get_double("EPSILON_VR"); 
	// Noise on firing potential.
	static const double EPSILON_VF = params->get_double("EPSILON_VF");
  	stringstream ss;
  	string       val;

	set_ = false;
	ss << no;
	ss >> val;
	path_ = path;
	path_ += "n" + val;
	no_col_ = no_col;
	no_ = no;
	Vr += bruit (EPSILON_VR);
	Vf += bruit (EPSILON_VF);
	thresh_ = Vr - Vf;
	pot_ = 0.0;
	output_ = 0.0;
	//tetaV_ = 0.5;
	sum_wi_ = 0.0;
	intra_ = 0.0;
	max_ = max;
	inhibed_ = 0;
	syndrive_ = 0.0;
	a_ = a;
	b_ = b;
	ip_step_ = ip_step;
	ip_mu_ = ip_mu;
	level_ = level;
}

Neuron::~Neuron ()
{
	int size = synapses_.size ();

	for (int i = 0; i < size; i++) {
		delete synapses_[i];
	}
}

bool Neuron::spiking () 
{
	static const double NEURON_SPIKING_THRESH = params->get_double("NEURON_SPIKING_THRESH");
	return output_ > NEURON_SPIKING_THRESH; 
}

void   Neuron::add_synapse (ComputeUnit& from, double w, bool constw)
{
	synapses_.push_back (new Synapse (from, *this, w, constw));
}

void   Neuron::add_synapse (ComputeUnit& from)
{
	// Maximum weight init value.
	static const double NEURON_WEIGHT_INIT  = params->get_double("NEURON_WEIGHT_INIT");
  	add_synapse (from, drand48 () * NEURON_WEIGHT_INIT, false);
}

void Neuron::compute_sum_wi ()
{
	int    size = synapses_.size ();
	double res = 0.0;
	
	for (int i = 0; i < size; i++) {
		res += synapses_[i]->w_get ();
	}
	sum_wi_ = res;
}

void update_IP (double& a, double& b, double h, double y, double step, double mu)
{
   a += (1.0 / a + h - (2.0 + 1.0 / mu) * h * y + 1.0 / mu * h * y * y) / step;
   b += (1.0 - (2.0 + 1.0 / mu) * y + 1.0 / mu * y * y) / step;
}

double sigmoid (double val)
{
  return 1.0 / (1.0 + 200.0 * exp (-50.0 * val));
}

double norm_weights (double pot, double sum)
{
  return pot / (sum);
}

double sigmoid_param (double h, double a, double b, double sum)
{
//   if (h > 0.05)
//     return 1.0 / (1.0 + exp (-0.8 * (h + b)))-0.1;
//   else
//     return 0.0;
  //cout << "a:" << a << " b:" << b << endl;
  //cout << 1.0 / (1.0 + exp (-a * (h + b))) << endl;
  //return h;
  return 1.0 / (1.0 + exp (-a * (h + b)));
  //return h / 10.0;
  //return norm_weights (h, sum);
}

void Neuron::compute ()
{
	if (set_) {
    	output_ = intra_;
	}
    else if (!synapses_.empty ()) {
		syndrive_ = max_ ? syndrive_max () : syndrive_sum ();
		compute_pot ();
		compute_sum_wi ();
		compute_activation ();
    }
	else {
		output_ = 0;
	}
	output_ = output_ < 0.0 ? 0.0 : output_;
  	output_ = output_ > 1.0 ? 1.0 : output_;
}


void Neuron::compute_syndrive ()
{
  syndrive_ = max_ ? syndrive_max () : syndrive_sum ();
}

void Neuron::compute_pot ()
{
	static const double NEURON_SPIKING_RATE = params->get_double("NEURON_SPIKING_RATE");
	static const double DELTA_T = params->get_double("DELTA_T");
	static const double HYPERPOL_PERIOD = params->get_double("HYPERPOL_PERIOD");
 	
	if (inhibed_ > 0) {
		// on ne remet pas à jour le potentiel
		inhibed_--;
	}  	
	else {
		pot_ += DELTA_T * (-pot_ + thresh_ + syndrive_) / NEURON_SPIKING_RATE;
		if (pot_ < 0) {
			// si le potentiel devient negatif, on cree une 
			// periode d'hyperpolarisation prolongee
			inhibed_ = HYPERPOL_PERIOD;
		}	
	}
}

void Neuron::compute_activation ()
{
	static const double POST_ACTIV_THRESH = params->get_double("POST_ACTIV_THRESH");
	static const double NEURON_ACTIVATION_NOISE = params->get_double("NEURON_ACTIVATION_NOISE");
	
	double br = bruit (2 * NEURON_ACTIVATION_NOISE);
//  	output_ = max_ ? (pot_ + br) : sigmoid_param (pot_ + br, a_, b_, sum_wi_);
//  	output_ += bruit (2 * NEURON_ACTIVATION_NOISE);

	//output_ = max_ ? (pot_ + br) : norm_weights (pot_ + br, sum_wi_);
	output_ = max_ ? pot_ : norm_weights (pot_, sum_wi_);
	// bruit multiplicatif
	output_ *= 1 + br; 
  	output_ = output_ > POST_ACTIV_THRESH ? output_ : 0.0;
}

double Neuron::syndrive_sum ()
{
  int    size = synapses_.size ();
  double res = 0.0;

  for (int i = 0; i < size; i++)
    res += synapses_[i]->drive ();

  return res;
}

double Neuron::syndrive_max ()
{
  double max = 0.0;
  double val = 0.0;
  int    size = synapses_.size ();

  for (int i = 0; i < size; i++)
    {
      val = synapses_[i]->drive ();
      if (val > max) {
	  	max = val;
      }
      // TODO : on a un neurone max inhibitable
      else if (val < 0) {
      	return val;
      }
    }
  return max;
}

double Neuron::syndrive_wta ()
{
  double max = 0.0;
  double val = 0.0;
  int    size = synapses_.size ();

  for (int i = 0; i < size; i++) {
  	val += (synapses_[i]->w_get () - synapses_[i]-> from_get ().output ())
  			*(synapses_[i]->w_get () - synapses_[i]-> from_get ().output ());
  }
  printf("%f\n", val);
  return exp(-val/5) ;	
}

void Neuron::learn ()
{
	int size = synapses_.size ();
	for (int i = 0; i < size; i++) {
		synapses_[i]->hlearn ();
	}
}

void Neuron::del_synapse (ComputeUnit& input)
{
  vector<Synapse *>::iterator it;
  Synapse*                    syn = 0;

  syn = syn_get (input);
  if (!syn) {
  	printf("pas de synapse\n");
    return;
  }
  it = find (synapses_.begin (), synapses_.end (), syn);
  delete *it;
  synapses_.erase (it);
}

Synapse* Neuron::syn_get (ComputeUnit& from)
{
  int  nbsyn = size ();

  for (int i = 0; i < nbsyn; i++)
    {
      if (synapses_[i]->from_get () == from)
	return synapses_[i];
    }
  return 0;
}

void Neuron::draw_links (ostream& os)
{
  int size = synapses_.size ();

  for (int i = 0; i < size; i++)
    synapses_[i]->draw_links (os);
}

void Neuron::write (string filename)
{
  ofstream  file (filename.c_str ());
  int       size = synapses_.size ();

  for (int i = 0; i < size; i++)
    file << synapses_[i]->w_get () << endl;
}

void Neuron::print (ostream& os)
{
  int size2 = size ();

  os << output_;
  os << " ";
  for (int i = 0; i < size2; i++)
    os << " " << synapses_[i]->w_get ();
  os << endl;
}

void Neuron::boost_weights ()
{
	// TODO: virer ce boost, a remplacer par de l'apprentissage
	// on pourrait forcer a avoir pendant X.dt la colonne nouvelle creee qui apprend
	// des connections
	int size = synapses_.size ();
  	int i = 100;
	while (output () < 0.75 && i > 0 ) {
		i--;
		for (int i = 0; i < size; i++) {
			if (synapses_[i]->w_get () > 0) {
				synapses_[i]->w_set (synapses_[i]->w_get () + 0.1);
			}
  		}
	}
	pot_ = 0;
}
