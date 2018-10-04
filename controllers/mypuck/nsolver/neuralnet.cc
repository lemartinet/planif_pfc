#include "neuralnet.hh"
#include "hippo.hh"
#include "cell.hh"
#include "params.hh"
#include "neuron.hh"
#include <iostream>
#include <assert.h>
#include <algorithm>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>

using namespace boost::lambda;

Neuralnet::~Neuralnet ()
{
	for_each (neurons_.begin (), neurons_.end (), bind (delete_ptr(), _1));
}

double Neuralnet::lateral_learning (Neuron& from, Neuron& to, bool increase)
{
	static const double LATERAL_LEARNING_STEP = Params::get_double("LATERAL_LEARNING_STEP");
	static const double MAX_LATERAL_WEIGHT = Params::get_double("MAX_LATERAL_WEIGHT");
	
	// on apprend le lien en un coup (valeur à MAX_LATERAL_WEIGHT) 
	// mais on desapprend 4 fois moins vite
	
	Synapse* s = to.syn_get (from);
	if (!s) {
		// on recrute une synapse
		to.add_synapse (from, MAX_LATERAL_WEIGHT, true);
		return MAX_LATERAL_WEIGHT;
	}
	else {
		// on modifie la synapse
		double val = increase ? MAX_LATERAL_WEIGHT : (s->w_get () * -LATERAL_LEARNING_STEP);
		//cout << "old " << s->w_get ();
		double new_w = s->w_get () + val;
		if (new_w > MAX_LATERAL_WEIGHT) {
			new_w = MAX_LATERAL_WEIGHT;
		}
		else if (new_w < 0) {
			new_w = 0;	
		}
		s->w_set (new_w);
		//cout << " new " << s->w_get () << endl;
		return new_w;
	}
}

void Neuralnet::synch ()
{
	for_each (neurons_.begin (), neurons_.end (), bind (&Neuron::compute, _1));
	for_each (neurons_.begin (), neurons_.end (), bind (&Neuron::update, _1));
}

void Neuralnet::synch_learn ()
{
	static const int TETA_RYTHM = Params::get_int("TETA_RYTHM");
	
	synch ();
	if (!(freq_ % TETA_RYTHM)) {
		for_each (neurons_.begin (), neurons_.end (), bind (&Neuron::learn, _1));
	}
	freq_++;
}

Neuron& Neuralnet::add_neuron (int no_col, double ip_step, 
								double ip_mu, double a, double b, int level)
{
  Neuron* res = new Neuron (no_col, false, ip_step, ip_mu, a, b, level);
  neurons_.push_back (res);
  return *res;
}

Neuron& Neuralnet::add_neuron_max (int no_col, int level)
{
  Neuron* res = new Neuron (no_col, true, 0.0, 0.0, 0.0, 0.0, level);
  neurons_.push_back (res);
  return *res;
}

void Neuralnet::draw_graph (ostream& os) const
{
	os << "digraph G {" << endl;
	for_each (neurons_.begin (), neurons_.end (), bind (&Neuron::draw_graph, _1, var (os)));
	os << "}" << endl;
}
