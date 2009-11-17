#ifndef SYNAPSE_H
# define SYNAPSE_H

#include "computeunit.hh"
#include <ostream>

using namespace std;

/**
 * Synaptic model for the firing rates neuron's model Neuron.
 */
class Synapse
{
public:
	/**
	 * Build a synapse between two neurons.
	 * @param from Input neuron.
	 * @param w Synaptic weight.
	 */
	Synapse (const ComputeUnit& from, double w) : from_(from), w_(w) {}
	~Synapse () {}

	double* w_get () { return &w_; }
	double drive () const { return w_ * from_.output (); }

private:
	const ComputeUnit& from_;
	double w_;
};

#endif
