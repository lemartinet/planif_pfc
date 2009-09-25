#ifndef HIPPO_HH_
#define HIPPO_HH_

#include <QObject>
#include <vector>
#include "coord.hh"

class Cell;
class ComputeUnit;

using namespace std;

/**
 * Hippocampus simulation model, containing its cells.
 * Cells are added incrementally, whenever no cell respond correctly to animat's stimulies.
 */
class Hippo : public QObject
{
  Q_OBJECT

public:
  /// Hippocampus constructor.
  Hippo () : iadd_(true), lastadded_(0), cpt_(0) {};

  /// Hippocampus destructor.
  ~Hippo ();

  /// Add a cell maximally responsive at coordinate position \a pos.
  //inline
  void    cell_add (Coord pos);
  
  /// Cell getter from the hippocampus map, at position \a cell.
  Cell*   cell_get (int cell);

  /// Return the size of hippocampus map.
  inline
  int     size () { return cellmap_.size (); }

  /// Cell incremental adding mode setter.
  inline
  void    iadd_set (bool iadd) { iadd_ = iadd; }

  inline
  bool    iadd_get () { return iadd_; }

  inline
  Cell*   lastadded_get () { return lastadded_; }

	inline
	const Coord& position_get () const { return position_; }

	/**
	* Compute hippocampus cells activities from rat position.
	* @note Update neurons activities.
	* @param ratpos Rat current position.
	*/
	bool synch (const Coord & signal);

  void    draw (ostream& os);

  void    reset ();
  
  inline
  vector<ComputeUnit*>& pop_get () { return cellmap_; }

private:
  vector<ComputeUnit*>   cellmap_;   ///< Collection of cells.
  bool             iadd_;      ///< incremental adding mode.
  Cell*            lastadded_;
  int              cpt_;
	Coord position_;

signals:
  void sig_addcell (int no);
  void sig_reset ();
};

#endif
