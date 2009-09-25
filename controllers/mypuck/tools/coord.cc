#include "coord.hh"

ostream& operator<< (ostream& os, Coord& coord)
{
  os << "Coord (" << coord.x_get() << "," << coord.y_get () << ")";
  return os;
}

bool operator== (Coord& c1, Coord& c2)
{
  return (c1.x_get () == c2.x_get ()) && (c1.y_get () == c2.y_get ());
}
