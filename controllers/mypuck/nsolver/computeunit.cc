#include "computeunit.hh"
#include "params.hh"
#include "logger.hh"
#include <sstream>

int ComputeUnit::nb_computeunits = 0;

ComputeUnit::ComputeUnit (nType type) : output_(0), no_(nb_computeunits) 
{ 
	nb_computeunits++;
	stringstream s;
	s << no_ << " " << type;
	Logger::log("type", s.str());
	Logger::add ("fr", this);
	Logger::add ("weight", this);
};

ComputeUnit::ComputeUnit (nType type, double output) : output_(output), no_(nb_computeunits) 
{ 
	nb_computeunits++;
	stringstream s;
	s << no_ << " " << type;
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
