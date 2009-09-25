#ifndef EGOACTIONPOP_HH_
#define EGOACTIONPOP_HH_

#include <vector>

class ComputeUnit;

using namespace std;

class EgoActionPop
{
public:
	EgoActionPop ();
	virtual ~EgoActionPop () {}
	
	void synch (double last_orientation, double orientation);
	
	inline
	vector<ComputeUnit*>& pop_get () { return pop_; }
	
private:
	vector<ComputeUnit*> pop_;
	double begin_orientation_;
	double choosen_ego_action_;
};

#endif /*EGOACTIONPOP_HH_*/
