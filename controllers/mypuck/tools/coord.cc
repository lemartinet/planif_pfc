#include "coord.hh"

ostream& operator<< (ostream& os, const Coord& coord)
{
	os << coord.x_get() << " " << coord.y_get ();
	return os;
}

bool operator== (const Coord& c1, const Coord& c2)
{
	return (c1.x_get () == c2.x_get ()) && (c1.y_get () == c2.y_get ());
}
