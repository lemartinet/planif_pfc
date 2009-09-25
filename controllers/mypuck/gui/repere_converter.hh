#ifndef REPERE_CONVERTER_HH
# define REPERE_CONVERTER_HH

#include "coord.hh"

class RepereConverter
{
public:
	RepereConverter (float src_min_x, float src_min_y, float src_max_x, float src_max_y,
  					 float dest_min_x, float dest_min_y, float dest_max_x, float dest_max_y) :
  					 x1min_(src_min_x), x1max_(src_max_x), y1min_(src_min_y), y1max_(src_max_y),
  					 x2min_(dest_min_x), x2max_(dest_max_x), y2min_(dest_min_y), y2max_(dest_max_y) {};
  					 
  ~RepereConverter () {};
  
  inline
  float convertx (Coord& coord) { return (coord.x_get () - x1min_) / (x1max_ - x1min_) * (x2max_ - x2min_) + x2min_; }

  inline
  float converty (Coord& coord) { return (coord.y_get () - y1max_) / (y1min_ - y1max_) * (y2min_ - y2max_) + y2max_; }

  inline
  float x2min_get () { return x2min_; }

  inline
  float x2max_get () { return x2max_; }

  inline
  float y2min_get () { return y2min_; }

  inline
  float y2max_get () { return y2max_; }

private:
  float x1min_;
  float x1max_;
  float y1min_;
  float y1max_;

  float x2min_;
  float x2max_;
  float y2min_;
  float y2max_;
};

#endif
