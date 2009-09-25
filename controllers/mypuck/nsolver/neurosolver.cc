#include <iostream>
#include "neurosolver.hh"
#include "action.hh"
#include "behavior.hh"
#include "device.hh"
#include "math.hh"
#include "params.hh"
#include "mystr.hh"


extern Params* params; 

Action* Neurosolver::planning () 
{
	// attend la stabilisation du réseau de planification
	colomns_.propagate ();
	Action * a = best_action ();
	return a;
}

Action* Neurosolver::best_action ()
{
	// TODO: utiliser une somme ponderee (ou autre) pour combiner
	// les reponses des colonnes voisines.
	Colomn*  best_col = colomns_.best_state_col (0);
	Minicol* best_minicol = 0;	
	if (best_col && best_col->actived()) {
		best_minicol = best_col->best_mean_minicol ();		
		if (best_minicol) {
			//cout << best_minicol->activation() << endl; 
			return best_minicol->action_get ().clone ();
		}
    }
    return 0;
}


void Neurosolver::learn_goal_position (RobotDevice& robot)
{
	static const double GOAL_ACTIVITY = params->get_double ("GOAL_ACTIVITY");
	
	if (robot.goal_reached () != robot.goal_reached_prec ()) {
		if (robot.goal_reached ()) {			
			Colomn* col = cols_get().best_state_col(0);
			if (col) {
//				col->sup_get ().add_synapse(motivation_, 1, true)
				col->goal_activity_set (GOAL_ACTIVITY);
			}
		}
	}
}


string Neurosolver::learning_or_planing (RobotDevice& robot, int nb_goal_reached)
{
	string message = "";
	
	if (robot.manually_moved ()) {
		if (nb_goal_reached == 3) {
			// on apprend le lien cellules lieux-colonnes après le passage dans chaque bras
	    	message = "apprentissage lieux-colonnes actif";
			learn_set (true);
			// On désactive pour l'instant l'apprentissage hebbien : one-shot learning
//		  	cols_get().learning_set (true);
	    	// on stoppe l'ajout de cellules de lieux
	    	hippo_get().iadd_set (false);
	    	
	    	robot.all_ways_visited(true);
		}
		else if (nb_goal_reached > 3) {
			if (!explo_done_) {
				explo_done_ = robot.all_ways_visited();
				// on veut visiter 2 fois chaque bras
				// a remplacer par une mesure de la qualite de la representation
				if (explo_done_) {
					// on bloque tout apprentissage
					message = "apprentissage termine";
					learn_set (false);
					cols_get().mode_set(PLANNING);	
					cols_get().learning_set (false);
				}	
			}
		}
	}
	return message;
}

bool Neurosolver::synch (RobotDevice& robot, bool go_random, int nb_goal_reached, string & message)
{	
	cout << endl << "== STEP ==" << endl;
	
	// on apprend la colonne de but
	learn_goal_position (robot);
	
	string tmp = learning_or_planing(robot, nb_goal_reached);
	
	// hippo
	state_.synch (robot.position_get ());
	
	// Pop ego
	ego_pop_.synch (last_orientation_, robot.angle_get()); 
	
	// Pop allo
	allo_pop_.synch (last_orientation_); 

  	// colonnes
  	colomns_.synch ();
  	
	//cout << "update current_orientation : " << last_orientation_ << " + " << robot.angle_get();
	last_orientation_ = mean_angles (last_orientation_, robot.angle_get());
	//cout << " = " << last_orientation_ << endl; 
  	
  	Colomn* current = colomns_.best_state_col (0);
  	if (learn_ && colomns_.mode_get () == LEARNING) {
  		
      	// apprentissage d'une nouvelle colonne lvl0 si besoin
      	if (!current || !current->actived ()) {
			current = &(colomns_.add_colomn (0, state_.pop_get (), true));
//			colomns_.synch ();
			//cout << "nouvelle colonne :" << current->no_get() << endl;
      	}
  	
	  	// apprentissage d'une nouvelle colonne lvl1 si besoin
	  	static const double NEURON_SPIKING_THRESH = params->get_double("NEURON_SPIKING_THRESH");
	  	Colomn*  current_lvl1 = colomns_.best_state_col (1);
	  	vector<ComputeUnit*> pop = ego_pop_.pop_get ();
	  	ComputeUnit* unit = 0;
	  	
//	  	if (learn_ && colomns_.mode_get () == LEARNING && colomns_.size () > 0 
	  	if (ego_pop_.pop_get ().at (1)->output () > NEURON_SPIKING_THRESH && nb_goal_reached >= 6) {
	  		if (current_lvl1) {	
		  		printf("best: (0) %d %f (1) %d %f\n",	current->state_get ().no_col_get(),
		  												current->state_get ().output(), 
				  										current_lvl1->state_get ().no_col_get(),
				  										current_lvl1->state_get ().output());
	  		}
	  		
//	  		if (!current_lvl1 || current_lvl1->state_get ().output () < current->state_get ().output()) {
			// Plutot que d'utiliser un seuil constant, on utilise un seuil basé sur l'activité de la couche du dessous
	      	if (!current_lvl1 || current_lvl1->state_get ().output () < NEURON_SPIKING_THRESH) {
	      		cout << endl << "nouvelle big colonne" << endl; 
				current_lvl1 = &(colomns_.add_colomn (1, colomns_.pop_get (), false));
//				unit = pop.at (0);
//				current_lvl1->state_get().add_synapse (*unit, -0.2, true);
//				unit = pop.at (1);
//				current_lvl1->state_get().add_synapse (*unit, 0.9, true);
//				unit = pop.at (2);
//				current_lvl1->state_get().add_synapse (*unit, -0.2, true);
	      	}
	      	
	      	printf("inhib sauf %d\n", current_lvl1->no_get ());
	      	colomns_.winner_set (current_lvl1->no_get ());
//	      	colomns_.synch ();
	      	prec_lvl1_ = current_lvl1;
	  	}
	  	else if ((ego_pop_.pop_get ().at (0)->output () > NEURON_SPIKING_THRESH
	  				|| ego_pop_.pop_get ().at (2)->output () > NEURON_SPIKING_THRESH)
	  				&& nb_goal_reached >= 6) {
	  		printf("virage\n");
	  		colomns_.winner_reset ();
//			colomns_.synch ();	
	  	}
  	}
//	colomns_.show_activities (0);
	colomns_.show_activities (1);
  	
  	message = "";
  	bool col_changed = false;
  	if (current) {
	    if (prec_lvl0_ && prec_lvl0_ != current) {
	    	col_changed = true;
	    	// on recupere l'orientation actuelle du robot
			// en faisant l'hypothese que c'est cette orientation qui nous
	      	// a amene de la col prec a la col suivante

	      	Action*  action = new Action(last_orientation_);
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
		Colomn*  current = colomns_.best_state_col (0);
		if (!current) {
			return message;	
		}
		Action a (last_orientation_);
		Minicol* real_minicol = current->minicol_get (a);
		if (real_minicol) {
	  		// on s'est cogne a un mur alors qu'on pouvait faire la transition
	  		message += " wrong " + i2str (real_minicol->from_get().no_get())
	  				+ " -> " + i2str (real_minicol->to_get().no_get());
	  		string m = "";
	  		colomns_.lateral_learning (real_minicol->from_get(), real_minicol->to_get(), &(real_minicol->action_get()), false, m);
	  		correction_done_ = true;
		}
		else {
			//cout << "pas de minicol associée" << endl;	
		}
	}
  	return message;
}

void Neurosolver::draw (ostream& os)
{
  os << "digraph G {" << endl;
  state_.draw (os);
  colomns_.draw (os);
  colomns_.draw_links (os);
  os << "}" << endl;
}

void Neurosolver::reset ()
{
	colomns_.reset ();
  	state_.reset ();
  	prec_lvl0_ = 0;
  	prec_lvl1_ = 0;
}
