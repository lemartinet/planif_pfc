#include "hippo.hh"
#include "math.hh"
#include "params.hh"
#include "cell.hh"
#include "computeunit.hh"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/casts.hpp>
#include <algorithm>

using namespace boost::lambda;

extern Params* params;

Hippo::~Hippo ()
{
    for_each (cellmap_.begin (), cellmap_.end (), bind (delete_ptr(), _1));
}

void Hippo::cell_add (Coord pos)
{
	Coord noised_pos (pos.x_get () + (drand () - 0.5) / 10, pos.y_get () + (drand () - 0.5) / 10);
	Cell* cell = new Cell (noised_pos);
	cellmap_.push_back (cell);
	lastadded_ = cell;
	emit sig_addcell (cell->no_get ());
}

bool Hippo::synch (const Coord & signal)
{
	// Minimum activation of a cell, under wich a new cell should be created.
	static const double CELL_FIRE_MIN = params->get_double("CELL_FIRE_MIN");
//	static const double CELL_SIMULT_MIN = params->get_double("CELL_SIMULT_MIN");

	position_.x_set (signal.x_get ());
	position_.y_set (signal.y_get ());
	lastadded_ = 0;
	
	for_each (cellmap_.begin (), cellmap_.end (), bind (&Cell::compute, ll_dynamic_cast<Cell*>(_1), position_));
	vector<ComputeUnit*>::iterator it = find_if (cellmap_.begin (), cellmap_.end (), 
		bind (&Cell::output, ll_dynamic_cast<Cell*>(_1)) > CELL_FIRE_MIN);
	bool winner = (it != cellmap_.end ());
//	bool winner = (nb_spiking_cells () >= 6);
//	if (iadd_ && !winner && (cpt_ % 10 == 0)) {
	if (iadd_ && !winner) {
		cell_add (signal);
		Cell* cell = dynamic_cast<Cell *>(cellmap_[cellmap_.size () - 1]);
		cell->compute (position_);
	}
//	cpt_++;
//	printf ("nb_spiking_cells: %d\n", nb_spiking_cells ());
	return !winner;
}

int Hippo::nb_spiking_cells () const
{
	return count_if (cellmap_.begin (), cellmap_.end (), bind (&ComputeUnit::spiking, _1)); 
}

void Hippo::draw (ostream& os) const
{
	os << "subgraph cluster_hippo" << " {" << endl;
	os << "style=filled;" << endl;
	os << "color=grey;" << endl;
	os << "node [style=filled,color=white];" << endl;
	for_each (cellmap_.begin (), cellmap_.end (), bind (&Cell::draw, ll_dynamic_cast<Cell*>(_1), var (os)));
	os << "label = \"hippocampe\";" << endl;
	os << "}" << endl;
}

const Cell& Hippo::cell_get (int cell) const
{
 	return dynamic_cast<const Cell&>(*cellmap_[cell]); 
}
