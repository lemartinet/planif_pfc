#include "coord.hh"

double Coord::w_xmin_, Coord::w_xmax_, Coord::w_ymin_, Coord::w_ymax_;
double Coord::g_xmin_, Coord::g_xmax_, Coord::g_ymin_, Coord::g_ymax_;

void Coord::webots_coord_set (double xmin, double ymin, double xmax, double ymax)
{
	w_xmin_ = xmin;
	w_ymin_ = ymin;
	w_xmax_ = xmax;
	w_ymax_ = ymax;
}

void Coord::gui_coord_set (double xmin, double ymin, double xmax, double ymax)
{
	g_xmin_ = xmin;
	g_ymin_ = ymin;
	g_xmax_ = xmax;
	g_ymax_ = ymax;
}

double Coord::convertx (const Coord& coord) 
{ 
	return (coord.x_get () - w_xmin_) / (w_xmax_ - w_xmin_) * (g_xmax_ - g_xmin_) + g_xmin_; 
}

double Coord::converty (const Coord& coord) 
{ 
	return (coord.y_get () - w_ymax_) / (w_ymin_ - w_ymax_) * (g_ymin_ - g_ymax_) + g_ymax_; 
}

ostream& operator<< (ostream& os, const Coord& coord)
{
	os << coord.x_get() << " " << coord.y_get ();
	return os;
}

bool operator== (const Coord& c1, const Coord& c2)
{
	return (c1.x_get () == c2.x_get ()) && (c1.y_get () == c2.y_get ());
}
