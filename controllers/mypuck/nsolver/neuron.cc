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

Neuron::Neuron (nType type, bool max): ComputeUnit(type, 0), pot_(0),
		max_(max), synapse_mult_(0), output_next_(0), thetaM_(0.3)
{
}

Neuron::~Neuron ()
{
	for_each (synapses_.begin (), synapses_.end (), 
		bind (delete_ptr(), bind (&snd, _1)));
}

double* Neuron::add_synapse (const ComputeUnit& from, double w)
{
	synapses_[from.no_get ()] = new Synapse (from, w);
	return synapses_[from.no_get ()]->w_get();
}

double* Neuron::add_synapse_mult (const ComputeUnit& from, double w)
{
	if(synapse_mult_ != 0)
		cout << "synapse déjà allouée" << endl;
	synapse_mult_ = new Synapse(from, w);
	return synapse_mult_->w_get();
}

double Neuron::sum_wi (const map<const int, Synapse *>& syn) const
{
	double res = 0.0;
	map<const int, Synapse *>::const_iterator iter;
	for (iter = syn.begin (); iter != syn.end (); ++iter) {
		res += *iter->second->w_get ();	
	}
	return res;
}

double Neuron::sigmoid(double x) const
{
	double a = 10, b = 0.3;
	return 1 / (1 + exp(-a * (x - b)));
}

void Neuron::compute ()
{
	static const double NEURON_TAU = Params::get_double("NEURON_TAU");
	static const double DELTA_T = Params::get_double("DELTA_T");
	static const double NEURON_NOISE = Params::get_double("NEURON_NOISE");
	static const double NEURON_THRESH = Params::get_double("NEURON_THRESH");

	double syndrive = 0;
	if (max_) {
		syndrive = syndrive_max (synapses_) + syndrive_max (synapsesI_);
	}
	else {
		syndrive = syndrive_sum (synapses_) + syndrive_sum (synapsesI_);
	}
	if (synapse_mult_ != 0)
		syndrive *= synapse_mult_->drive();
	pot_ += DELTA_T * (-pot_ + NEURON_THRESH + syndrive) / NEURON_TAU;
	output_next_ = pot_ * (1 + bruit (2 * NEURON_NOISE));
//	output_next_ = sigmoid(output_next_);
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
	for (iter = syn.begin (); iter != syn.end (); ++iter) {
		res += iter->second->drive ();	
	}
	return res / sum_wi (syn);
}

double Neuron::syndrive_max (const map<const int, Synapse *>& syn) const
{
	double max = 0;
	map<const int, Synapse *>::const_iterator it = syn.begin();
	for (; it != syn.end(); ++it) {
		const double dr = it->second->drive();
		if (dr > max) {
			max = dr;
		}
	}
	return max;
}

double* Neuron::syn_get (const ComputeUnit& from) const
{
	return syn_get (from.no_get ());
}

double* Neuron::syn_get (int from_no) const
{
	map<const int, Synapse *>::const_iterator iter;
	iter = synapses_.find (from_no);
	return (iter == synapses_.end ()) ? 0 : iter->second->w_get();	
}

double* Neuron::max_syn_get () const
{
	map<const int, Synapse *>::const_iterator iter;
	double max_w = 0;
	Synapse* max_s = 0;
	for (iter = synapses_.begin (); iter != synapses_.end (); ++iter) {
		if (max_w < *iter->second->w_get ()) {
			max_w = *iter->second->w_get ();
			max_s = iter->second;
		}
	}
	return max_s == 0 ? 0 : max_s->w_get();
}

map<const int, double*> Neuron::all_syn_get () const 
{
	map<const int, Synapse*>::const_iterator iter;
	map<const int, double*> res;
	for (iter = synapses_.begin (); iter != synapses_.end (); ++iter) {
		res[iter->first] = iter->second->w_get ();
	}
	return res; 
}

map<const int, double*> Neuron::all_syn_getI () const 
{ 
	map<const int, Synapse*>::const_iterator iter;
	map<const int, double*> res;
	for (iter = synapsesI_.begin (); iter != synapsesI_.end (); ++iter) {
		res[iter->first] = iter->second->w_get ();
	}
	return res; 
}
