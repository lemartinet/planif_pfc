#include "behavior.hh"
#include "params.hh"
#include "math.hh"
#include "mystr.hh"
#include "action.hh"
#include "logger.hh"
#include <iostream>

#define WAIT_BETWEEN_DECISIONS 10

Behavior::Behavior (): 
	robot_(), neurosolver_(robot_), avoid_(robot_), action_done_(true),	
	wait_(0), wait_at_goal_ (0), current_(0.0)
{
}

Behavior::~Behavior () 
{
}

void Behavior::do_action ()
{
	// revoir cette fonction (peut-être deplacee dans obstacleavoid ?)
	static const int RANDOM_MOVE = Params::get_int("RANDOM_MOVE");
  	if (RANDOM_MOVE && robot_.nb_trial_get () > 5) {
  		robot_.setSpeed (10, 10);
  		return;
  	} 
  	if (wait_at_goal_ > 0) {
		robot_.setSpeed (0, 0);
		return;
	}
	
	double angle = current_;
	double diff = ecart_angulaire (robot_.angle_get (), angle);
	if (!action_done_) {
  		action_done_ = angle_equal (robot_.angle_get (), angle);
	}
	if (action_done_) {
		diff = 0;
	}
	int left_speed, right_speed;
	bool bloque = avoid_.avoid (diff, robot_.position_get (), left_speed, right_speed);
	neurosolver_.correct_transition (bloque);
	robot_.setSpeed (left_speed, right_speed);
}

void Behavior::e_greedy (const vector<double>& dirs, double* pa, stringstream& s)
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
	for (int i = 0; i < nb_free; i++) {
		// on evalue la meilleur action: non-nulle et correspond a une dir
//		if (action) {
//			cout << dirs[i] << "/" << action->angle_get() << " ";
//		}
		if (action && angle_equal (dirs[i], action->angle_get())) {
			found = true;
			pa[i] = 1;
		}
		else {
			pa[i] = 0;
		}
	}
//	cout << endl;
	// selon epsilon, on explore ou exploite cette meilleure action
	if (drand() < epsilon + epsilon_help || !found) {
		if (!found) {
			s << "no_action_";				
		}
		// le robot explore (dirs equiprobable)
		for (int i = 0; i < nb_free; i++) {
			pa[i] = 1.0 / nb_free;
//			cout << pa[i] << " ";
		}
//		cout << endl;
	}	
}

void Behavior::softmax (const vector<double>& dirs, double* pa)
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
}

void Behavior::q_greedy (const vector<double>& dirs, double* pa)
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

Action* Behavior::select_action ()
{
	vector<double> dirs;
	avoid_.free_ways (dirs, robot_.angle_get ());
	int nb_free = dirs.size ();
	// si on est dans un couloir, on continue
	// on a enleve la possibilite de revenir en arriere
	if (nb_free <= 1) {
		return 0;
	}
	// generation des pa (prob de select de l'action)
	double pa[nb_free];
	stringstream s;
	static const string EXPLO = Params::get ("EXPLO");
	if (EXPLO == "egreedy") {
		e_greedy (dirs, pa, s);
	}
	else if (EXPLO == "qgreedy") {
		cout << EXPLO << endl;
		q_greedy (dirs, pa);
	}
	else if (EXPLO == "soft") {
		softmax (dirs, pa);
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
	Action* action = new Action (dirs[i]);
	// TODO: a verifier
	bool explore = (neurosolver_.inf_get (dirs[i]) == -1);
	for (int j = 0; j < nb_free && !explore; j++) {
		if (pa[i] <= pa[j] && i != j) {
			explore = true;
		}	
	}
	if (explore) {
		s << "exploration ";	
	}
	else {
		s << "planning ";
	}
	s << dirs[i] << " " << pa[i];
	Logger::log ("decision", robot_.cpt_total_get (), s.str ());		
	return action;
}

void Behavior::compute_next_action ()
{
	robot_.synch ();
	if (robot_.sleep_get ()) {
		neurosolver_.sleep ();
		return;
	}
	
	// mecanisme pour s'arreter au goal et au depart
  	if (wait_at_goal_ > 0) {
		wait_at_goal_--;
	}
	else {
		wait_at_goal_ = robot_.goal_reached () ? 10 * WAIT_BETWEEN_DECISIONS : 0;
	}
	
	bool col_changed = neurosolver_.synch ();
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
	Action* action = select_action ();	
    if (action) {
    	wait_ = WAIT_BETWEEN_DECISIONS;
    	current_ = action->angle_get ();
    	action_done_ = false;
    	delete action;
    }
}

void Behavior::synch () 
{
  	compute_next_action ();
  	do_action (); 
}
