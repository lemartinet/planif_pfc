#include <assert.h>
#include "synapse.hh"
#include "neuron.hh"
#include "cell.hh"
#include "params.hh"

extern Params* params;
double alpha = 0.9;

void BCM (double& wij, double rj, double ri)
{
	static const double HLEARN_RATE = params->get_double("HLEARN_RATE");
	static const double DELTA_T = params->get_double("DELTA_T");
  if (rj >= 0.3 && ri >= 0.1)
    {
      wij += (DELTA_T * (rj * (ri - wij))) / HLEARN_RATE;
      //wij += deltaT * (ri * rj - alpha * wij * ri * ri) / HLEARN_RATE;
      //wij += ri * rj * (1 - wij);
      //wij = deltaT * (rj * ri - 0.001) / HLEARN_RATE;
    }
//   wij = wij < 0.0 ? 0.0 : wij;
//   wij = wij > 1.0 ? 1.0 : wij;
  //wij = fabs (wij);
  //tetaV += deltaT * (ri * ri - tetaV) / TETAV_RATE;
  //cout << ri - tetaV << endl;
  //assert (wij >= 0.0 && wij <= 1.0);
  assert (wij >= 0.0);
}

void Synapse::hlearn ()
{
  //double tetaV = to_.tetaV_get ();
  //double from = 0.0;

  if (!constw_)
    {
      /*
      if (to_.no_get () == 3 && from_.no_get () == 23)
	{
	  //cout << to_.output () - tetaV << endl;
	   cout << w_ << endl;
	}
      */

      BCM (w_, from_->output (), to_->output ());
      printf("learning synapse !!!\n");
      //to_->tetaV_set (tetaV);
    }
}

void Synapse::draw_links (ostream& os)
{
  Neuron* dest = dynamic_cast<Neuron *> (to_);
  Neuron* neuron = 0;
  Cell*   cell = 0;

  if ((cell = dynamic_cast<Cell *> (from_))) {
  	double cell_output = cell->output (); 
    os << "c" << &cell_output<< " -> " << dest->path_get () << " [color=green];" << endl;
  }
  else
    {
      neuron = dynamic_cast<Neuron *> (from_);
      if (neuron->no_col_get () != dest->no_col_get ())
	os << neuron->path_get () << " -> " << dest->path_get () << " [color=blue];" << endl;
      else
	os << neuron->path_get () << " -> " << dest->path_get () << ";" << endl;
    }
}
