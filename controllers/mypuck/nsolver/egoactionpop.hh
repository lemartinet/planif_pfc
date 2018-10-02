#ifndef EGOACTIONPOP_HH_
#define EGOACTIONPOP_HH_

#include <vector>
#include <queue>

class ComputeUnit;

using namespace std;

class EgoActionPop
{
public:
	EgoActionPop ();
	virtual ~EgoActionPop ();
	
	void synch (double orientation);
	
	const vector<ComputeUnit*>& pop_get () const { return pop_; }
	
private:
	vector<ComputeUnit*> pop_;
	queue<double> angles_;
};

#endif /*EGOACTIONPOP_HH_*/
