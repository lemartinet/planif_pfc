#ifndef NEURON_HH_
# define NEURON_HH_

#include <map>
#include "computeunit.hh"
#include "synapse.hh"

class Coord;

using namespace std;

/**
 * Firing rate's neuron model class.
 */
class Neuron : public ComputeUnit
{
public:
	Neuron (nType type, bool max = true);
	virtual ~Neuron ();
  
	// Getting the number of synapses
	int size () const { return synapses_.size (); }
	double* syn_get (const ComputeUnit& from) const;
	double* syn_get (int from_no) const;
	double* syn_mod_get() const { return synapse_mod_ == 0 ? 0 : synapse_mod_->w_get(); }
	double* max_syn_get () const;
	map<const int, double*> all_syn_get () const;
	map<const int, double*> all_syn_getI () const;
	double pot_get () const { return pot_; }
	double thetaM_get () const { return thetaM_; }
	void thetaM_set (double thetaM) { thetaM_ = thetaM; }
	double sum_wE_get() const { return sum_wi(synapses_); } 

	// Add a synapse from another neuron, with synaptic weight w.
	double* add_synapse (const ComputeUnit& from, double w);
	double* add_synapse_mult (const ComputeUnit& from, double w);
	double* add_synapse_modulation (const ComputeUnit& from, double w);

	// Set of functions to compute next firing rate and update the output
	// compute () has to be called first by all neurons
	void compute ();
	void update () { output_ = output_next_; }
	
private:
	double syndrive_sum (const map<const int, Synapse*>& syn) const;
	double syndrive_max (const map<const int, Synapse*>& syn) const;
	double syndrive_wta (const map<const int, Synapse*>& syn) const;	
	double sum_wi (const map<const int, Synapse*>& syn) const;
	double sigmoid(double x, double phi) const; 
	
private:
	double thresh_; ///< Global spiking threshold value.
	double pot_; ///< Membrane's current potential.
	const bool max_;
	map<const int, Synapse*>   synapses_;
	map<const int, Synapse*>   synapsesI_;
	Synapse* synapse_mod_;
	Synapse* synapse_mult_; 
	double output_next_;
	double thetaM_; // BCM threshold
};

#endif
