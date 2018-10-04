#include "behavior.hh"
#include <iostream>
#include <fstream>
#include "params.hh"
#include "math.hh"
#include "mystr.hh"
#include "action.hh"

#define WAIT_BETWEEN_DECISIONS 10

extern Params* params;
ofstream * output_mypuck;
ofstream * output_neurosolver;


Behavior::Behavior () : cpt_(0), robot_(), neurosolver_(robot_), left_near_(false), 
		right_near_(false), action_done_(true), wait_(0), nb_goal_reached_(0), nb_free_(0), dirs_(0), 
		go_random_(false)
{
	const int SIMULATION_ID = params->get_int ("SIMULATION_ID");
	 
	current_ = new Action(0.0);
  	
  	string filename ("../../data/data_raw/");
	filename += i2str (SIMULATION_ID);
	filename += "/output_mypuck.txt";
  	output_mypuck = new ofstream (filename.c_str ());
  	
  	  
  	filename = "../../data/data_raw/";
	filename += i2str (SIMULATION_ID);
	filename += "/output_neurosolver.txt";
  	output_neurosolver = new ofstream (filename.c_str ());
}

Behavior::~Behavior () 
{
	if (current_) {
		delete current_;
	}
}

void Behavior::write_message (ofstream* file, string message)
{
	const int TIME_STEP = params->get_int ("TIME_STEP");
	
	// il y a 12 essais par jour
	int day = 1 + nb_goal_reached_ / 12;
	int trial = 1 + nb_goal_reached_ % 12;
	*file << "Day " << day << " Trial " << trial 
			<< " Time " << cpt_ * TIME_STEP / 1000.0 
			<< "s : " << message << endl;
}

double Behavior::e_greedy () {
	return 0.9 * exp (-0.1 * (nb_goal_reached_ - 3)) + 0.1;
}

void Behavior::do_action ()
{
	static const int RANDOM_MOVE = params->get_int("RANDOM_MOVE");
  	if (RANDOM_MOVE && nb_goal_reached_ > 5) {
  		robot_.speed_set(10,10);
  		return;
  	} 
  	
	double angle = current_->angle_get();
	double diff = ecart_angulaire (robot_.angle_get (), angle);
	static const double ANGLE_THRESH = params->get_double ("ANGLE_THRESH");
	if (!action_done_) {
  		action_done_ = (fabs (diff) < ANGLE_THRESH)?true:false;
	}
	if (action_done_) {
		diff = 0;
	}
	bool bloque = avoid_.obstacle_avoidance_module(diff, robot_.position_get ());
	string message = neurosolver_.correct_transition (bloque);
	if (message != "") {
		write_message (output_neurosolver, message);
	}
}

int Behavior::analyse_cross_road (bool& left, bool& straight, bool& right)
{
	// fonction adaptée au laby de tolman. 
	
	// on analyse les alentours du robot
	// le test > 0 est pour supprimer l'artefact de debut de simulation
	if (avoid_.obstacle_left_mid_get () > 0 && avoid_.obstacle_left_mid_get () < 500) {
		left_near_ = true;
	} 
	else if (avoid_.obstacle_left_mid_get () > 2000) {
		left_near_ = false;
	}		
	if (avoid_.obstacle_right_mid_get () > 0 && avoid_.obstacle_right_mid_get () < 500) {
		right_near_ = true;
	}
	else if (avoid_.obstacle_right_mid_get () > 2000) {
		right_near_ = false;
	}
	bool left_reached = false;
	if (avoid_.obstacle_left_get () > 0 && avoid_.obstacle_left_get ()  < 500) {
		left_reached = true;
	}
	bool right_reached = false;
	if (avoid_.obstacle_right_get () > 0 && avoid_.obstacle_right_get ()  < 500) {
		right_reached = true;
	}
	bool front_reached = false;
	//if (robot_.obstacle_left_front_get() > 2000 && robot_.obstacle_right_front_get()) {
	if (avoid_.obstacle_left_front_get() > 1500 && avoid_.obstacle_right_front_get()) {
		front_reached = true;
	}
	//cout << robot_.obstacle_left_front_get() << endl;

	left = straight = right = false;
	// Quel type d'intersection ?
	if (left_near_ && left_reached) {
		if (right_near_) {
			if (right_reached) {
				if (!front_reached) {
					// on a 3 chemins (g,d,td)
					//cout << "on a 3 chemins (g,d,td)" << endl;
					left = straight = right = true;
					return 3;
				}
				else {
					// on a 2 chemins (g,d)
					//cout << "on a 2 chemins (g,d)" << endl;
					left = right = true;
					return 2;
				}
			}
			else {
				// il faut attendre encore le chemin d
				//cout << "il faut attendre encore le chemin d" << endl;
				return 1;
			}
		}
		else if (!front_reached) {
			// on a 2 chemins (g,td)
			//cout << "on a 2 chemins (g,td)" << endl;
			left = straight = true;
			return 2;
		}
		else {
			// on a 1 seul chemin, pas de décision
			//cout << "on a 1 seul chemin (g)" << endl;
			// on reinitialise la detection d'intersection
			left_near_ = false;
			return 1;	
		}
	}
	else if (right_near_ && right_reached) {
		if (left_near_ && !left_reached) {
			// il faut attendre encore le chemin g
			//cout << "il faut attendre encore le chemin g" << endl;
			return 1;
		}
		else if (!front_reached) {
			// on a 2 chemins (d,td)
			//cout << "on a 2 chemins (d,td)" << endl;
			right = straight = true;
			return 2;
		}
		else {
			// on a 1 seul chemin, pas de décision
			//cout << "on a 1 seul chemin (d)" << endl;
			// on reinitialise la detection d'intersection
			right_near_ = false;
			return 1;
		}
	}
	else if (!front_reached) {
			// on a 1 chemin (td)
			//cout << "on a 1 chemin (td)" << endl;
			straight = true;
			return 1;
	}
	return 0;	
} 

//int analyse_cross_road_bis ()
//{
//	// fonction adaptée au laby de tolman. 
//	
//	// on analyse les alentours du robot
//	// le test > 0 est pour supprimer l'artefact de debut de simulation
//	bool left_reached = false;
//	if (robot_.obstacle_left_get () > 0 && robot_.obstacle_left_get ()  < 500) {
//		left_reached = true;
//	}
//	bool right_reached = false;
//	if (robot_.obstacle_right_get () > 0 && robot_.obstacle_right_get ()  < 500) {
//		right_reached = true;
//	}
//	bool front_reached = false;
//	if (robot_.obstacle_left_front_get() > 1500 && robot_.obstacle_right_front_get()) {
//		front_reached = true;
//	}
//	
//	int count = 0;
//	count = left_reached ? count + 1: count;
//	count = right_reached ? count + 1: count;
//	count = !front_reached ? count + 1: count;
//	cout << "count :" << count << endl;
//	return count;	
//} 


void Behavior::free_ways ()
{
	// à modifier pour que plus généralement, elle serve à découvrir
	// les directions empruntables par le robot selon un échantillonnage
	// ici on regarde tous les 90° (gauche, tout droit et droite)
	// ajouter le demi-tour ???
	bool go_left = false, go_straight = false, go_right = false;
	nb_free_ = analyse_cross_road(go_left, go_straight, go_right);
	delete dirs_;
	dirs_ = new double[nb_free_];
	int index = 0;
	if (go_left) {
		dirs_[index++] = robot_.angle_get () + PI/2.0;
	}
	if (go_right) {
		dirs_[index++] = robot_.angle_get () - PI/2.0;
	}
	if (go_straight) {
		dirs_[index] = robot_.angle_get ();
	}
}

Action* Behavior::random_or_planif ()
{
	go_random_ = false;
	Action * action = 0;
	// on explore exponentiellement moins au cours des essais
	// de plus epsilon depend de la position ds le laby :
	// proche du but -> peu d'explo, loin -> beaucoup
	//double epsilon = e_greedy () * (robot_.distance_goal_factor());
	double epsilon = e_greedy ();
	// pour aider au cas ou le robot planifie mal au sein d'un essai : 
	// il explore de plus en plus après 5000 steps
	// espilon_help augmente exponetiellement de 5000->0 à 7000->1
	// ne depend pas de la position dans le laby comme epsilon
	double epsilon_help = (exp (0.001 * (cpt_ - 5000))-1)/6.4;
	epsilon_help = epsilon_help > 0 ? epsilon_help : 0;
//	cout << "epsilons :" << epsilon << " " << epsilon_help << endl;
	
	if (nb_goal_reached_ < 3 || drand() < epsilon + epsilon_help) {
		go_random_ = true;		
	}
	else {
		action = neurosolver_.best_action ();
		if (action) {
			string mess = "je planifie vers " + d2str (action->angle_get());
			write_message (output_mypuck, mess);
		}
		else {
			go_random_ = true;
			string mess = "pas d'action à planifier";
			write_message (output_mypuck, mess);
		}
	}
		
	if (go_random_) {
		// le robot explore	
		int r = (int)(drand() * nb_free_);
		action = new Action (dirs_[r]);
		string mess = "j'explore vers : " + d2str (action->angle_get());
		write_message (output_mypuck, mess);
	}
	
	return action;
}

void Behavior::compute_next_action ()
{
	avoid_.update_obstacle_info ();
	robot_.synch ();
	
	cpt_++;

	if (robot_.manually_moved ()) {
		string mess = "goal found " + i2str (1 + nb_goal_reached_);
		write_message(output_mypuck, mess);
		nb_goal_reached_++;
		cpt_ = 0;
	}

	string message;
	bool col_changed = neurosolver_.synch (nb_goal_reached_, message);
	if (message != "") {
		write_message (output_neurosolver, message);
	}
	if (col_changed) {
		// on stoppe l'action en cours quand on passe à une nouvelle colonne
		// à remplacer à terme par : on planifie à chaque colonne
		action_done_ = true;
	}
	
	// on regarde si l'action en cours est finie
	// et si on a attendu le tps min entre 2 décisions (persistance) 
	if (!action_done_ || wait_ > 0) {
		wait_--;
		return;
	}
	
	free_ways();
	
	Action*  action = 0;
	if (nb_free_ > 1) {
		action = random_or_planif ();
	}
	
    if (action) {
    	wait_ = WAIT_BETWEEN_DECISIONS;
    	delete current_; 
    	current_ = action;
    	action_done_ = false;
    }
}

void Behavior::synch () 
{
  	compute_next_action ();
  	do_action (); 	
}
