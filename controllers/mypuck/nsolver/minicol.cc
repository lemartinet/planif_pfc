#include "minicol.hh"
#include "neuralnet.hh"
#include "column.hh"
#include "params.hh"
#include "math.hh"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

Minicol::Minicol (Neuralnet& net, Action* action, const Column& src, const Column& dest, 
					int no_col, int no, int level) :
	no_(no), sup_(net.add_neuron_max ( no_col, level)),
	inf_(net.add_neuron_max (no_col, level)), 
	action_(action), net_(net), src_(src), dest_(dest),
	mean_val_(inf_.output()), level_(level), log_("minicols", src.no_get (), dest.no_get ())
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
	if (action_) {
		delete action_;
	}
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

void Minicol::log (const string& time_string, const Coord& position, double angle, int day, int trial)
{
	ostringstream msg;
	msg << "# " << time_string << endl;
	msg << position.x_get () << " " << position.y_get () << " " << angle << " "
  				<< inf_.output () << " " << sup_.output () << endl << endl;
  	log_.log (day, trial, msg.str ());
}
