#ifndef EGOACTIONPOP_HH_
#define EGOACTIONPOP_HH_

#include <queue>
#include "dcell.hh"

using namespace std;

class EgoAction : public DCell
{
public:
	EgoAction(): DCell (0), resting_(0) {}
	~EgoAction() {}
	
	void compute(double orientation, int nb_path);
	
private:
	queue<double> angles_;
	int resting_;
};

#endif /*EGOACTIONPOP_HH_*/
