#include "egoactionpop.hh"
#include "dcell.hh"
#include "math.hh"

EgoActionPop::EgoActionPop ()
{
	DCell* cell = new DCell (-PI/2, true);
	pop_.push_back(cell);
	cell = new DCell (0, true);
	pop_.push_back(cell);
	cell = new DCell (PI/2, true);
	pop_.push_back(cell);
}

EgoActionPop::~EgoActionPop ()
{
	vector<ComputeUnit*>::iterator it;
	for (it = pop_.begin(); it != pop_.end(); it++) {
		delete (*it);
	}		
}

void EgoActionPop::synch (double orientation)
{
	if (angles_.size () < 4) {
		// on étudie la variation de l'orientation sur 4 time steps
		angles_.push (orientation);
		return;
	}
	else {
		double diff = ecart_angulaire (angles_.front (), orientation);
		angles_.pop ();
		angles_.push (orientation);
		dynamic_cast<DCell *>(pop_.at (1))->compute (diff);
	}	
}
