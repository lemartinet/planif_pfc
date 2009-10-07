#include "behavior.hh"
#include "params.hh"
#include "math.hh"
#include "mystr.hh"
#include "action.hh"
#include "logger.hh"
#include <iostream>

#define WAIT_BETWEEN_DECISIONS 48

Behavior::Behavior (): 
	robot_(), neurosolver_(), avoid_(robot_), current_(0.0), wait_(0), automate_(GO_ON)
{
}

Behavior::~Behavior () 
{
}

void Behavior::synch () 
{
  	compute_next_action ();
  	do_action (); 
}

void Behavior::compute_next_action ()
{
	static const int ONLY_INTERSECT = Params::get_int ("ONLY_INTERSECT");
	// update des capteurs et réseau de neurones
	robot_.synch ();
	vector<double> dirs;
	if (automate_ == SLEEP) {
		neurosolver_.sleep (wait_);
	}
	else {
		avoid_.free_ways (dirs, robot_.angle_get ());
		neurosolver_.synch (! robot_.manually_moved (), ONLY_INTERSECT == 1 ? automate_ == DECIDE : true);
	}
	
	// màj de l'automate
	static const int SLEEP_STEP = Params::get_int ("SLEEP_STEP");
	if (automate_ != SLEEP && SLEEP_STEP > 0 && robot_.sleep_get ()) {
		// on dort
		wait_ = SLEEP_STEP;
		automate_ = SLEEP;
	}
	else if (automate_ == SLEEP && wait_ == 0) {	
		automate_ = GO_ON;
	}
	else if (automate_ != GOAL && automate_ != SLEEP && robot_.goal_reached ()) {
		// on s'arrete au goal
		wait_ = 5 * WAIT_BETWEEN_DECISIONS;
		automate_ = GOAL;
		neurosolver_.goal_learning ();
	}
	else if (automate_ == GOAL && wait_ == 0) {	
		automate_ = GO_ON;
	}
	else if (automate_ != BLOCK && automate_ != SLEEP && avoid_.bloque_get ()) {
		const_cast<Columns&>(neurosolver_.cols_get ()).correct_transition ();
		// utiliser la meme valeur que le rythme d'appr dans neurosolver ?
		wait_ = 96;
		automate_ = BLOCK;
	}
	else if (automate_ == BLOCK && wait_ == 0) {
		automate_ = GO_ON;
	}	
	else if (automate_ == GO_ON && dirs.size () > 1) {
		// on attend avt de prendre la decision, le tps de la propagation
		// on prend une décision seulement aux intersections (pas ds couloir)
		wait_ = ONLY_INTERSECT == 1 ? 20 : 0; 
		automate_ = DECIDE;
	}
	else if (automate_ == DECIDE && wait_ == 0) {
		// on prend la décision
		automate_ = DECIDED;
		wait_ = WAIT_BETWEEN_DECISIONS;
		current_ = select_action (dirs);	
		//cout << "action :" << current_ << endl;
	}
	else if (automate_ == DECIDED && wait_ == 0) { //dirs.size () <= 1) {
		// on attend entre 2 décisions (persistance) 
		automate_ = GO_ON;
	}
	else if (wait_ > 0) {
		wait_--;	
	}
//	cout << automate_state () << endl;
}

void Behavior::do_action ()
{
	// revoir cette fonction (peut-être deplacee dans obstacleavoid ?)
	static const int RANDOM_MOVE = Params::get_int("RANDOM_MOVE");
	int left_speed, right_speed;
  	if (RANDOM_MOVE && robot_.nb_trial_get () > 6) {
  		left_speed = right_speed = 10;
  	}
	else if (automate_ == GOAL || automate_ == DECIDE || automate_ == SLEEP) {
		left_speed = right_speed = 0;
	}
	else {
		double diff = ecart_angulaire (robot_.angle_get (), current_);
		if (angle_equal (robot_.angle_get (), current_)) {
	  		diff = 0;
	  		current_ = robot_.angle_get ();
		}
		//cout << "diff " << diff;
		avoid_.avoid (diff, robot_.position_get (), left_speed, right_speed);
	}
	//cout << "left " << left_speed << " right " << right_speed << endl;
	robot_.setSpeed (left_speed, right_speed);
}


double Behavior::select_action (const vector<double>& dirs)
{
	int nb_free = dirs.size ();
	double pa[nb_free];
	stringstream s;
	static const string EXPLO = Params::get ("EXPLO");
	bool explore = false;
	
	// generation des pa (prob de select de l'action)
	if (EXPLO == "egreedy") {
		explore = e_greedy (dirs, pa, s);
	}
	else if (EXPLO == "qgreedy") {
		cout << EXPLO << endl;
		explore = q_greedy (dirs, pa);
	}
	else if (EXPLO == "soft") {
		explore = softmax (dirs, pa);
	}
//	for (int i = 0; i < nb_free; i++) {
//		cout << dirs[i] << "/" << pa[i] << " ";
//	}
//	cout << endl;

	// Lotterie pour la selection
	double rand_action = drand ();
	double sum_pa = 0;
	int i;
	for (i = 0; i < nb_free; i++) {
		sum_pa += pa[i];
		if (rand_action < sum_pa) {
			break;
		}	
	}
	// TODO: a verifier
//	bool explore = (neurosolver_.inf_get (dirs[i]) == -1);
	for (int j = 0; j < nb_free && !explore; j++) {
		if (pa[i] <= pa[j] && i != j) {
			explore = true;
		}	
	}
	s << (explore ? "0 " : "1 ") << dirs[i] << " " << pa[i];
	Logger::log ("decision", robot_.cpt_total_get (), s.str ());		
	return dirs[i];
}

bool Behavior::e_greedy (const vector<double>& dirs, double* pa, stringstream& s)
{
	// Mecanisme epsilon-greedy	
	// on explore exponentiellement moins au cours des essais
	double epsilon = 0.9 * exp (-0.1 * (robot_.nb_trial_get () - 3)) + 0.1;
	// epsilon peut dependre de la position ds laby (proche but = peu d'explo)
//	epsilon *= robot_.distance_goal_factor();
	// aide au cas d'une plannif mauvaise: explo augmente après 5000 steps
//	double epsilon_help = (exp (0.001 * (robot_.cpt_trial_get () - 5000))-1)/6.4;
//	epsilon_help = epsilon_help > 0 ? epsilon_help : 0;
	double epsilon_help = 0;
//	cout << "epsilons :" << epsilon << " " << epsilon_help << endl;
	
	Action* action = neurosolver_.best_action ();
	bool found = false;
	int nb_free = dirs.size ();
	double min = 4*PI;
	int min_d = -1;
	for (int i = 0; i < nb_free; i++) {
		// on cherche la meilleure dir existante
		if (action && pow2 (ecart_angulaire (dirs[i], action->angle_get())) < min) {
			found = true;
			min	= pow2 (ecart_angulaire (dirs[i], action->angle_get()));
			min_d = i;
		}
		pa[i] = 0;
	}
	if (min_d >= 0) {
		pa[min_d] = 1;
	}
	// selon epsilon, on explore ou exploite cette meilleure action
	if (drand() < epsilon + epsilon_help || !found) {
		if (!found) {
			s << "-";				
		}
		// le robot explore (dirs equiprobable)
		for (int i = 0; i < nb_free; i++) {
			pa[i] = 1.0 / nb_free;
//			cout << pa[i] << " ";
		}
//		cout << endl;
		return true;
	}
	return false;
}

bool Behavior::softmax (const vector<double>& dirs, double* pa)
{
	// Mecanisme softmax 
	int nb_free = dirs.size ();
	int explo_var = 20;
	double sum_norm = 0;
	for (int i = 0; i < nb_free; i++) {
		pa[i] = qval (dirs[i]);
		sum_norm += exp (explo_var * pa[i]);
	}
	for (int i = 0; i < nb_free; i++) {
		pa[i] = exp (explo_var * pa[i]) / sum_norm;
	}
	return false;
}

bool Behavior::q_greedy (const vector<double>& dirs, double* pa)
{
	// Mecanisme "q-greedy"
	int nb_free = dirs.size ();
	int explo_var = 10;
	int max_dir = 0;
	for (int i = 0; i < nb_free; i++) {
		pa[i] = qval (dirs[i]);
		if (pa[i] > pa[max_dir]) {
			max_dir = i;
		}
//		cout << pa[i] << "," << dirs[i] << " ";
	}
//	cout << endl;
	pa[max_dir] = 1 - exp (-explo_var * pa[max_dir]);
	for (int i = 0; i < nb_free; i++) {
		if (i != max_dir) {
			pa[i] = (1 - pa[max_dir]) / (nb_free - 1);
		}
	}
	return false;
}

double Behavior::qval (double angle) const
{
	double qval = neurosolver_.inf_get (angle);
	if (qval >= 0) {
		return qval;
	}
	else {
		// cf algo Rmax, on renvoit une val proche de la récompense max
		// pr forcer l'exploration (0.95 +- 0.05)
		cout << "angle " << angle << " unvisited" << endl;
		return 0.95 + bruit (0.1);
	}
}

string Behavior::automate_state ()
{
	switch( automate_ ) {
	    case GO_ON: return "GO_ON"; break;
	    case DECIDE: return "DECIDE"; break;
	    case DECIDED: return "DECIDED"; break;
	   	case GOAL: return "GOAL"; break;
	    case BLOCK: return "BLOCK"; break;
	    case SLEEP: return "SLEEP"; break;
	    default: return ""; break;
    }
}

