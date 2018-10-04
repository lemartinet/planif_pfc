#include "neurosolver.hh"
#include "action.hh"
#include "behavior.hh"
#include "device.hh"
#include "math.hh"
#include "params.hh"
#include "mystr.hh"
#include "column.hh"
#include "cell.hh"
#include "logger.hh"
#include "minicol.hh"
#include <iostream>
#include <sstream>
#include <algorithm>

Neurosolver::Neurosolver (const RobotDevice& robot) : 
	robot_(robot),  hippo_(), ego_pop_(), allo_pop_(), motivation_ (1), columns_(hippo_.pop_get ()),  
	prec_lvl0_(0), prec_lvl1_(0), learn_(false), explo_done_(false), no_learning_timer_(0)
{
}

Neurosolver::~Neurosolver () 
{
}

Action* Neurosolver::best_action () const
{
	Minicol* best_minicol = columns_.best_minicol (0);	
	if (best_minicol) {
//		cout << best_minicol->activation() << endl; 
		return &best_minicol->action_get ();
	}
	return 0;
}


void Neurosolver::goal_learning ()
{
	if (!robot_.goal_reached ()) {
		return;			
	}
	if (!current_lvl0_ || !current_lvl0_->spiking ()) {
		return;
	}
	set_goal_weight (current_lvl0_, 1);
	// ajouter la détection de l'absence d'un goal attendu
}

void Neurosolver::set_goal_weight (Column* col, double val)
{
	Synapse* s = col->sup_get ().syn_get (motivation_);
	if (s != 0) {
		s->w_set (val);
	}
	else {
		col->sup_get ().add_synapse (motivation_, val, true);
	}	
}

bool Neurosolver::is_goal_position (Column* col)
{
	Synapse* s = col->sup_get ().syn_get (motivation_);
	return (s != 0) && (s->w_get() > 0);	
}

bool Neurosolver::synch (bool sleep)
{
	if (robot_.manually_moved ()) {
		// on vient de bouger le robot, on apprend rien 
		// pendant no_learning_timer_ pas de temps
		// permet de virer le lien entre fin de P1 et début de P1
  		prec_lvl0_ = 0;
  		prec_lvl1_ = 0;
  		no_learning_timer_ = 1;
	}
	
	hippo_.synch (robot_.position_get ());
	ego_pop_.synch (robot_.angle_get()); 
	allo_pop_.synch (robot_.angle_get()); 
	
  	bool col_changed = false;
  	if (no_learning_timer_ > 0) {
//  		cout << "no learning" << endl;
  		no_learning_timer_--;
	  	columns_.synch (false, robot_.position_get (), hippo_.pop_get (), *ego_pop_.pop_get ().at (1));
  		col_changed = true;
  	}
  	else {
		// TODO: faut-il mettre learn_ ou true ???
	  	columns_.synch (true, robot_.position_get (), hippo_.pop_get (), *ego_pop_.pop_get ().at (1));
	  	if (robot_.cpt_total_get () % 1 == 0) {
			state_learning ();
	  		col_changed = topology_learning ();
			goal_learning ();
	  	}
  	}	
	Logger::log (robot_.cpt_total_get ());
  	return col_changed;
}

void Neurosolver::state_learning ()
{
	current_lvl0_ = columns_.best_state_col (0);
  	current_lvl1_ = columns_.best_state_col (1);
//  	if (hippo_.nb_spiking_cells () >= 6) {
  	if (learn_) {
//  		cout << "learning" << endl;
  		
      	// apprentissage d'une nouvelle colonne lvl0 si besoin
      	if (!current_lvl0_ || !current_lvl0_->spiking ()) {
			current_lvl0_ = &(columns_.add_colomn (0, hippo_.pop_get (), robot_.position_get ()));
			ostringstream message;
			message << "new_column_lvl0: " << current_lvl0_->no_get();
			Logger::log ("network", robot_.cpt_total_get (), message.str ());
			//cout << message.str ();
      	}
  	
	  	// apprentissage d'une nouvelle colonne lvl1 si besoin	  	
//		cout << "nb_col0 " << columns_.nb_spiking_cells (0) << endl;
		// ca marche pas mal avec 1.3 = .7 + .3 + .3 par ex
		static const double THRESH_ADD_LVL1 = Params::get_double("THRESH_ADD_LVL1");
		if (columns_.nb_spiking_cells (0) > THRESH_ADD_LVL1) {
	  		if (!current_lvl1_ || !current_lvl1_->state_get ().spiking ()) {
	  			ComputeUnit* unit = ego_pop_.pop_get ().at (1);
	      		if (unit->spiking ()) { 
					current_lvl1_ = &(columns_.add_colomn (1, *unit));
					ostringstream message;
					message << "new_column_lvl1: " << current_lvl1_->no_get();
					Logger::log ("network", robot_.cpt_total_get (), message.str ());
					//cout << message.str ();
	      		}
	      	}
	  	}
  	}
  	
//  	if (current_lvl0_ && current_lvl1_) {	
//  		cout << "best: (0) " << current_lvl0_->no_get() 
//  			 << " " << current_lvl0_->state_get ().output() 
//  			 << " (1) " << current_lvl1_->no_get() 
//  			 << " " << current_lvl1_->state_get ().output() << endl;
//	}
//	columns_.show_activities (0);
//	columns_.show_activities (1);
}

bool Neurosolver::topology_learning ()
{
  	stringstream message;
  	bool col_changed = false;

	if (!learn_) {
		return col_changed;
	}
	
	if (current_lvl0_) {
	    if (prec_lvl0_ && prec_lvl0_ != current_lvl0_) {
	    	col_changed = true;
	      	Action*  action = new Action(robot_.angle_get());
	      	columns_.lateral_learning (*prec_lvl0_, *current_lvl0_, action, true, message);
	  	}
  	}
	prec_lvl0_ = current_lvl0_;
	  	
  	if (current_lvl1_) {
  		if (prec_lvl1_ && prec_lvl1_ != current_lvl1_) {
  			columns_.lateral_learning (*prec_lvl1_, *current_lvl1_, 0, true, message);
  		}
	}
	prec_lvl1_ = current_lvl1_;
  	
  	if (message.str () != "") {
		Logger::log ("network", robot_.cpt_total_get (), message.str ());
	}
  	return col_changed;	
}

void Neurosolver::correct_transition (bool bloque)
{
	stringstream message;
	if (!bloque) {
		// cette variable est utilisee pour devaluer les poids une seule fois
		correction_done_ = false;
	}
	else if (!correction_done_) {
		if (!current_lvl0_) {
			return;	
		}
		Action a (robot_.angle_get ());
		Minicol* real_minicol = columns_.minicol_get (current_lvl0_->no_get (), a);
		if (real_minicol) {
	  		// on s'est cogne a un mur alors qu'on pouvait faire la transition
	  		message << "wrong " << real_minicol->from_get().no_get()
	  				<< "->" << real_minicol->to_get().no_get();
	  		stringstream m;
	  		columns_.lateral_learning (const_cast<Column&>(real_minicol->from_get()), 
	  									const_cast<Column&>(real_minicol->to_get()), 
	  									&(real_minicol->action_get()), false, m);
	  		correction_done_ = true;
		}
		else {
			//cout << "pas de minicol associée" << endl;	
		}
	}
	if (message.str () != "") {
		Logger::log ("network", robot_.cpt_total_get (), message.str ());
	}
}

void Neurosolver::draw (ostream& os) const
{
  os << "digraph G {" << endl;
  hippo_.draw (os);
  columns_.draw (os);
  os << "}" << endl;
}

double Neurosolver::inf_get (double angle) const
{
	if (current_lvl0_ == 0) {
		return 1;	
	}
	Action a (angle);
	vector<double> qvals;
	int nbcols = columns_.size ();
	// on regarde l'ensemble des états actifs pr calculer la qval
	for (int i = 0; i < nbcols; i++) {
		Minicol* m;
		if (columns_.col_get (i)->spiking () && (m = columns_.minicol_get(columns_.col_get (i)->no_get(), a))) {
			qvals.push_back (m->activation ());	
		}
	}
	if (qvals.size () > 0) {
		return *max_element (qvals.begin (), qvals.end ());
	}
	else {
		return -1;
	}
	
//	Minicol* m = current_lvl0_->minicol_get (a);
//	if (m != 0) {
//		return m->activation ();
//	}
//	else {
//		-1;
//	}
}

void Neurosolver::learn_set (bool learn) 
{
	learn_ = learn; 
	if (learn == true) { 
    	Logger::log ("network", robot_.cpt_total_get (), "apprentissage lieux-colonnes actif");
    	// on stoppe l'ajout de cellules de lieux
    	hippo_.iadd_set (false);
	} 
}
