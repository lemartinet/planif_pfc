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

  /// Simple coord constructor.
  Coord () : x_(0), y_(0) {};

  /**
   * Coord constructor taking coordinates.
   * @param x abcisse value.
   * @param y ordinate value.
   */
  Coord (float x, float y) : x_(x), y_(y) {};

  /// Coord destructor.
  ~Coord () {};

  /// Abcisse getter.
  inline
  float     x_get () const { return x_; }

  /// Ordinate getter.
  inline
  float     y_get () const { return y_; }

  inline
  void    x_set (float x) { x_ = x; }

  inline
  void    y_set (float y) { y_ = y; }

  /// Euclidian norm of coordinate vector.
  inline
  float  norm () { return sqrt (x_ * x_ + y_ * y_); }

  /// Overload for coord difference.
  inline
  Coord   operator- (Coord& coord) { return Coord (x_ - coord.x_get (), y_ - coord.y_get ()); }

  inline
  void  operator= (Coord& coord) { x_ = coord.x_get (); y_ = coord.y_get (); }

  inline
  void operator*= (float val) { x_ = x_ * val; y_ = y_ * val; }

  inline
  void operator+= (Coord& coord) { x_ += coord.x_get (); y_ += coord.y_get (); }

  inline
  void operator/= (float val) { if (val == 0.0) return; x_ = x_ / val; y_ = y_ / val; }

 private:
  float x_;  ///< x value (abcisse).
  float y_;  ///< y value (ordinate).
};

/**
 * Print a coordinate in the style : Coord (x,y).
 * @param os Output stream (typically std::cout).
 * @param coord Coordinate to print.
 * @return Output stream
 */
ostream& operator<< (ostream& os, Coord& coord);

bool operator== (Coord& c1, Coord& c2);

#endif
