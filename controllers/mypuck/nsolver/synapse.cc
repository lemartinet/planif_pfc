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

void Synapse::BCM ()
{
	double& wij = w_;
	double rj = from_.output ();
	double ri = to_.output ();
	static const double HLEARN_RATE = Params::get_double("HLEARN_RATE");
	static const double DELTA_T = Params::get_double("DELTA_T");
	
	if (rj >= 0.3 && ri >= 0.1) {
		wij += (DELTA_T * (rj * (ri - wij))) / HLEARN_RATE;
//		wij += ri * rj * (1 - wij);
    }
//	wij = wij < 0.0 ? 0.0 : wij;
//	wij = wij > 1.0 ? 1.0 : wij;
//	tetaV += deltaT * (ri * ri - tetaV) / TETAV_RATE;
//	assert (wij >= 0.0 && wij <= 1.0);
	assert (wij >= 0.0);
}

void Synapse::hlearn ()
{
//	double tetaV = to_.tetaV_get ();
//	double from = 0.0;
	
	if (!constw_) {
		BCM ();
		cout << "learning synapse !!!" << endl;
//		to_->tetaV_set (tetaV);
	}
}

double Synapse::drive () const
{ 
	return w_ * from_.output () * (a_ + b_ * (from_mult_ == 0 ? 0 : from_mult_->output ()));
}

