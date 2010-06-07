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
//#include <numeric>
#include <algorithm>
#include <cmath>

Minicol::Minicol (Columns& columns, int no) :
	no_(no), sup_(columns.add_neuron_max (SUP)), inf_(columns.add_neuron_max (INF)), 
	action_(0), recruited_(false)
{
	stringstream s1, s2;
	s1 << inf_.no_get ()+1 << " " << no+1;
	Logger::log("minicol", s1.str());
	s2 << sup_.no_get ()+1 << " " << no+1;
	Logger::log("minicol", s2.str());
}

Minicol::~Minicol ()
{
	if (action_) {
		delete action_;
	}
}

void Minicol::new_set (const Action& action, Column& src, Column& dest)
{
	src_ = &src;
	dest_ = &dest;
	action_ = new Action (action);
	recruited_ = true;
	
	// intra-colomn connections
	inf_.add_synapse (src.state_get (), 1);
	inf_.add_synapse (src.inf_get (), 1);
	inf_.add_synapse_mult (sup_, 1);
	src.sup_get ().add_synapse (sup_, 1.0);
	
	stringstream s;
	s << no_+1 << " " << src.no_get ()+1;
	Logger::log("minicol_col", s.str());
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

void Minicol::synch()
{
	inf_.update_recent();
}

void Minicol::lateral_learning (bool increase, double factor)
{
	static const double LATERAL_LTP = Params::get_double("LATERAL_LTP");
	static const double LATERAL_LTD = Params::get_double("LATERAL_LTD");
	static const double MAX_LATERAL_WEIGHT = Params::get_double("MAX_LATERAL_WEIGHT");
		
	double* forw = dest_->inf_get ().syn_get (inf_);
	double* back = sup_.syn_get (dest_->sup_get ());
	if (forw == 0 || back == 0) {
		// on recrute une synapse
//		double init_val = MAX_LATERAL_WEIGHT;
		double init_val = LATERAL_LTP ;
//		forw = dest_->inf_get ().add_synapse (inf_, init_val);
		forw = dest_->inf_get ().add_synapse (inf_, 1);
		back = sup_.add_synapse (dest_->sup_get (), init_val);
	}

	// on modifie les synapses forw & back
	double valb;
	if (increase) {
//		valf = (MAX_LATERAL_WEIGHT - *forw) * LATERAL_LTP;
		valb = (MAX_LATERAL_WEIGHT - *back) * LATERAL_LTP;
//		valf = 2* (MAX_LATERAL_WEIGHT - *forw) * src_->lastT_recent () * dest_->lastT_recent ();
//		valb = 2* (MAX_LATERAL_WEIGHT - *back) * src_->lastT_recent () * dest_->lastT_recent ();
	}
	else {
//		valf = *forw * -LATERAL_LTD;
		valb = *back * -LATERAL_LTD * factor;
	}
//	cout << "old " << *forw << " " << *back << " // ";
//	*forw = *forw + valf;
	*back = *back + valb;
//	cout << "new " << *forw << " " << *back << endl;
}

double Minicol::lastT_recent () const
{
//	double total = accumulate (inf_.lastT_recent().begin(), inf_.lastT_recent().end(), 0.0);
//	// on retire l'activité moyenne pendant le creux, et on moyenne sur les peaks 
//	return (total - NB_STEP/2 * 0.05) / (NB_STEP/2);
	return * max_element (inf_.lastT_recent().begin(), inf_.lastT_recent().end());
}
