#include <assert.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include "neuron.hh"
#include "math.hh"
#include "params.hh"
#include "cell.hh"

using namespace boost::lambda;

Synapse* snd (const pair<const int, Synapse *>& p)
{
	return p.second;	
}

Neuron::Neuron (nType type, bool max): ComputeUnit(type), max_(max)
{
	static const double EPSILON_VR = Params::get_double("EPSILON_VR"); 
	static const double EPSILON_VF = Params::get_double("EPSILON_VF");
	static const double VR = Params::get_double("VR");
  	static const double VF = Params::get_double("VF");
	double Vr = VR + bruit (EPSILON_VR);
	double Vf = VF + bruit (EPSILON_VF);
  	stringstream ss;
  	string       val;

	ss << no_;
	ss >> val;
	thresh_ = Vr - Vf;
	pot_ = 0.0;
	output_ = output_next_ = 0.0;
	thetaM_ = 0.3;
}

Neuron::~Neuron ()
{
	for_each (synapses_.begin (), synapses_.end (), 
		bind (delete_ptr(), bind (&snd, _1)));
}

Synapse* Neuron::add_synapse (const ComputeUnit& from, double w, bool constw)
{
	synapses_[from.no_get ()] = new Synapse (from, *this, w, constw);
	return synapses_[from.no_get ()];
}

Synapse* Neuron::add_synapse (const ComputeUnit& from, const ComputeUnit& from_mult, double w, double a, double b)
{
	synapses_[from.no_get ()] = new Synapse (from, from_mult, *this, w, a, b);
	return synapses_[from.no_get ()];
}

void Neuron::add_synapse_modulation (const ComputeUnit& from, double modulation)
{
	map<const int, Synapse *>::const_iterator iter;
	for (iter = synapses_.begin (); iter != synapses_.end (); iter++) {
		iter->second->from_mult_set (from);
		iter->second->b_set (modulation);
	}
}

double Neuron::sum_wi (const map<const int, Synapse *>& syn) const
{
	double res = 0.0;
	map<const int, Synapse *>::const_iterator iter;
	for (iter = syn.begin (); iter != syn.end (); iter++) {
		res += iter->second->w_get ();	
	}
	return res;
}

void Neuron::compute ()
{
	static const double NEURON_TAU = Params::get_double("NEURON_TAU");
	static const double DELTA_T = Params::get_double("DELTA_T");
	static const double NEURON_ACTIVATION_NOISE = Params::get_double("NEURON_ACTIVATION_NOISE");
	double br = bruit (2 * NEURON_ACTIVATION_NOISE);
	
	double syndrive = 0;
	if (max_) {
		syndrive = syndrive_max (synapses_) + syndrive_max (synapsesI_);
	}
	else {
		syndrive = syndrive_sum (synapses_) + syndrive_sum (synapsesI_);
	}
	pot_ += DELTA_T * (-pot_ + thresh_ + syndrive) / NEURON_TAU;
	output_next_ = pot_ * (1 + br);
	output_next_ = output_next_ < 0.0 ? 0.0 : output_next_;
  	output_next_ = output_next_ > 1.0 ? 1.0 : output_next_;
	if (output_next_ < 0.05) {
		output_next_ = 0.05 + bruit(2 * 0.05);	
	}
}

double Neuron::syndrive_sum (const map<const int, Synapse *>& syn) const
{
	double res = 0.0;
	map<const int, Synapse *>::const_iterator iter;
	for (iter = syn.begin (); iter != syn.end (); iter++) {
		res += iter->second->drive ();	
	}
	return res / sum_wi (syn);
}

double Neuron::syndrive_max (const map<const int, Synapse *>& syn) const
{
	map<const int, Synapse *>::const_iterator it;
	it = max_element (syn.begin (), syn.end (), 
		bind (&Synapse::drive, bind (&snd, _1)) < bind (&Synapse::drive, bind (&snd, _2)));
	return (it == syn.end ()) ? 0 : it->second->drive ();
}

// Pas utilisé !
double Neuron::syndrive_wta (const map<const int, Synapse *>& syn) const
{
	double val = 0.0;
	map<const int, Synapse *>::const_iterator iter;
	for (iter = syn.begin (); iter != syn.end (); iter++) {
		double wij = iter->second->w_get ();
		double rj = iter->second->from_get ().output ();
	  	val += (wij - rj) * (wij - rj);
	}
	return exp(-val/5) ;	
}

Synapse* Neuron::syn_get (const ComputeUnit& from) const
{
	return syn_get (from.no_get ());
}

Synapse* Neuron::syn_get (int from_no) const
{
	map<const int, Synapse *>::const_iterator iter;
	iter = synapses_.find (from_no);
	return (iter == synapses_.end ()) ? 0 : iter->second;	
}

Synapse* Neuron::max_syn_get () const
{
	map<const int, Synapse *>::const_iterator iter;
	double max_w = 0;
	Synapse* max_s = 0;
	for (iter = synapses_.begin (); iter != synapses_.end (); iter++) {
		if (max_w < iter->second->w_get ()) {
			max_w = iter->second->w_get ();
			max_s = iter->second;
		}
	}
	return max_s;
}
