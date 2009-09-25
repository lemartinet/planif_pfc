#include "alloactionpop.hh"
#include "computeunit.hh"
#include "dcell.hh"
#include "math.hh"

AlloActionPop::AlloActionPop ()
{
	DCell* cell = new DCell (-PI/2);
	pop_.push_back(cell);
	cell = new DCell (0);
	pop_.push_back(cell);
	cell = new DCell (PI/2);
	pop_.push_back(cell);
	cell = new DCell (PI);
	pop_.push_back(cell);
}

void AlloActionPop::synch (double current_action)
{
//	printf("%f %f %f %f\n", pop_.at (0)->compute (), pop_.at (1)->compute (), pop_.at (2)->compute (), pop_.at (3)->compute ());
	dynamic_cast<DCell *>(pop_.at (0))->compute (current_action);
	dynamic_cast<DCell *>(pop_.at (1))->compute (current_action);
	dynamic_cast<DCell *>(pop_.at (2))->compute (current_action);
	dynamic_cast<DCell *>(pop_.at (3))->compute (current_action);
}
