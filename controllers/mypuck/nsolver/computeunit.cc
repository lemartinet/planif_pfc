#include "computeunit.hh"
#include "params.hh"
#include "logger.hh"
#include <sstream>

#define NB_STEP 8

int ComputeUnit::nb_computeunits = 0;

ComputeUnit::ComputeUnit (nType type) : output_(0), no_(nb_computeunits), lastTidx_(0) 
{ 
	nb_computeunits++;
	stringstream s;
	s << no_+1 << " " << type;
	Logger::log("type", s.str());
	Logger::add ("fr", this);
	Logger::add ("weight", this);
};

ComputeUnit::ComputeUnit (nType type, double output) : output_(output), no_(nb_computeunits), lastTidx_(0) 
{ 
	nb_computeunits++;
	stringstream s;
	s << no_+1 << " " << type;
	Logger::log("type", s.str());
	Logger::add ("fr", this);
	Logger::add ("weight", this);
};


bool ComputeUnit::spiking () const
{
	static const double NEURON_SPIKING_THRESH = Params::get_double("NEURON_SPIKING_THRESH");
	return output () > NEURON_SPIKING_THRESH;
}

bool operator== (const ComputeUnit& n1, const ComputeUnit& n2)
{
  return &n1 == &n2;
}

void ComputeUnit::update_recent()
{
	// on garde un historique de l'activation sur X time steps
	if (lastTrecent_.size () < NB_STEP) {
		lastTrecent_.push_back (output_);
	}
	else {
		lastTrecent_[lastTidx_] = output_;
		lastTidx_ = ++lastTidx_ % NB_STEP;
	}
}

const vector<double>& ComputeUnit::lastT_recent() const
{
	return lastTrecent_;
}
