#include "minicol.hh"
#include "neuralnet.hh"
#include "column.hh"
#include "params.hh"
#include "math.hh"
#include <sstream>
#include <iostream>

extern Params* params;

Minicol::Minicol (Neuralnet& net, Action* action, const Column& src, const Column& dest, 
					const string& path, int no_col, int no, int level) :
	no_(no), sup_(net.add_neuron_max (path, no_col, level)),
	inf_(net.add_neuron_max (path, no_col, level)), 
	action_(action?new Action (*action):action),					
	net_(net), src_(src), dest_(dest),
	mean_val_(inf_.output()), level_(level)
{
	// intra-colomn connections
	inf_.add_synapse (src_.state_get (), sup_, 1.0, 0, 1);
	inf_.add_synapse (src_.inf_get (), sup_, 1.0, 0, 1);
	src_.sup_get ().add_synapse (sup_, 1.0, true);
	
  	// Inter-columns connections
  	net_.lateral_learning (inf_, dest.inf_get (), true);
  	net_.lateral_learning (dest.sup_get (), sup_, true);
}

Minicol::~Minicol ()
{
  delete action_;
}

void Minicol::draw (ostream& os) const
{
  os << "{<n" << sup_.no_get () << "> s" << no_ << " | <n" << inf_.no_get () << "> i" << no_ << "}";
}

void Minicol::draw_links (ostream& os) const
{
  stringstream ss;
  string       val;

  ss << dest_.no_get ();
  ss >> val;
  os << inf_.path_get () << " -> " << "c" << val << "[color=red]" << ";" << endl;
}

void Minicol::adapt_action (Action* action)
{
	double new_angle = mean_angles (action_->angle_get(), action->angle_get());
//	cout << "update action ";
//	cout << src_.no_get() << "->" << dest_.no_get() << " : " << action_->angle_get() << " + ";
	action_->angle_set(new_angle);
//	cout << action->angle_get() << " = " << action_->angle_get() << endl; 
}

void Minicol::update_value ()
{
	static const double MEAN_PERIOD = params->get_double ("MEAN_PERIOD");
	double factor = 1 / MEAN_PERIOD;
	mean_val_ = mean_value (mean_val_, inf_.output (), factor); 
}

  
