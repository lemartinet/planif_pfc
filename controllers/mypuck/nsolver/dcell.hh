#ifndef DCELL_HH_
#define DCELL_HH_

#include "computeunit.hh"

class DCell : public ComputeUnit
{
public:
	DCell (double angle) { angle_ = angle; }
	virtual ~DCell() {}
	
	void compute (double current_angle);
	
private:
	double angle_;
};

#endif /*DCELL_HH_*/
