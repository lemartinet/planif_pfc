#ifndef CELL_HH_
#define CELL_HH_

#include "computeunit.hh"
#include "coord.hh"
#include "log.hh"
#include <vector>

/**
 * Model of Hippocampus cells activity
 * Generate activity fields given the true position of the animat.
 * @note Should be replaced by Angelo's cells models.
 */
class Cell : public ComputeUnit
{
public:
	/// Constructor taking maximum response at coordinate \a pos.
	Cell (const Coord& pos);
	Cell (const string& filename, const Coord& pos);
	virtual ~Cell ();
	 
	/**
	 * Compute cell activity from the rat position.
	 * @note Update cell activity. 
	 */
	void compute (const Coord& pos);

	/// Maximum response coordinate getter.
	const Coord& pos_get () const { return pos_; }
	void draw (ostream& os) const;
	void log (const string& time_string, const Coord& position, double angle, int day, int trial) const;

private:
	const Coord pos_;            // cell's receptive field center
	Log* log_;
	std::vector<double> r_; // r_ = [r1, r2, ...]
	std::vector<Coord> posr_; // posr_ = [(x1,y1), (x2,y2), ...]
};

#endif /*CELL_HH_*/
