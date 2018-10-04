#ifndef DCELL_HH_
#define DCELL_HH_

#include "computeunit.hh"

class DCell : public ComputeUnit
{
public:
	DCell (double angle, bool can_rest = false) : angle_(angle), can_rest_(can_rest), resting_(0) {}
	virtual ~DCell() {}
	
	void compute (double current_angle);
	
private:
	const double angle_;
	const bool can_rest_;
	int resting_;
};

#endif /*DCELL_HH_*/
