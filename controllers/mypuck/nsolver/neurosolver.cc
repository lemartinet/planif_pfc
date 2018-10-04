#include <iostream>
#include "neurosolver.hh"
#include "action.hh"
#include "behavior.hh"
#include "device.hh"
#include "math.hh"
#include "params.hh"
#include "mystr.hh"
#include "column.hh"


extern Params* params; 

Neurosolver::Neurosolver (const RobotDevice& robot) : robot_(robot), colomns_(), hippo_(), 
	ego_pop_(), allo_pop_(), motivation_ (1, 0), learn_(false), explo_done_(false) 
{
}

Neurosolver::~Neurosolver () 
{
}

Action* Neurosolver::best_action () const
{
	Minicol* best_minicol = colomns_.best_minicol (0);	
	if (best_minicol) {// && best_minicol->spiking ()) {
//		cout << best_minicol->activation() << endl; 
		return new Action(best_minicol->action_get ());
	}
	return 0;
}


void Neurosolver::update_goal_position ()
{
	if (robot_.goal_reached () != robot_.goal_reached_prec ()) {
		if (robot_.goal_reached ()) {			
			learn_goal_position (cols_get().best_state_col(0));
		}
	}
	// ajouter la détection de l'absence d'un goal attendu
}

void Neurosolver::learn_goal_position (Column* col)
{
	if (!col || !col->spiking ()) {
		return;
	}
	if (is_goal_position (col)) {
		Synapse* s = col->sup_get ().syn_get(motivation_);
		s->w_set (1);
	}
	else {
		col->sup_get ().add_synapse(motivation_, 1, true);
	}		
}

void Neurosolver::unlearn_goal_position (Column* col)
{
	if (col && is_goal_position (col)) {
		Synapse* s = col->sup_get ().syn_get(motivation_);
		s->w_set (0);
	}			
}

bool Neurosolver::is_goal_position (Column* col)
{
	Synapse* s = col->sup_get ().syn_get (motivation_);
	return (s != 0) && (s->w_get() == 1);	
}

string Neurosolver::learning_or_planing (int nb_goal_reached)
{
	string message = "";
	
	if (robot_.manually_moved ()) {
		if (nb_goal_reached == 3) {
			// on apprend le lien cellules lieux-colonnes après le passage dans chaque bras
	    	message = "apprentissage lieux-colonnes actif";
			learn_set (true);
			// On désactive pour l'instant l'apprentissage hebbien : one-shot learning
//		  	cols_get().learning_set (true);
	    	// on stoppe l'ajout de cellules de lieux
	    	hippo_.iadd_set (false);
	    	
	    	const_cast<RobotDevice&>(robot_).all_ways_visited(true);
		}
		else if (nb_goal_reached > 3) {
			if (!explo_done_) {
				explo_done_ = const_cast<RobotDevice&>(robot_).all_ways_visited();
				// on veut visiter 2 fois chaque bras
				// a remplacer par une mesure de la qualite de la representation
				if (explo_done_) {
					// on bloque tout apprentissage
					message = "apprentissage termine";
					learn_set (false);
					colomns_.learning_set (false);
				}	
			}
		}
	}
	return message;
}

bool Neurosolver::synch (int nb_goal_reached, string & message)
{	
	cout << endl << "== STEP ==" << endl;
	
	// on apprend la colonne de but
	update_goal_position ();
	
	string tmp = learning_or_planing(nb_goal_reached);
	
	hippo_.synch (robot_.position_get ());
	ego_pop_.synch (robot_.angle_get()); 
	allo_pop_.synch (robot_.angle_get()); 
  	colomns_.synch ();
  	
	if (robot_.manually_moved ()) {
		// on vient de bouger le robot, on apprend rien pendant un pas de temps
		// permet de virer le lien entre fin de P1 et début de P1
  		prec_lvl0_ = 0;
  		prec_lvl1_ = 0;
  		return true;
  	}
  	
  	Column* current = colomns_.best_state_col (0);
  	Column*  current_lvl1 = colomns_.best_state_col (1);
//  	if (hippo_.nb_spiking_cells () >= 6) {
  	if (learn_) {
  		
      	// apprentissage d'une nouvelle colonne lvl0 si besoin
      	if (!current || !current->spiking ()) {
			current = &(colomns_.add_colomn (0, hippo_.pop_get (), true, 0));
			//cout << "nouvelle colonne :" << current->no_get() << endl;
      	}
  	
	  	// apprentissage d'une nouvelle colonne lvl1 si besoin
	  	vector<ComputeUnit*> pop = ego_pop_.pop_get ();
	  	ComputeUnit* unit = 0;
	  	unit = pop.at (1);
	  	
	  	if (nb_goal_reached >= 6) {
//		if (colomns_.nb_spiking_cells (0) > 1.3) { Ca marche pas mal comme ça, 1.3 = .7 + .3 + .3 par ex
	  		if (!current_lvl1 || !current_lvl1->state_get ().spiking ()) {
	      		if (unit->spiking ()) {
		      		cout << endl << "nouvelle big colonne" << endl; 
					current_lvl1 = &(colomns_.add_colomn (1, colomns_.pop_get (), false, unit));
			      	prec_lvl1_ = current_lvl1;
	      		}
	      	}
	  	}
  	}
  	
  	if (current && current_lvl1) {	
  		printf("best: (0) %d %f (1) %d %f\n",	current->state_get ().no_col_get(),
  												current->state_get ().output(), 
		  										current_lvl1->state_get ().no_col_get(),
		  										current_lvl1->state_get ().output());
	}
//	colomns_.show_activities (0);
	colomns_.show_activities (1);
  	
  	message = "";
  	bool col_changed = false;
// revoir ça par pas logique : si on recrute c effectivement que current n'est plus activée
//  	if (current && current->spiking ()) { 
		if (current) {
	    if (prec_lvl0_ && prec_lvl0_ != current) {
	    	col_changed = true;
	    	// on recupere l'orientation actuelle du robot
			// en faisant l'hypothese que c'est cette orientation qui nous
	      	// a amene de la col prec a la col suivante

	      	Action*  action = new Action(robot_.angle_get());
	      	colomns_.lateral_learning (*prec_lvl0_, *current, action, true, message);
	      	
//	      	if (previous_minicol_) {
//	      		cout << "C " << current->no_get() << " P " << previous_minicol_->to_get().no_get() << endl;
//	      	}
//	      	if (!go_random && previous_predicted_minicol_ && previous_predicted_minicol_->to_get().no_get() != current->no_get()) {
//	      		// on avait pas correctement prédit la transition
//	      		cout << "prediction col" << endl;
//	      		message += "correct " + i2str (prec_->no_get()) 
//	      				+ " -> " + i2str (current->no_get())
//	      				+ " wrong " + i2str (prec_->no_get())
//	      				+ " -> " + i2str (previous_predicted_minicol_->to_get().no_get());
//	      		colomns_.lateral_learning (*prec_, previous_predicted_minicol_->to_get(), &(previous_predicted_minicol_->action_get()), false);
//	      	}
//	      	previous_predicted_minicol_ = current->best_mean_minicol ();
	  	}
	    prec_lvl0_ = current;
  	}
  	else {
  		prec_lvl0_ = 0;	
  	}
  	
  	if (tmp != "") {
  		message += "\n";
  		message += tmp;
  	}
  	return col_changed;
}

string Neurosolver::correct_transition (bool bloque)
{
	string message="";
	if (!bloque) {
		correction_done_ = false;
	}
	else if (!correction_done_) {
		Column*  current = colomns_.best_state_col (0);
		if (!current) {
			return message;	
		}
		Action a (robot_.angle_get ());
		Minicol* real_minicol = current->minicol_get (a);
		if (real_minicol) {
	  		// on s'est cogne a un mur alors qu'on pouvait faire la transition
	  		message += " wrong " + i2str (real_minicol->from_get().no_get())
	  				+ " -> " + i2str (real_minicol->to_get().no_get());
	  		string m = "";
	  		colomns_.lateral_learning (const_cast<Column&>(real_minicol->from_get()), 
	  									real_minicol->to_get(), 
	  									&(real_minicol->action_get()), false, m);
	  		correction_done_ = true;
		}
		else {
			//cout << "pas de minicol associée" << endl;	
		}
	}
  	return message;
}

void Neurosolver::draw (ostream& os) const
{
  os << "digraph G {" << endl;
  hippo_.draw (os);
  colomns_.draw (os);
  colomns_.draw_links (os);
  os << "}" << endl;
}
