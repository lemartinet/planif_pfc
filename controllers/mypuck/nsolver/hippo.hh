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
	
	const vector<ComputeUnit*>& pop_get () const { return cellmap_; }
	void synch (const Coord& position, bool sleeping=false, int ripples=0);

private:
	void sleep (int ripples);
	const Cell& cell_get (int cell) const;
	bool theta_new () const;
	void cell_add (const Coord& pos);
	double nb_spiking_cells () const;
		
private:
	vector<ComputeUnit*> cellmap_;
	vector<double> activ_;
	int nb_used_pc_; 
	int step_;
	int nb_stable_;
};

#endif
