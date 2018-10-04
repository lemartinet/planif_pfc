#include "minicol.hh"
#include "columns.hh"
#include "column.hh"
#include "action.hh"
#include "params.hh"
#include "math.hh"
#include "logger.hh"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

Minicol::Minicol (Columns& columns, int no) :
	no_(no), sup_(columns.add_neuron_max (SUP)), inf_(columns.add_neuron_max (INF)), 
	action_(0), mean_val_(inf_.output()), recruited_(false)
{
	stringstream s1, s2;
	s1 << inf_.no_get () << " " << no;
	Logger::log("minicol", s1.str());
	s2 << sup_.no_get () << " " << no;
	Logger::log("minicol", s2.str());
}

Minicol::~Minicol ()
{
	if (action_) {
		delete action_;
	}
}

void Minicol::new_set (const Action& action, Column& src, Column& dest, int level)
{
	src_ = &src;
	dest_ = &dest;
	action_ = new Action (action);
	level_ = level;
	recruited_ = true;
	
	// intra-colomn connections
	inf_.add_synapse (src.state_get (), sup_, 1.0, 0, 1);
	inf_.add_synapse (src.inf_get (), sup_, 1.0, 0, 1);
	src.sup_get ().add_synapse (sup_, 1.0, true);
	
  	// Inter-columns connections
  	lateral_learning (true);
  	
	stringstream s;
	s << no_ << " " << src.no_get ();
	Logger::log("minicol_col", s.str());
}

void Minicol::draw (ostream& os) const
{
  os << "{<n" << sup_.no_get () << "> s" << no_ << " | <n" << inf_.no_get () << "> i" << no_ << "}";
}

void Minicol::adapt_action (const Action& action)
{
	return;
	
	double new_angle = mean_angles (action_->angle_get(), action.angle_get());
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

void Minicol::lateral_learning (bool increase)
{
	static const double LATERAL_LEARNING_STEP = Params::get_double("LATERAL_LEARNING_STEP");
	static const double MAX_LATERAL_WEIGHT = Params::get_double("MAX_LATERAL_WEIGHT");
		
	Synapse* forw = dest_->inf_get ().syn_get (inf_);
	Synapse* back = sup_.syn_get (dest_->sup_get ());
	if (forw == 0 || back == 0) {
		// on recrute une synapse
//		double init_val = MAX_LATERAL_WEIGHT;
		double init_val = 0.1 * drand ();
		forw = dest_->inf_get ().add_synapse (inf_, init_val, true);
		back = sup_.add_synapse (dest_->sup_get (), init_val, true);
	}

	// on modifie les synapses forw & back
	double valf, valb;
	if (increase) {
//		valf = MAX_LATERAL_WEIGHT - forw->w_get ();
//		valb = MAX_LATERAL_WEIGHT - back->w_get ();
//		valf = (MAX_LATERAL_WEIGHT - forw->w_get ()) * src_->state_activation() * dest_->state_activation();
//		valb = (MAX_LATERAL_WEIGHT - back->w_get ()) * src_->state_activation() * dest_->state_activation();
		valf = (MAX_LATERAL_WEIGHT > forw->w_get () ? 1 : 0) * src_->state_activation() * dest_->state_activation();
		valb = (MAX_LATERAL_WEIGHT > back->w_get () ? 1 : 0) * src_->state_activation() * dest_->state_activation();
	}
	else {
		valf = forw->w_get () * -LATERAL_LEARNING_STEP;
		valb = back->w_get () * -LATERAL_LEARNING_STEP;
	}
	cout << "old " << forw->w_get () << " " << back->w_get () << " // ";
	forw->w_set (forw->w_get () + valf);
	back->w_set (back->w_get () + valb);
	cout << "new " << forw->w_get () << " " << back->w_get () << endl;
}
