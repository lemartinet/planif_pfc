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

#define NB_STEP 8

Minicol::Minicol (Columns& columns, int no) :
	no_(no), sup_(columns.add_neuron_max (SUP)), inf_(columns.add_neuron_max (INF)), 
	action_(0), recruited_(false), lastTidx_(0), from0_(0), to0_(0), increase_lvl1_(true)
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

void Minicol::new_set (const Action& action, Column& src, Column& dest, int level, Neuron* from0, Neuron* to0)
{
	src_ = &src;
	dest_ = &dest;
	action_ = new Action (action);
	level_ = level;
	recruited_ = true;
	
	// intra-colomn connections
	inf_.add_synapse (src.state_get (), 1);
	inf_.add_synapse (src.inf_get (), 1);
	inf_.add_synapse_mult (sup_, 1);
	src.sup_get ().add_synapse (sup_, 1.0);
	
	stringstream s;
	s << no_+1 << " " << src.no_get ()+1;
	Logger::log("minicol_col", s.str());

	if (level == 1) {
		from0_ = from0;
		to0_ = to0;
	}
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
	if (level_ == 1 && recruited_) {
//	  	cout << from0_->output() << " " << to0_->output() << endl;
	  	lateral_learning_lvl1();
	}

	// on garde un historique de l'activation sur X time steps
	if (lastT_.size () < NB_STEP) {
		lastT_.push_back (inf_.output ());
	}
	else {
		lastT_[lastTidx_] = inf_.output ();
		lastTidx_ = ++lastTidx_ % NB_STEP;
	}
}

void Minicol::lateral_learning_lvl0 (bool increase, double factor)
{
	static const double LATERAL_LEARNING_STEP = Params::get_double("LATERAL_LEARNING_STEP");
	static const double MAX_LATERAL_WEIGHT = Params::get_double("MAX_LATERAL_WEIGHT");
		
	double* forw = dest_->inf_get ().syn_get (inf_);
	double* back = sup_.syn_get (dest_->sup_get ());
	if (forw == 0 || back == 0) {
		// on recrute une synapse
		double init_val = MAX_LATERAL_WEIGHT;
//		double init_val = 0.1 * drand ();
		forw = dest_->inf_get ().add_synapse (inf_, init_val);
		back = sup_.add_synapse (dest_->sup_get (), init_val);
	}

	// on modifie les synapses forw & back
	double valf, valb;
	if (increase) {
//		valf = (MAX_LATERAL_WEIGHT - *forw) * LATERAL_LEARNING_STEP * factor;
//		valb = (MAX_LATERAL_WEIGHT - *back) * LATERAL_LEARNING_STEP * factor;
		valf = (MAX_LATERAL_WEIGHT - *forw) * factor;
		valb = (MAX_LATERAL_WEIGHT - *back) * factor;
//		valf = 2* (MAX_LATERAL_WEIGHT - forw->w_get ()) * src_->lastT_recent () * dest_->lastT_recent ();
//		valb = 2* (MAX_LATERAL_WEIGHT - back->w_get ()) * src_->lastT_recent () * dest_->lastT_recent ();
	}
	else {
		valf = *forw * -LATERAL_LEARNING_STEP * factor;
		valb = *back * -LATERAL_LEARNING_STEP * factor;
	}
//	cout << "old " << forw->w_get () << " " << back->w_get () << " // ";
	*forw = *forw + valf;
	*back = *back + valb;
//	cout << "new " << forw->w_get () << " " << back->w_get () << endl;
}

void Minicol::lateral_learning_lvl1()
{
	// calcul du nb de synapse lvl0 au sein d'une colonne lvl1
	// le +1 est parce qu'on prend le sup de to0 (car il est pas biaisé)
	// alors qu'il faudrait le sup de from0
	int nb_syn_back = log(to0_->output()) / log(0.9) + 1;
	int nb_syn_from = log(from0_->output()) / log(0.9) + 1;
	// réajustement de la force de propagation
	double back_out = pow(1 * 0.9 * pow(1 / 0.9, 0.8), nb_syn_back);
	double to_out = pow(1 * 0.9 * pow(1 / 0.9, 0.8), nb_syn_from);

	double* forw = dest_->inf_get ().syn_get (inf_);
	double* back = sup_.syn_get (dest_->sup_get ());
	if (forw == 0 || back == 0) {
		forw = dest_->inf_get ().add_synapse (inf_, to_out);
		back = sup_.add_synapse (dest_->sup_get (), back_out);
	} else if (increase_lvl1_){
		*forw = to_out;
		*back = back_out;
	} else {
		*forw = 0;
		*back = 0;
	}
}

double Minicol::lastT_recent () const
{
//	double total = accumulate (lastT_.begin(), lastT_.end(), 0.0);
//	// on retire l'activité moyenne pendant le creux, et on moyenne sur les peaks 
//	return (total - NB_STEP/2 * 0.05) / (NB_STEP/2);
	return * max_element (lastT_.begin(), lastT_.end());
}
