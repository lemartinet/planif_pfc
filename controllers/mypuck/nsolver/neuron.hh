#ifndef NEURON_HH_
# define NEURON_HH_

#include <string>
#include <map>
#include "computeunit.hh"
#include "synapse.hh"

using namespace std;
class Coord;

/**
 * Firing rate's neuron model class.
 * This is a cross recursive class with Synapse.
 */
class Neuron : public ComputeUnit
{
public:
	Neuron (const string& path, int no_col, bool max, double ip_step, double ip_mu, double a, double b, int level);
	virtual ~Neuron ();
  
	/// Getting the number of synapses.
	int size () const { return synapses_.size (); }

	/// Synapse getter.
	Synapse* syn_get (const ComputeUnit& from) const;

	int no_col_get () const { return no_col_; }
	const string& path_get () const { return path_; }
	double a_get () const { return a_; }
	double b_get () const { return b_; }
	double pot_get () const { return pot_; }
	double syndrive_get () const { return syndrive_; }

	/**
	 * Add & del a synapse from another neurons, with synaptic weight w.
	 */
	void add_synapse (const ComputeUnit& from, double w, bool constw);
	void add_synapse (const ComputeUnit& from, const ComputeUnit& from_mult, double w,  double a, double b);
	void add_synapse (const ComputeUnit& from);
	void add_synapse_modulation (const ComputeUnit& from, double modulation);

	/**
	 * Set of functions to compute next firing rate and update the output
	 * compute () has to be called first by all neurons
	 */
	void compute ();
	void update () { output_ = output_next_; }
	double syndrive_sum () const;
	double syndrive_max () const;
	double syndrive_wta () const;
	double compute_sum_wi () const;

	/**
	 * Neuron's learning step 
	 */
	void learn ();

	void draw_links (ostream& os) const;
	void draw_graph (ostream& os) const;
	void print_weights (ostream& os) const;
	void center_rf (Coord& moy) const;
	
private:
	void update_IP ();
  
private:
	const int no_col_;
	string path_;
	double thresh_; ///< Global spiking threshold value.
	double pot_; ///< Membrane's current potential.
//	double tetaV_; ///< current BCM parameter value.
	const bool max_;
	double a_;
	double b_;
	double syndrive_;
	const double ip_step_;
	const double ip_mu_;
	map<const int, Synapse *>   synapses_; ///< Collection of synapses connected to neuron.
	double output_next_;
};

#endif
