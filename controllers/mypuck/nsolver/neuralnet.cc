#include <iostream>
#include <assert.h>
#include "neuralnet.hh"
#include "hippo.hh"
#include "cell.hh"
#include "params.hh"

extern Params* params;

Neuralnet::~Neuralnet ()
{
  int size = neurons_.size ();

  for (int i = 0; i < size; i++)
    delete neurons_[i];
}

double Neuralnet::lateral_learning (Neuron& from, Neuron& to, bool increase)
{
	static const double LATERAL_LEARNING_STEP = params->get_double("LATERAL_LEARNING_STEP");
	static const double MAX_LATERAL_WEIGHT = params->get_double("MAX_LATERAL_WEIGHT");
	
	// on apprend le lien en un coup (valeur à MAX_LATERAL_WEIGHT) 
	// mais on desapprend 4 fois moins vite
	
	Synapse* s = get_synapse (from, to);
	if (!s) {
		// on recrute une synapse
		add_synapse (from, to, MAX_LATERAL_WEIGHT, true);
		return MAX_LATERAL_WEIGHT;
	}
	else {
		// on modifie la synapse
		double val = increase ? MAX_LATERAL_WEIGHT : (s->w_get () * -LATERAL_LEARNING_STEP);
		//cout << "old " << s->w_get ();
		double new_w = s->w_get () + val;
		if (new_w > MAX_LATERAL_WEIGHT) {
			new_w = MAX_LATERAL_WEIGHT;
		}
		else if (new_w < 0) {
			new_w = 0;	
		}
		s->w_set (new_w);
		//cout << " new " << s->w_get () << endl;
		return new_w;
	}
}

void Neuralnet::synch ()
{
	int size = neurons_.size ();

	for (int i = 0; i < size; i++) {
		neurons_[i]->compute ();
	}
}

void Neuralnet::propagate (int nb)
{
  for (int i = 0; i < nb; i++)
    synch ();
}

void Neuralnet::synch_learn ()
{
	static const int TETA_RYTHM = params->get_int("TETA_RYTHM");
	
	synch ();
	if (!(freq_ % TETA_RYTHM)) {
		int size = neurons_.size ();
		for (int i = 0; i < size; i++)
		neurons_[i]->learn ();
	}
	freq_++;
}

Neuron* Neuralnet::add_neuron (double Vr, double Vf, string& path, int no_col, double ip_step, 
								double ip_mu, double a, double b, int level)
{
  Neuron* res;

  res = new Neuron (Vr, Vf, path, no_col, neurons_.size (), false, ip_step, ip_mu, a, b, level);
  neurons_.push_back (res);
  return res;
}

Neuron* Neuralnet::add_neuron_max (double Vr, double Vf, string& path, int no_col, int level)
{
  Neuron* res;

  res = new Neuron (Vr, Vf, path, no_col, neurons_.size (), true, 0.0, 0.0, 0.0, 0.0, level);
  neurons_.push_back (res);
  return res;
}

void Neuralnet::del_neuron (Neuron* neuron)
{
  int nbneurons = neurons_.size ();
  vector<Neuron *>::iterator it;

  if (!neuron)
    return;
  for (int i = 0; i < nbneurons; i++)
    {
      neurons_[i]->del_synapse (*neuron);
    }
  it = find (neurons_.begin (), neurons_.end (), neuron);
  //certains neurones semblent etre supp. plusieurs fois. chercher l'erreur...
  assert (it != neurons_.end ());
  if (it != neurons_.end ())
    {
      delete *it;
      neurons_.erase (it);
    }
}

void Neuralnet::draw_graph (ostream& os)
{
  int size = neurons_.size ();
  int nbsyn = 0;
  Cell* cell = 0;
  Neuron* neuron = 0;

  os << "digraph G {" << endl;
  for (int i = 0; i < size; i++)
    {
      os << "n" << neurons_[i]->no_get () << " [label=\""<< i << ":" << neurons_[i]->output () << "\"]" << endl;
      nbsyn = neurons_[i]->size ();
      for (int j = 0; j < nbsyn; j++)
	{
	  if ((cell = dynamic_cast <Cell *>(&(neurons_[i]->syn_get (j)->from_get ())))/*neurons_[i]->syn_get (j)->input_get ()*/)
	    {
	    	double cell_output = cell->output ();
	      os << "c" << &cell_output << " [label=\"" << j << ":" << &cell_output << "\", color=grey, style=filled];" << endl;
	      os << "c" << &cell_output;
	    }
	  else
	    {
	      neuron = dynamic_cast <Neuron *> (&(neurons_[i]->syn_get (j)->from_get ()));
	      os << "n" << neuron->no_get ();
	    }
	  os << " -> n" << neurons_[i]->no_get () << " [label=\"" << neurons_[i]->syn_get (j)->w_get () << "\"];" << endl;
	}
    }
  os << "}" << endl;
}

void Neuralnet::draw_links (ostream& os)
{
  int size = neurons_.size ();

  for (int i = 0; i < size; i++)
    neurons_[i]->draw_links (os);
}

// void Neuralnet::unset_all ()
// {
//   int size = neurons_.size ();

//   for (int i = 0; i < size; i++)
//     neurons_[i]->unset_activity ();
// }

