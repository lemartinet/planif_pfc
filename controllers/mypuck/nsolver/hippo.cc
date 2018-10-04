#include "hippo.hh"
#include "math.hh"
#include "params.hh"
#include "cell.hh"
#include "computeunit.hh"
#include "logger.hh"
#include "coord.hh"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/casts.hpp>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace boost::lambda;

Hippo::Hippo () : step_(0)
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
		}
		nb_used_pc_ = cellmap_.size ();
	}
	else {
		nb_used_pc_ = 0;
	}
	// on complete la population pour arriver a SIZE_POP
	static const unsigned int SIZE_POP = Params::get_int("SIZE_POP");
	while (cellmap_.size() < SIZE_POP) {
		Cell* cell = new Cell ();
		cellmap_.push_back (cell);
	}
}

Hippo::~Hippo ()
{
    for_each (cellmap_.begin (), cellmap_.end (), bind (delete_ptr(), _1));
}

void Hippo::cell_add (const Coord& pos)
{
	static const int SIZE_POP = Params::get_int("SIZE_POP");
	if (nb_used_pc_ >= SIZE_POP) {
		cout << "[Mypuck] Too much place cells" << endl;	
	}
	Coord noised_pos (pos.x_get () + (drand () - 0.5) / 10, pos.y_get () + (drand () - 0.5) / 10);
	Cell* cell = dynamic_cast<Cell*> (cellmap_[nb_used_pc_]);
	cell->pos_set (noised_pos);
//	cell->compute (pos);
	nb_used_pc_++;
}

bool Hippo::synch (const Coord & position)
{
	// Minimum activation of a cell, under wich a new cell should be created.
	static const double CELL_FIRE_MIN = Params::get_double("CELL_FIRE_MIN");
//	static const double CELL_SIMULT_MIN = Params::get_double("CELL_SIMULT_MIN");
	static const int LOAD_PC = Params::get_int("LOAD_PC");

	// les pc sont actives pendant les 2/4 step du rythme (= 60ms sur 120ms)
	static const double THETA_MOD = Params::get_int ("THETA_MOD");
	bool peak = true;
	if (THETA_MOD && (step_ == 2 || step_ == 3)) {
		peak = false;
	}
	step_ = ++step_ % 4;
	for_each (cellmap_.begin (), cellmap_.end (), bind (&Cell::compute, ll_dynamic_cast<Cell*>(_1), position, peak));
	
	// recruitement dynamique
	vector<ComputeUnit*>::iterator it = find_if (cellmap_.begin (), cellmap_.end (), 
		bind (&Cell::output, ll_dynamic_cast<Cell*>(_1)) > CELL_FIRE_MIN);
//	cout << "hp: " << (*it)->output () << endl;
	bool winner = (it != cellmap_.end ());
//	bool winner = (nb_spiking_cells () >= 6);
	if (!LOAD_PC && !winner && peak) {
		cell_add (position);
	}
//	cout << "nb_spiking_cells: " << nb_spiking_cells () << endl;
	return !winner;
}

bool Hippo::synch (int numcell)
{
	// on met ttes les cells à 0
	for_each (cellmap_.begin (), cellmap_.end (), bind (&Cell::compute, ll_dynamic_cast<Cell*>(_1), Coord(100,100), false));
	// on a 10% de chance d'activer la cellule donnee
	if (drand () > 0.9) {
		Cell* cell = dynamic_cast<Cell*> (cellmap_[numcell]);
		cell->compute (*cell->pos_get (), true);
		return true;
	}
	return false;
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

bool Hippo::theta_new () const 
{ 
	static const double THETA_MOD = Params::get_int ("THETA_MOD");
	return THETA_MOD && (step_ == 1); 
}
