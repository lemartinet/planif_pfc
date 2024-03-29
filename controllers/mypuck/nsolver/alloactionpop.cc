#include "alloactionpop.hh"
#include "dcell.hh"
#include "math.hh"
#include "params.hh"
#include <cmath>

AlloActionPop::AlloActionPop ()
{
	const int NB_HD = Params::get_int("NB_HD");
	for (int i = 0; i < NB_HD; i++) {
		double angle = -M_PI + i * 2 * M_PI / NB_HD;
		DCell* cell = new DCell (angle);
		pop_.push_back(cell);
	}
}

AlloActionPop::~AlloActionPop ()
{
	vector<ComputeUnit*>::iterator it;
	for (it = pop_.begin(); it != pop_.end(); it++) {
		delete (*it);
	}		
}

void AlloActionPop::synch (double current_action)
{
	vector<ComputeUnit*>::iterator it;
	for (it = pop_.begin(); it != pop_.end(); it++) {
		dynamic_cast<DCell *>(*it)->compute (current_action);
	}
}
