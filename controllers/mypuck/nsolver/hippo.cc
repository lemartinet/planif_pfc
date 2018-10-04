#include "hippo.hh"
#include "math.hh"
#include "params.hh"
#include "cell.hh"
#include "computeunit.hh"
#include "logger.hh"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/casts.hpp>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace boost::lambda;

Hippo::Hippo () 
{
	static const int LOAD_PC = Params::get_int("LOAD_PC");
	if (LOAD_PC) {
		ostringstream pc_centers;
		pc_centers <<  Params::get_path_pc () << "pref_pos.dat";
		ifstream file (pc_centers.str ().c_str ());
		vector<Coord> centers;
		double x,y;
		while (!file.eof ()) {
			file >> x >> y;
			centers.push_back (Coord (x,y));
		}
		file.close ();
		for (int i = 0; i < (int)centers.size (); i++) {
			ostringstream filename;
			filename << Params::get_path_pc () << "cells/" << i << ".txt";
			Cell* cell = new Cell (filename.str (), centers[i]);
			cellmap_.push_back (cell);
			Logger::add("pc", cell);
		}
		iadd_ = false;
		lastadded_ = dynamic_cast<Cell*> (cellmap_[cellmap_.size () - 1]);
		nb_used_pc_ = cellmap_.size ();
	}
	else {
		iadd_ = true;
		lastadded_ = 0;
		nb_used_pc_ = 0;
	}
	// on complete la population pour arriver a SIZE_POP
	static const unsigned int SIZE_POP = Params::get_int("SIZE_POP");
	while (cellmap_.size() < SIZE_POP) {
		Cell* cell = new Cell ();
		cellmap_.push_back (cell);
		Logger::add("pc", cell);
	}
}

Hippo::~Hippo ()
{
    for_each (cellmap_.begin (), cellmap_.end (), bind (delete_ptr(), _1));
}

void Hippo::cell_add (Coord pos)
{
	static const int SIZE_POP = Params::get_int("SIZE_POP");
	if (nb_used_pc_ >= SIZE_POP) {
		cout << "[Mypuck] Too much place cells" << endl;	
	}
	Coord noised_pos (pos.x_get () + (drand () - 0.5) / 10, pos.y_get () + (drand () - 0.5) / 10);
	lastadded_ = dynamic_cast<Cell*> (cellmap_[nb_used_pc_]);
	lastadded_->pos_set (noised_pos);
	emit sig_addcell (lastadded_->no_get ());
	nb_used_pc_++;
}

bool Hippo::synch (const Coord & signal)
{
	// Minimum activation of a cell, under wich a new cell should be created.
	static const double CELL_FIRE_MIN = Params::get_double("CELL_FIRE_MIN");
//	static const double CELL_SIMULT_MIN = Params::get_double("CELL_SIMULT_MIN");

	position_.x_set (signal.x_get ());
	position_.y_set (signal.y_get ());
	lastadded_ = 0;
	
	for_each (cellmap_.begin (), cellmap_.end (), bind (&Cell::compute, ll_dynamic_cast<Cell*>(_1), position_));
	vector<ComputeUnit*>::iterator it = find_if (cellmap_.begin (), cellmap_.end (), 
		bind (&Cell::output, ll_dynamic_cast<Cell*>(_1)) > CELL_FIRE_MIN);
	bool winner = (it != cellmap_.end ());
//	bool winner = (nb_spiking_cells () >= 6);
	if (iadd_ && !winner) {
		cell_add (signal);
		Cell* cell = dynamic_cast<Cell *>(cellmap_[cellmap_.size () - 1]);
		cell->compute (position_);
	}
//	cout << "nb_spiking_cells: " << nb_spiking_cells () << endl;

	static bool first_synch = true;
	static const int LOAD_PC = Params::get_int("LOAD_PC");
	if (LOAD_PC && first_synch) {
		first_synch = false;
		for (int i = 0; i < nb_used_pc_; i++) {
			Cell* cell = dynamic_cast<Cell*>(cellmap_[i]);
			emit sig_addcell (cell->no_get ());
		}
	}
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
