#ifndef COMPUTEUNIT_H
# define COMPUTEUNIT_H

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
	ComputeUnit (nType type);
	ComputeUnit (nType type, double output);
	   
	virtual ~ComputeUnit () { --nb_computeunits; };
	
	double output () const { return output_; }
	bool spiking () const;
	int no_get () const { return no_; }

protected:
	double output_;
	const int no_;
  
private:
	static int nb_computeunits;
};

bool operator== (const ComputeUnit& n1, const ComputeUnit& n2);

#endif
