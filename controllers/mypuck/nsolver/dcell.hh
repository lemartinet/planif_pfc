#ifndef DCELL_HH_
#define DCELL_HH_

#include "computeunit.hh"

class DCell : public ComputeUnit
{
public:
	DCell (double angle) : ComputeUnit(DC), angle_(angle) {}
	virtual ~DCell() {}
	
	virtual void compute (double current_angle);
	
protected:
	const double angle_;
};

#endif /*DCELL_HH_*/
