#ifndef CELL_HH_
#define CELL_HH_

#include "computeunit.hh"
#include "coord.hh"
#include <vector>

/**
 * Model of Hippocampus cells activity
 * Generate activity fields given the true position of the animat.
 * @note Should be replaced by Angelo's cells models.
 */
class Cell : public ComputeUnit
{
public:
	Cell ();
	Cell (const Coord& pos);
	Cell (const string& filename, const Coord& pos);
	virtual ~Cell ();
	 
	const Coord* pos_get () const { return pos_; }
	void pos_set (const Coord& pos);

	void compute (const Coord& pos, bool peak);

	void draw (ostream& os) const;

private:
	Coord* pos_;            // cell's receptive field center
	std::vector<double> r_; // r_ = [r1, r2, ...]
	std::vector<Coord> posr_; // posr_ = [(x1,y1), (x2,y2), ...]
	static int step_;
};

#endif /*CELL_HH_*/
