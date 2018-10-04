/**
 * @file neuralnet.hh
 * Spiking rates neural network model interface.
 */

#ifndef NEURALNET_HH_
# define NEURALNET_HH_

#include <string>
#include <vector>

using namespace std;

class Neuron;

/**
 * Spiking rates neural network model.
 */
class Neuralnet
{
public:
	Neuralnet () : freq_(0) {};
	~Neuralnet ();
	    
	Neuron* get (int i) const { return neurons_[i]; }
	int size () const { return neurons_.size (); }

	double lateral_learning (Neuron& from, Neuron& to, bool increase);
	/**
	 * Update each neuron of the network & compute a learning 
	 * step for all neurons of the network
	 */
	void synch_learn ();
	/**
	 * Update each neuron of the network
	 */
	void synch ();

	Neuron& add_neuron (double ip_step, double ip_mu, double a, double b);
	Neuron& add_neuron_max ();
	
	void draw_graph (ostream& os) const;

private:
	vector<Neuron *> neurons_;
	int freq_;
};

#endif
