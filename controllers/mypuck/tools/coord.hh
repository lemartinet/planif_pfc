/**
 * @file coord.hh
 * Coordinate tool's interface.
 * Simplify simulations of hippocampus model.
 */

#ifndef COORD_HH_
# define COORD_HH_

#include <math.h>
#include <ostream>

using namespace std;

/**
 * Coordinate class.
 * Specialized for 2 dimensions.
 */
class Coord
{
public:
	/**
	 * Coord constructor taking coordinates.
	 * @param x abcisse value.
	 * @param y ordinate value.
	 */
	Coord (float x, float y) : x_(x), y_(y) {};
	Coord () : x_(0), y_(0) {};

	/// Coord destructor.
	~Coord () {};
	
	float x_get () const { return x_; }
	float y_get () const { return y_; }
	void x_set (float x) { x_ = x; }
	void y_set (float y) { y_ = y; }

	/// Euclidian norm of coordinate vector.
	float norm () const { return sqrt (x_ * x_ + y_ * y_); }
	
	Coord operator- (const Coord& coord) const { return Coord (x_ - coord.x_get (), y_ - coord.y_get ()); }
	void operator= (const Coord& coord) { x_ = coord.x_get (); y_ = coord.y_get (); }
	void operator*= (float val) { x_ = x_ * val; y_ = y_ * val; }
	void operator+= (const Coord& coord) { x_ += coord.x_get (); y_ += coord.y_get (); }
	void operator/= (float val) { if (val == 0.0) return; x_ = x_ / val; y_ = y_ / val; }
	
	static void webots_coord_set (double xmin, double ymin, double xmax, double ymax);
	static void gui_coord_set (double xmin, double ymin, double xmax, double ymax);
	static double convertx (const Coord& coord);
	static double converty (const Coord& coord);

private:
	float x_;  ///< x value (abcisse).
	float y_;  ///< y value (ordinate).

	// var for coordinate conversion from webots to gui	
	static double w_xmin_;
	static double w_xmax_;
	static double w_ymin_;
	static double w_ymax_;
	static double g_xmin_;
	static double g_xmax_;
	static double g_ymin_;
	static double g_ymax_;
};

// print a coordinate in the style : "x y"
ostream& operator<< (ostream& os, const Coord& coord);
bool operator== (const Coord& c1, const Coord& c2);

#endif
