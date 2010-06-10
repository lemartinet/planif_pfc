#include "computeunit.hh"
#include "logger.hh"
#include <sstream>
#include <numeric>

int ComputeUnit::nb_computeunits = 0;

ComputeUnit::ComputeUnit (nType type, double output) : output_(output), no_(nb_computeunits), lastTidx_(0) 
{ 
	nb_computeunits++;
	stringstream s;
	s << no_+1 << " " << type;
	Logger::log("type", s.str());
	Logger::add ("fr", this);
	Logger::add ("weight", this);
}

bool operator== (const ComputeUnit& n1, const ComputeUnit& n2)
{
  return &n1 == &n2;
}

void ComputeUnit::update_recent()
{
	static const unsigned int NB_STEP = Params::get_int("NEURON_HISTORY");
	// on garde un historique de l'activation sur X time steps
	if (lastTrecent_.size () < NB_STEP) {
		lastTrecent_.push_back (output_);
	}
	else {
		lastTrecent_[lastTidx_] = output_;
		lastTidx_ = ++lastTidx_ % NB_STEP;
	}
}

double ComputeUnit::mean_recent() const
{
	static const unsigned int NB_STEP = Params::get_int("NEURON_HISTORY");
	return accumulate (lastTrecent_.begin(), lastTrecent_.end(), 0.0) / NB_STEP;
}
