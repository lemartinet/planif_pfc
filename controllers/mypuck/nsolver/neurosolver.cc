#include "neurosolver.hh"
#include "action.hh"
#include "behavior.hh"
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

Neurosolver::Neurosolver () : 
	hippo_(), allo_pop_(), motivation_ (MOTIV, 1),
	columns_(hippo_.pop_get()), ripples_(0), learn_rythm_(0)
{
}

Neurosolver::~Neurosolver () 
{
}

void Neurosolver::goal_learning ()
{
	Column* col = columns_.best_state_col ();
	if (col && col->lastT_recent () >= 0.3) {
		set_goal_weight (col, 1);
	}
	// ajouter la détection de l'absence d'un goal attendu
}

void Neurosolver::set_goal_weight (Column* col, double val)
{
	double* w = col->sup_get ().syn_get (motivation_);
	if (w != 0) {
		*w = val;
	}
	else {
		col->sup_get ().add_synapse (motivation_, val);
	}	
}

bool Neurosolver::is_goal_position (Column* col) const
{
	double* w = col->sup_get ().syn_get (motivation_);
	return (w != 0) && (*w > 0);	
}

void Neurosolver::gui_goal_set(int no_col)
{
	Column* col = columns_.nocol_get(no_col);
	if (is_goal_position (col)) {
		set_goal_weight (col, 0.0);
	}
	else {
		set_goal_weight (col, 1.0);
	}	
}

void Neurosolver::synch (bool learning, bool decision_point, bool goal_found, bool bloque, 
						 bool sleeping, int sleep_step)
{
	if (sleeping) {
		sleep(sleep_step);
		return;	
	}
	if (goal_found) {
		goal_learning();
  		// on sauvegarde les poids à chaque fin d'essai
		Logger::logw ("weight");			
	}
	if (bloque) {
		columns_.correct_transition ();	
	}
	if (!learning) {
		// on vient d'arriver au goal, on apprend rien pendant un certain temps
		// permet de virer le lien entre fin de P1 et début de P1
// 		cout << "no learning" << endl;	
	  	columns_.synch (false, hippo_.pop_get (), &Behavior::behavior_get().position_get ());
  	}
  	else {
  		static const int LEARN_RYTHM = Params::get_int("LEARN_RYTHM");
  		bool learn_step = learn_rythm_ % LEARN_RYTHM == LEARN_RYTHM - 1;
	  	columns_.synch (learn_step, hippo_.pop_get (), &Behavior::behavior_get().position_get ());
	}
	++learn_rythm_;
	
	// on màj les entrées après pour avoir r_cortex(t) = F(r_subcort(t-1))
	hippo_.synch (Behavior::behavior_get().position_get ());
//	cout << Behavior::behavior_get().nb_path_get() << endl;
	allo_pop_.synch (Behavior::behavior_get().angle_get());
	motivation_.output_set (Behavior::behavior_get().goal_reached () || decision_point ? 1 : 0);
	Logger::log ();
}

void Neurosolver::sleep (int sleep_step)
{
//	cout << "sleep " << sleep_step << endl;
	// des ripples tous les 8 cycles theta (ici au 8eme cycle, entre bin 56 et 64 )
	// car 8 bins dans 1 cycle (8 * 15ms = 120ms)
	if (sleep_step % 64 == 56) {
		ripples_ = 8;
	}
	else if (ripples_ >= 0) {
		ripples_--;
	}
	columns_.synch (true, hippo_.pop_get ());
	hippo_.synch (Behavior::behavior_get().position_get (), true, ripples_);
	Logger::log ();

	if (sleep_step == 1) {
		Logger::logw ("weight");
	}
}

void Neurosolver::get_actions(vector<double>& angles, vector<double>& values)
{
	if (columns_.best_state_col () && is_goal_position (columns_.best_state_col ())) {
		angles.push_back(0);
		values.push_back(1);
		return;
	}
	const vector<Minicol*>& minicols = columns_.pop_minicol_get();
	vector<Minicol*>::const_iterator it;
	for (it = minicols.begin(); it != minicols.end(); it++) {
		Minicol* minicol = *it;
		if (minicol->recruited_get()) {
			angles.push_back(minicol->action_get().angle_get());
			// eventuellement utiliser activation() a la place de lastT_recent()
			values.push_back(minicol->lastT_recent());
		}
	}
	Minicol* best_minicol = columns_.best_minicol ();
	if (best_minicol) {
		stringstream s; 
		s << "3 " << best_minicol->from_get ().no_get ()+1 << " " << best_minicol->to_get ().no_get ()+1 
			<< " " << best_minicol->action_get().angle_get();
		Logger::log ("network", s.str (), true);	
	}		
}

bool Neurosolver::shortcut(double available, int& nb_action)
{
	nb_action = 0;
	Column* col = columns_.best_state_col();
	if (is_goal_position(col))
		return true;
	else if (col == 0)
		return false;
	vector<int> visited;
	double angle = 0;
	do {
		visited.push_back(col->no_get());
		vector<Minicol*> minicols = columns_.minicol_get(col->no_get());
		Minicol* best_minicol = 0;
		vector<Minicol*>::iterator it;
		for (it = minicols.begin (); it != minicols.end (); ++it) {
			if (!best_minicol || (best_minicol->lastT_recent () < (*it)->lastT_recent ())) {
				best_minicol = *it;
			}
		}
		if (best_minicol == 0)
			break;
		col = const_cast<Column*>(&best_minicol->to_get());
		angle += best_minicol->action_get().angle_get();
		nb_action++;
//		cout << col->no_get() << " ";
	} while (!is_goal_position(col) && !angle_equal(angle / nb_action, available) &&
			find(visited.begin(), visited.end(), col->no_get()) == visited.end());
	return angle_equal(angle / nb_action, available);
}

