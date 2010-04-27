#ifndef COMPUTEUNIT_H
# define COMPUTEUNIT_H

#include <vector>

using namespace std;

enum nType { 
	STATE = 0, 
	INF = 1, 
	LAT = 2,
	SUP = 3, 
	MAXSUP = 4,
	PC = 10,
	DC = 11, 
	MOTIV = 12
};


class ComputeUnit
{
public:
	ComputeUnit (nType type, double output=0);
	   
	virtual ~ComputeUnit () { --nb_computeunits; };
	
	double output () const { return output_; }
	void output_set (double output) { output_ = output; }
	bool spiking () const;
	int no_get () const { return no_; }
	static int nb_computeunits_get () { return nb_computeunits; }
	const vector<double>& lastT_recent () const { return lastTrecent_; }
	void update_recent();
	 
protected:
	double output_;
	const int no_;
  
private:
	static int nb_computeunits;
	vector<double> lastTrecent_;
	int lastTidx_;
};

bool operator== (const ComputeUnit& n1, const ComputeUnit& n2);

#endif
