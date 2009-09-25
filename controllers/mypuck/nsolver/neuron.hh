
/**
 * @file neuron.hh
 * Simple firing rate's neuron interface.
 * For model's details, see Theoretical Neuroscience (F.Abott, 2006).
 */

#ifndef NEURON_HH_
# define NEURON_HH_

#include <string>
#include <vector>
#include "computeunit.hh"
#include "synapse.hh"

using namespace std;

void update_IP (double& a, double& b, double h, double y, double step, double mu);

/**
 * Firing rate's neuron model class.
 * This is a cross recursive class with Synapse.
 */
class Neuron : public ComputeUnit
{
public:
  /**
   * Neuron's constructor.
   * @param Vr Resting potential of neuron's membrane.
   * @param Vf Threshold potential of neuron's membrane.
   */
  Neuron (double Vr, double Vf, string& path, int no_col, int no, bool max, double ip_step, 
  			double ip_mu, double a, double b, int level);
  
  /// Neuron's destructor.
  virtual ~Neuron ();

  /// Getting tetaV BCM parameter.
//   inline
//   double tetaV_get () { return tetaV_; }

  /// Setting tetaV BCM parameter.
//   inline
//   void   tetaV_set (double tetaV) { tetaV_ = tetaV; }
  
  /// Getting the number of synapses.
  inline
  int    size () { return synapses_.size (); }

  /// Synapse getter.
  inline
  Synapse* syn_get (int i) { return synapses_[i]; }

  bool spiking ();

  inline
  int no_get () { return no_; }

  inline
  int no_col_get () { return no_col_; }

  inline
  string& path_get () { return path_; }

  inline
  void intra_set (double val) { intra_ = val; set_ = true; }

  inline
  void intra_set () { set_ = true; }

  inline
  void intra_unset () { set_ = false; }

  inline
  bool intra_setted () { return set_; }
  
  inline
  double a_get () { return a_; }

  inline
  double b_get () { return b_; }

  inline
  void synch_IP () { update_IP (a_, b_, pot_, output_, ip_step_, ip_mu_); }

  inline
  double pot_get () { return pot_; }

  inline
  double syndrive_get () { return syndrive_; }

  inline
  double sum_wi_get () { return sum_wi_; }

  /**
   * Add a synapse to another neuron, with synaptic weight w.
   * @param from Neuron to connect to.
   * @param w Weight of synapse.
   */
  void   add_synapse (ComputeUnit& from, double w, bool constw);

  void   add_synapse (ComputeUnit& from);

  /**
   * Update the neuron's firing rate
   * @note Doesn't propagate the firing through the network.
   * @note Update membrane's potential.
   */
  void compute ();

  /**
   * Compute the neuron's synaptic drive (from every dentrites).
   * @return Synaptic drive value
   * @note Doesn't affect the neuron.
   */
  double syndrive_sum ();
  double syndrive_max ();
  double syndrive_wta ();

  /**
   * Neuron's learning step 
   */
	void learn ();

  void compute_sum_wi ();

  void del_synapse (ComputeUnit& from);

  Synapse* syn_get (ComputeUnit& from);

  void draw_links (ostream& os);
  
  void write (string filename);

  void compute_syndrive ();

  void compute_pot ();

  void compute_activation ();

  void print (ostream& os);
  
  void boost_weights ();

private:
  int                 no_;
  int                 no_col_;
  string              path_;
  double              thresh_;       ///< Global spiking threshold value.
  double              pot_;          ///< Membrane's current potential.
  //double              tetaV_;        ///< current BCM parameter value.
  double              sum_wi_;
  bool                set_;
  double              intra_;
  bool                max_;
	int inhibed_;
  double              a_;
  double              b_;
  double              syndrive_;
  double              ip_step_;
  double              ip_mu_;
  vector<Synapse *>   synapses_;      ///< Collection of synapses connected to neuron.
};

#endif
