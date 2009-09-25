#ifndef CELL_HH_
#define CELL_HH_

#include "computeunit.hh"
#include "coord.hh"

class Hippo;

/**
 * Model of Hippocampus cells activity
 * Generate activity fields given the true position of the animat.
 * @note Should be replaced by Angelo's cells models.
 */
class Cell : public ComputeUnit
{
 public:

  /// Constructor taking maximum response at coordinate \a pos.
  Cell (Coord& pos, int no, Hippo* hippo) : pos_(pos), no_(no), hippo_(hippo) { level_ = -1; };

  /// Cell destructor.
  virtual ~Cell () {};
 
  /**
   * Compute cell activity from the rat position.
   * @note Update cell activity. 
   */
  void compute ();

  /// Maximum response coordinate getter.
  inline
  Coord&  pos_get () { return pos_; }

  inline
  int     no_get () { return no_; }

 private:
  Coord  pos_;            ///< Cell's receptive field center
  int    no_;
  Hippo* hippo_;
};

#endif /*CELL_HH_*/
