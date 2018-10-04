#include "computeunit.hh"
#include "params.hh"

extern Params* params;

bool operator== (const ComputeUnit& n1, const ComputeUnit& n2)
{
  return &n1 == &n2;
}

int ComputeUnit::nb_computeunits = 0;

bool ComputeUnit::spiking () const
{
	static const double NEURON_SPIKING_THRESH = params->get_double("NEURON_SPIKING_THRESH");
	return output () > NEURON_SPIKING_THRESH;
}