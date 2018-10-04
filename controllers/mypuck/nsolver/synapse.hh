#ifndef SYNAPSE_H
# define SYNAPSE_H

#include <ostream>
#include "computeunit.hh"

using namespace std;

/**
 * Synaptic model for the firing rates neuron's model Neuron.
 * This is a cross recursive class with Neuron.
 * @note One neuron can be connected with others and with \c double values.
 * @see Neuron
 */
class Synapse
{
public:
	/**
	 * Build a synapse between two neurons.
	 * @param from Input neuron.
	 * @param to Output neuron.
	 * @param w Synaptic weight.
	 * @param constw constant synapse (no learning)
	 */
	Synapse (const ComputeUnit& from, const ComputeUnit& to, double w, bool constw);
	
	// Synapse multiplicatrice
	Synapse (const ComputeUnit& from, const ComputeUnit& from_mult, const ComputeUnit& to, 
		double w, double a, double b);

	~Synapse ();

	double w_get () const { return w_; }
	void w_set (double v) { w_ = v; }
	void b_set (double b) { b_ = b; }
	ComputeUnit& to_get () const { return const_cast<ComputeUnit&>(to_); }
	ComputeUnit& from_get () const { return const_cast<ComputeUnit&>(from_); }
	void from_mult_set (const ComputeUnit& from_mult) { from_mult_ = &from_mult; }
	
	double drive () const;

	/**
	 * Synapse hebbian learning.
	 * @note Update only one synapse, using current neurons potential values.
	 */
	void hlearn ();

	void draw_links (ostream& os) const;

private:
	/**
	 * Learning using a BCM rule.
	 */
	void BCM ();

private:
	const ComputeUnit& from_;
	const ComputeUnit* from_mult_;
	const ComputeUnit& to_;
	double w_;
	const bool constw_;
	const double a_;
	double b_;
};

#endif
