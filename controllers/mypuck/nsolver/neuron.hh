#ifndef NEURON_HH_
# define NEURON_HH_

#include <map>
#include "computeunit.hh"
#include "synapse.hh"

class Coord;

using namespace std;

/**
 * Firing rate's neuron model class.
 * This is a cross recursive class with Synapse.
 */
class Neuron : public ComputeUnit
{
public:
	Neuron (nType type, bool max = true, double ip_step = 0, double ip_mu = 0, double a = 0, double b = 0);
	virtual ~Neuron ();
  
	// Getting the number of synapses
	int size () const { return synapses_.size (); }
	Synapse* syn_get (const ComputeUnit& from) const;
	Synapse* syn_get (int from_no) const;
	Synapse* max_syn_get () const;
	const map<const int, Synapse *>& all_syn_get () const { return synapses_; }
	const map<const int, Synapse *>& all_syn_getI () const { return synapsesI_; }
	double a_get () const { return a_; }
	double b_get () const { return b_; }
	double pot_get () const { return pot_; }
	double syndrive_get () const { return syndrive_; }
	double thetaM_get () const { return thetaM_; }
	void thetaM_set (double thetaM) { thetaM_ = thetaM; }

	/**
	 * Add & del a synapse from another neurons, with synaptic weight w.
	 */
	Synapse* add_synapse (const ComputeUnit& from, double w, bool constw);
	Synapse* add_synapse (const ComputeUnit& from, const ComputeUnit& from_mult, double w,  double a, double b);
	Synapse* add_synapse (const ComputeUnit& from);
	void add_synapse_modulation (const ComputeUnit& from, double modulation);

	/**
	 * Set of functions to compute next firing rate and update the output
	 * compute () has to be called first by all neurons
	 */
	void compute ();
	void update () { output_ = output_next_; }
	double syndrive_sum (const map<const int, Synapse *>& syn) const;
	double syndrive_max () const;
	double syndrive_wta () const;
	double compute_sum_wi () const;

	void draw_graph (ostream& os) const;
	void print_weights (ostream& os) const;
	
private:
	void update_IP ();
  
private:
	double thresh_; ///< Global spiking threshold value.
	double pot_; ///< Membrane's current potential.
	const bool max_;
	double a_;
	double b_;
	double syndrive_;
	const double ip_step_;
	const double ip_mu_;
	map<const int, Synapse *>   synapses_;
	map<const int, Synapse *>   synapsesI_; 
	double output_next_;
	double thetaM_; // BCM threshold
};

#endif
