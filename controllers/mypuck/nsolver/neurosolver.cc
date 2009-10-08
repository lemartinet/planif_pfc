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

#define LEARN_RYTHM 96
//#define LEARN_RYTHM 8

Action go_forward_goal (0);

Neurosolver::Neurosolver () : 
	hippo_(), ego_pop_(), allo_pop_(), motivation_ (MOTIV, 1), 
	columns_(hippo_.pop_get (), *ego_pop_.pop_get ().at (1)),  
	ripples_(0), learn_rythm_(0)
{
	Logger::logw ("weight");
}

Neurosolver::~Neurosolver () 
{
}

Action* Neurosolver::best_action () const
{
	if (columns_.best_state_col (0) && is_goal_position (columns_.best_state_col (0))) {
		return &go_forward_goal;
	}
	 
	Minicol* best_minicol = columns_.best_minicol (0);	
	if (best_minicol) {
		stringstream s; 
		s << "3 " << best_minicol->from_get ().no_get ()+1 << " " << best_minicol->to_get ().no_get ()+1 
			<< " " << best_minicol->action_get().angle_get();
		Logger::log ("network", s.str (), true);	
		return &best_minicol->action_get ();
	}
	return 0;
}


void Neurosolver::goal_learning ()
{
	Column* col = columns_.best_state_col (0);
	if (!col || col->lastT_recent () < 0.3) {
		return;
	}
	set_goal_weight (col, 1);
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

bool Neurosolver::is_goal_position (Column* col) const
{
	Synapse* s = col->sup_get ().syn_get (motivation_);
	return (s != 0) && (s->w_get() > 0);	
}

bool Neurosolver::synch (bool learning, bool decision_point)
{
	if (Behavior::behavior_get().goal_reached ()) {
  		// on sauvegarde les poids à chaque fin d'essai
		Logger::logw ("weight");			
	}
	bool col_changed = false;
	if (!learning) {
		// on vient d'arriver au goal, on apprend rien pendant un certain temps
		// permet de virer le lien entre fin de P1 et début de P1
// 		cout << "no learning" << endl;	
	  	columns_.synch (false, hippo_.pop_get (), *ego_pop_.pop_get ().at (1), &Behavior::behavior_get().position_get ());
  		col_changed = true;
  	}
  	else {
  		bool learn_step = learn_rythm_ % LEARN_RYTHM == LEARN_RYTHM - 1;
	  	col_changed = columns_.synch (learn_step, hippo_.pop_get (), *ego_pop_.pop_get ().at (1), &Behavior::behavior_get().position_get ());
	}
	++learn_rythm_;
	
	// on màj les entrées après pour avoir r_cortex(t) = F(r_subcort(t-1))
	hippo_.synch (Behavior::behavior_get().position_get ());
	ego_pop_.synch (Behavior::behavior_get().angle_get()); 
	allo_pop_.synch (Behavior::behavior_get().angle_get());
	motivation_.output_set (Behavior::behavior_get().goal_reached () || decision_point ? 1 : 0);
	
	Logger::log ();
  	return col_changed;
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
	columns_.synch (true, hippo_.pop_get (), *ego_pop_.pop_get ().at (1));
	hippo_.sleep (ripples_);
	Logger::log ();

	if (sleep_step == 1) {
		Logger::logw ("weight");
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
	if (columns_.best_state_col (0) == 0) {
		return 1;	
	}
	Action a (angle);
	vector<double> qvals;
	int nbcols = columns_.size ();
	// on regarde l'ensemble des états actifs pr calculer la qval
	for (int i = 0; i < nbcols; i++) {
		if (columns_.col_get (i)->spiking ()) {
			vector<Minicol*> all_minicols = columns_.minicol_get (columns_.col_get (i)->no_get(), a);
			for (vector<Minicol*>::iterator it = all_minicols.begin (); it != all_minicols.end (); ++it) {
				qvals.push_back ((*it)->activation ());	
			}
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

