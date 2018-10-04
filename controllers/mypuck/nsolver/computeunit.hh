#ifndef COMPUTEUNIT_H
# define COMPUTEUNIT_H

class ComputeUnit
{
public:
	ComputeUnit () : output_(0), no_(nb_computeunits) { nb_computeunits++; };
	ComputeUnit (double output) : output_(output), no_(nb_computeunits) { nb_computeunits++; };
	   
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
