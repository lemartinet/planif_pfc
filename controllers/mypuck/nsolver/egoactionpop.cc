#include "egoactionpop.hh"
#include "computeunit.hh"
#include "dcell.hh"
#include "math.hh"
#include <cmath>
#include "params.hh"

extern Params* params;

EgoActionPop::EgoActionPop () : begin_orientation_ (0), choosen_ego_action_ (0)
{
	DCell* cell = new DCell (-PI/2);
	pop_.push_back(cell);
	cell = new DCell (0);
	pop_.push_back(cell);
	cell = new DCell (PI/2);
	pop_.push_back(cell);
}

void EgoActionPop::synch (double last_orientation, double orientation)
{
	// mise a jour de la dir ego
	static const double ROT_THRESH = params->get_double ("ROT_THRESH");
	double diff = fabs (ecart_angulaire (last_orientation, orientation));
	if (diff < ROT_THRESH) {
		choosen_ego_action_ = ecart_angulaire (begin_orientation_, orientation);
		begin_orientation_ = orientation;
	}
	
	dynamic_cast<DCell *>(pop_.at (0))->compute (choosen_ego_action_);
	dynamic_cast<DCell *>(pop_.at (1))->compute (choosen_ego_action_);
	dynamic_cast<DCell *>(pop_.at (2))->compute (choosen_ego_action_);
	
//	printf("%f\n", diff);
//	printf("%f %f %f\n", pop_.at (0)->output (), pop_.at (1)->output (), pop_.at (2)->output ());
}
