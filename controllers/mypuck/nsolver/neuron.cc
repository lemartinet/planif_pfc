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

Neuron::Neuron (bool max, double ip_step, double ip_mu, double a, double b) : 
	max_(max), a_(a), b_(b), ip_step_(ip_step), ip_mu_(ip_mu)
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
	syndrive_ = 0.0;
	thetaM_ = 0.3;
}

Neuron::~Neuron ()
{
	for_each (synapses_.begin (), synapses_.end (), 
		bind (delete_ptr(), bind (&snd, _1)));
}

void Neuron::add_synapse (const ComputeUnit& from, double w, bool constw)
{
	synapses_[from.no_get ()] = new Synapse (from, *this, w, constw);
}

void Neuron::add_synapse (const ComputeUnit& from, const ComputeUnit& from_mult, double w, double a, double b)
{
	synapses_[from.no_get ()] = new Synapse (from, from_mult, *this, w, a, b);
}

void Neuron::add_synapse (const ComputeUnit& from)
{
	// Maximum weight init value.
	static const double NEURON_WEIGHT_INIT  = Params::get_double("NEURON_WEIGHT_INIT");
  	add_synapse (from, drand () * NEURON_WEIGHT_INIT, false);
}

void Neuron::add_synapse_modulation (const ComputeUnit& from, double modulation)
{
	map<const int, Synapse *>::const_iterator iter;
	for (iter = synapses_.begin (); iter != synapses_.end (); iter++) {
		iter->second->from_mult_set (from);
		iter->second->b_set (modulation);
	}
}

double Neuron::compute_sum_wi () const
{
	double res = 0.0;
	map<const int, Synapse *>::const_iterator iter;
	for (iter = synapses_.begin (); iter != synapses_.end (); iter++) {
		res += iter->second->w_get ();	
	}
	return res;
}

void Neuron::update_IP ()
{
   a_ += (1.0 / a_ + pot_ - (2.0 + 1.0 / ip_mu_) * pot_ * output_ + 1.0 / ip_mu_ * pot_ * output_ * output_) / ip_step_;
   b_ += (1.0 - (2.0 + 1.0 / ip_mu_) * output_ + 1.0 / ip_mu_ * output_ * output_) / ip_step_;
}

void Neuron::compute ()
{
	static const double NEURON_TAU = Params::get_double("NEURON_TAU");
	static const double DELTA_T = Params::get_double("DELTA_T");
	static const double NEURON_ACTIVATION_NOISE = Params::get_double("NEURON_ACTIVATION_NOISE");
	double br = bruit (2 * NEURON_ACTIVATION_NOISE);
		
	if (!synapses_.empty ()) {
		syndrive_ = max_ ? syndrive_max () : syndrive_sum ();
		pot_ += DELTA_T * (-pot_ + thresh_ + syndrive_) / NEURON_TAU;

		output_next_ = pot_;
		if (!max_) {
			double sum_wi = compute_sum_wi ();
			output_next_ /= sum_wi;
//			output_next_ = sigmoid (pot_, a_, b_);
		}
    }
	else {
		output_next_ = 0;
	}
	
	// bruit additif
//	output_next_ += br;
	// bruit multiplicatif
	output_next_ *= 1 + br;
	output_next_ = output_next_ < 0.0 ? 0.0 : output_next_;
  	output_next_ = output_next_ > 1.0 ? 1.0 : output_next_;
//  	if (output_next_ < 0.05) {
//		output_next_ = 0.05 + bruit(2 * 0.05);	
//	}
}

double Neuron::syndrive_sum () const
{
	double res = 0.0;
	map<const int, Synapse *>::const_iterator iter;
	for (iter = synapses_.begin (); iter != synapses_.end (); iter++) {
		res += iter->second->drive ();	
	}
	return res;
}

double Neuron::syndrive_max () const
{
	map<const int, Synapse *>::const_iterator it;
	it = max_element (synapses_.begin (), synapses_.end (), 
		bind (&Synapse::drive, bind (&snd, _1)) < bind (&Synapse::drive, bind (&snd, _2)));
	return (it == synapses_.end ()) ? 0 : it->second->drive ();
}

// Pas utilisé !
double Neuron::syndrive_wta () const
{
	double val = 0.0;
	map<const int, Synapse *>::const_iterator iter;
	for (iter = synapses_.begin (); iter != synapses_.end (); iter++) {
		double wij = iter->second->w_get ();
		double rj = iter->second->from_get ().output ();
	  	val += (wij - rj) * (wij - rj);
	}
	return exp(-val/5) ;	
}

void Neuron::learn ()
{
	for_each (synapses_.begin (), synapses_.end (), bind (&Synapse::hlearn, bind (&snd, _1)));
}

Synapse* Neuron::syn_get (const ComputeUnit& from) const
{
	map<const int, Synapse *>::const_iterator iter;
	iter = synapses_.find (from.no_get ());
	return (iter == synapses_.end ()) ? 0 : iter->second;
}

void Neuron::draw_graph (ostream& os) const
{
	os << "n" << no_get () << " [label=\""<< no_get () << ":" << output () << "\"]" << endl;
	map<const int, Synapse *>::const_iterator iter;
	for (iter = synapses_.begin (); iter != synapses_.end (); iter++) {
		Cell* cell = dynamic_cast <Cell *>(&(iter->second->from_get ()));
		if (cell) {
			double cell_output = cell->output ();
			os << "c" << &cell_output << " [label=\"" << cell->no_get () << ":" << &cell_output 
				<< "\", color=grey, style=filled];" << endl
				<< "c" << &cell_output;
		}
		else {
			Neuron* neuron = dynamic_cast <Neuron *> (&(iter->second->from_get ()));
			os << "n" << neuron->no_get ();
		}
		os << " -> n" << no_get () << " [label=\"" << iter->second->w_get () << "\"];" << endl;
	}	
}

void Neuron::print_weights (ostream& os) const
{
  	map<const int, Synapse *>::const_iterator iter;
	for (iter = synapses_.begin (); iter != synapses_.end (); iter++) {
		os << iter->second->w_get () << " ";
	}
}
