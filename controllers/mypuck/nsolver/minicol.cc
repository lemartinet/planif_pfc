#include "minicol.hh"
#include "neuralnet.hh"
#include "column.hh"
#include "params.hh"
#include "math.hh"
#include "logger.hh"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

Minicol::Minicol (Neuralnet& net, Action* action, Column& src, Column& dest, int no, int level) :
	no_(no), sup_(net.add_neuron_max ()), inf_(net.add_neuron_max ()), 
	action_(action), net_(net), src_(&src), dest_(&dest),
	mean_val_(inf_.output()), level_(level), recruited_(true)
{
	// intra-colomn connections
	inf_.add_synapse (src.state_get (), sup_, 1.0, 0, 1);
	inf_.add_synapse (src.inf_get (), sup_, 1.0, 0, 1);
	src.sup_get ().add_synapse (sup_, 1.0, true);
	
  	// Inter-columns connections
  	net_.lateral_learning (inf_, dest.inf_get (), true);
  	net_.lateral_learning (dest.sup_get (), sup_, true);
  	
  	Logger::add ("inf", &inf_);
  	Logger::add ("sup", &sup_);
}

Minicol::Minicol (Neuralnet& net, int no) :
	no_(no), sup_(net.add_neuron_max ()), inf_(net.add_neuron_max ()), 
	net_(net), mean_val_(inf_.output()), recruited_(false)
{
  	Logger::add ("inf", &inf_);
  	Logger::add ("sup", &sup_);	
}

Minicol::~Minicol ()
{
	if (action_) {
		delete action_;
	}
}

void Minicol::new_set (Action* action, Column& src, Column& dest, int level)
{
	src_ = &src;
	dest_ = &dest;
	action_ = action;
	level_ = level;
	recruited_ = true;
	
	// intra-colomn connections
	inf_.add_synapse (src.state_get (), sup_, 1.0, 0, 1);
	inf_.add_synapse (src.inf_get (), sup_, 1.0, 0, 1);
	src.sup_get ().add_synapse (sup_, 1.0, true);
	
  	// Inter-columns connections
  	net_.lateral_learning (inf_, dest.inf_get (), true);
  	net_.lateral_learning (dest.sup_get (), sup_, true);
}

void Minicol::draw (ostream& os) const
{
  os << "{<n" << sup_.no_get () << "> s" << no_ << " | <n" << inf_.no_get () << "> i" << no_ << "}";
}

void Minicol::adapt_action (Action* action)
{
	return;
	
	double new_angle = mean_angles (action_->angle_get(), action->angle_get());
//	cout << "update action ";
//	cout << src_.no_get() << "->" << dest_.no_get() << " : " << action_->angle_get() << " + ";
	action_->angle_set(new_angle);
//	cout << action->angle_get() << " = " << action_->angle_get() << endl; 
}

void Minicol::update_value ()
{
	static const double MEAN_PERIOD = Params::get_double ("MEAN_PERIOD");
	double factor = 1 / MEAN_PERIOD;
	mean_val_ = mean_value (mean_val_, inf_.output (), factor); 
}

