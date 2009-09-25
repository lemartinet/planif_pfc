/**
 * @file neuralnet.hh
 * Spiking rates neural network model interface.
 */

#ifndef NEURALNET_HH_
# define NEURALNET_HH_

#include <vector>
#include "neuron.hh"

/**
 * Spiking rates neural network model.
 * Manage Neurons and connections adding.
 * @note Can be connected with \c double values.
 * @see add_synapse(double& input, Neuron* to, double w)
 */
class Neuralnet
{
public:
  
  /// Neuralnet constructor.
  Neuralnet () : freq_(0) {};
  
  /**
   * Neuralnet destructor.
   * Destruct all created neurons and synaptic connections.
   */
  ~Neuralnet ();
    
  /**
   * Get the \b ith neuron added to the network.
   * @param i index of neuron to get.
   * @return The neuron searched.
   */
  inline
  Neuron* get (int i) { return neurons_[i]; }
  
  /// Number of neurons getter.
  inline
  int     size () { return neurons_.size (); }

  /**
   * Add a synapse between two neurons.
   * @param from Presynaptic neuron.
   * @param to Postsynaptic neuron.
   * @param w Weight of synaptic connection.
   */
  // 2 ajouts = 2 liens
  inline
  void add_synapse (ComputeUnit& from, Neuron& to, double w, bool constw) { to.add_synapse (from, w, constw); }
  
  inline
  void add_synapse (ComputeUnit& from, Neuron& to) { to.add_synapse (from); }
  
  inline
  void del_synapse (ComputeUnit& from, Neuron& to) { to.del_synapse (from); }

  inline
  Synapse* get_synapse (ComputeUnit& from, Neuron& to) { return to.syn_get (from); }
  
  double lateral_learning (Neuron& from, Neuron& to, bool increase);

//   void unset_all ();

  void propagate (int nb);

  /**
   * Update each neuron of the network & compute a learning 
   * step for all neurons of the network
   */
  void synch_learn ();

  /**
   * Update each neuron of the network
   */
  void synch ();

  Neuron* add_neuron (double Vr, double Vf, string& path, int no_col, double ip_step, 
  						double ip_mu, double a, double b, int level);

  Neuron* add_neuron_max (double Vr, double Vf, string& path, int no_col, int level);

  void del_neuron (Neuron* neuron);

  void draw_graph (ostream& os);

  void draw_links (ostream& os);

private:
  vector<Neuron *>  neurons_;  ///< Collection of neurons.
  int               freq_;
};

#endif
