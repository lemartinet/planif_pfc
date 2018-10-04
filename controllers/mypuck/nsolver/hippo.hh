#ifndef HIPPO_HH_
#define HIPPO_HH_

#include <QObject>
#include <vector>
#include "coord.hh"
#include <ostream>

class Cell;
class ComputeUnit;

/**
 * Hippocampus simulation model, containing its cells.
 * Cells are added incrementally, whenever no cell respond correctly to animat's stimulies.
 */
class Hippo : public QObject
{
	Q_OBJECT

public:
	Hippo ();
	~Hippo ();
	
	const Cell& cell_get (int cell) const;
	int size () const { return cellmap_.size (); }
	void iadd_set (bool iadd) { iadd_ = iadd; }
	bool iadd_get () const { return iadd_; }
	Cell* lastadded_get () const { return lastadded_; }
	const Coord& position_get () const { return position_; }
	const vector<ComputeUnit*>& pop_get () const { return cellmap_; }

	void cell_add (Coord pos);
	/**
	* Compute hippocampus cells activities from rat position.
	* @note Update neurons activities.
	* @param ratpos Rat current position.
	*/
	bool synch (const Coord & signal);
	int nb_spiking_cells () const;

	void draw (ostream& os) const;

private:
	std::vector<ComputeUnit*> cellmap_;   ///< Collection of cells.
	bool iadd_;      ///< incremental adding mode.
	Cell* lastadded_;
	Coord position_;
	int cpt_; 

signals:
	void sig_addcell (int no);
};

#endif
