#ifndef HIPPO_HH_
#define HIPPO_HH_

#include <vector>
#include <ostream>

class Cell;
class ComputeUnit;
class Coord;

using namespace std;

/**
 * Hippocampus simulation model, containing its cells.
 * Cells are added incrementally, whenever no cell respond correctly to animat's stimulies.
 */
class Hippo
{
public:
	Hippo ();
	~Hippo ();
	
	const Cell& cell_get (int cell) const;
	int size () const { return cellmap_.size (); }
	const vector<ComputeUnit*>& pop_get () const { return cellmap_; }

	void cell_add (const Coord& pos);
	bool synch (const Coord& position);
	bool synch (int numcell);
	int nb_spiking_cells () const;

	void draw (ostream& os) const;

private:
	vector<ComputeUnit*> cellmap_;
	int nb_used_pc_; 
	int step_;
};

#endif
