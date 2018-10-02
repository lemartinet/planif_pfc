#include "synapse.hh"
#include "neuron.hh"
#include "cell.hh"
#include "params.hh"
#include <iostream>
#include <assert.h>

Synapse::Synapse (const ComputeUnit& from, const ComputeUnit& to, double w, bool constw) : 
	from_(from), from_mult_(0), to_(to), w_(w), constw_(constw), a_(1), b_(0)
{
	//cout << "new Synapse : " << from_.no_get () << "->" << to_.no_get () << endl;
}

Synapse::Synapse (const ComputeUnit& from, const ComputeUnit& from_mult, const ComputeUnit& to, 
	double w, double a, double b) : 
	from_(from), from_mult_(&from_mult), to_(to), w_(w), constw_(true), a_(a), b_(b) 
{
}

Synapse::~Synapse ()
{
}

double Synapse::drive () const
{ 
	return w_ * from_.output () * (a_ + b_ * (from_mult_ == 0 ? 0 : from_mult_->output ()));
}

