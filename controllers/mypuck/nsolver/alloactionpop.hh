#ifndef ALLOACTIONPOP_HH_
#define ALLOACTIONPOP_HH_

#include <vector>

class ComputeUnit;

using namespace std;

class AlloActionPop
{
public:
	AlloActionPop ();
	virtual ~AlloActionPop () {}
	
	void synch (double current_action);
	
	inline
	vector<ComputeUnit*>& pop_get () { return pop_; }
	
private:
	vector<ComputeUnit*> pop_;
};

#endif /*ALLOACTIONPOP_HH_*/
