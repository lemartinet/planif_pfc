#ifndef SYNAPSE_H
# define SYNAPSE_H

#include <ostream>
#include "computeunit.hh"

using namespace std;

/**
 * BCM rule.
 * @param wij 
 * The weight between presynaptic neuron \a rj axnd postsynaptic neuron \a ri.
 * @param tetaV A parameterx specific of the postsynaptic neuron.
 * @param rj Presynaptic value.
 * @param ri Postsynaptic value.
 */
void BCM (double& wij, double rj, double ri, double& tetaV);

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
   * Synapse constructor.
   * Build a synapse between two neurons.
   * @param from Input neuron.
   * @param to Output neuron.
   * @param w Synaptic weight.
   * @param constw constant synapse (no learning)
   */
  //Synapse (Neuron& from, Neuron& to, double w, bool constw) : from_(from), val_(w_), to_(to), w_(w), input_(false), constw_(constw) {};
  Synapse (ComputeUnit & from, ComputeUnit& to, double w, bool constw) : from_(&from), to_(&to), w_(w), constw_(constw) {};

  /// Synapse destructor.
  // TODO: doit supprimer la synapse des neurones pre et post
  ~Synapse () {};

  /// Synaptic weight getter.
  inline
  double w_get () { return w_; }

  inline
  void w_set (double v) { w_ = v; }

  inline
  double& w_ref_get () { return w_; }

  /// Potentials difference of synapse connection.
  inline
  double drive () { return from_->output () * w_; }

  inline
  ComputeUnit& to_get () { return *to_; }

  inline
  ComputeUnit& from_get () { return *from_; }

  /**
   * Synapse hebbian learning.
   * @note Update only one synapse, using current neurons potential values.
   */
  void   hlearn ();

  void draw_links (ostream& os);

private:
  ComputeUnit*  from_;
  ComputeUnit*  to_;
  double        w_;     ///< Synaptic weight.
  bool          constw_;
};

#endif
