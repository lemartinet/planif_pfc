#include <sstream>
#include <iostream>
#include "minicol.hh"
#include "column.hh"
#include "params.hh"
#include "math.hh"

extern Params* params;

Minicol::Minicol (Neuralnet& net, Action* action, Colomn& src, Colomn& dest, string& path, int no_col, int no, int level)
{
	static const double VR = params->get_double("VR");
  	static const double VF = params->get_double("VF");
  	path_ = path;
  	no_ = no;
  	sup_ = net.add_neuron_max (VR, VF, path_, no_col, level); //<- here
  	inf_ = net.add_neuron (VR, VF, path_, no_col, 200.0, 0.9, 8.0, -1.2, level); //<- hum
  	action_ = (action?action->clone ():action);
  	net_ = &net;
  	src_ = &src;
  	dest_ = &dest;
	mode_ = LEARNING;
	mean_val_ = inf_->output();
	level_ = level;
}

Minicol::~Minicol ()
{
  net_->del_neuron (sup_);
  net_->del_neuron (inf_);
  delete action_;
}

void Minicol::draw (ostream& os)
{
  os << "{<n" << sup_->no_get () << "> s" << no_ << " | <n" << inf_->no_get () << "> i" << no_ << "}";
}

void Minicol::draw_links (ostream& os)
{
  stringstream ss;
  string       val;

  ss << dest_->no_get ();
  ss >> val;
  os << inf_->path_get () << " -> " << "c" << val << "[color=red]" << ";" << endl;
}

double Minicol::goal_link_activation ()
{
  return sup_->syn_get (dest_->sup_get ())->drive ();
}

double Minicol::state_link_activation ()
{
  return dest_->inf_get ().syn_get (*inf_)->drive ();
}

void Minicol::mode_set (enum e_mode mode) 
{
	if (mode_ != mode) {
		mode_ = mode;
      	if (mode_ == LEARNING) {
        	net_->del_synapse (*sup_, *inf_);
      	}
      	else {
        	net_->add_synapse (*sup_, *inf_, 1.0, true);
      	}
    }
}

void Minicol::adapt_action (Action * action)
{
	double new_angle = mean_angles (action_->angle_get(), action->angle_get());
//	cout << "update action ";
//	cout << src_->no_get() << "->" << dest_->no_get() << " : " << action_->angle_get() << " + ";
	action_->angle_set(new_angle);
//	cout << action->angle_get() << " = " << action_->angle_get() << endl; 
}

void Minicol::update_value ()
{
	static const double MEAN_PERIOD = params->get_double ("MEAN_PERIOD");
	double factor = 1 / MEAN_PERIOD;
	mean_val_ = mean_value (mean_val_, inf_->output (), factor); 
}

  
