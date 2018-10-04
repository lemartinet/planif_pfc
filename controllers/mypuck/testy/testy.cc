/*
 * File:         testy.cc
 * Date:         December 2007
 * Description:  class for all the outputs
 * Author:       Benjamin Fouque, louis-emmanuel.martinet@lip6.fr
 */

#include <iostream>
#include <sstream>
#include "testy.hh"
#include "device.hh"
#include "columns.hh"
#include "column.hh"
#include "neurosolver.hh"
#include "behavior.hh"
#include "mystr.hh"
#include "params.hh"
#include "cell.hh"

extern Params* params;

Testy::Testy (Behavior& behav) : robot_(&behav.robot_get ()), behav_(&behav), 
	solver_(&behav.neurosolver_get ()), cpt_(0), cpt_goal(0)
{
  	simu_id_ = params->get_int ("SIMULATION_ID");	
}

Testy::~Testy ()
{
	for (unsigned int i = 0; i < files_weights_.size(); i++) {
		delete files_weights_[i];	
	}
	for (unsigned int i = 0; i < files_outputs_.size(); i++) {
		delete files_outputs_[i];	
	}
	for (unsigned int i = 0; i < files_cells_.size(); i++) {
		delete files_cells_[i];	
	}
}

void Testy::synch ()
{
	// si on est en mode echantillonage des activites, 
	// on enregistre a chaque pas de temps
	static const int RANDOM_MOVE = params->get_int("RANDOM_MOVE");

	unsigned int nbcols = solver_->cols_get().size ();
  	unsigned int nbcells = solver_->hippo_get().size ();
  	Neuron* neuron = 0;

	while (nbcols > files_weights_.size()) {
		string filename ("../../data/data_raw/");
		filename += i2str (simu_id_);
		filename += "/w_";
	  	filename += i2str (files_weights_.size());
		filename += ".txt";
		ofstream* f = new ofstream (filename.c_str ());
	  	files_weights_.push_back(f);
	  	
		filename = "../../data/data_raw/";
		filename += i2str (simu_id_);
		filename += "/o_";
	  	filename += i2str (files_outputs_.size());
		filename += ".txt";
		f = new ofstream (filename.c_str ());
		files_outputs_.push_back(f);
	}
	
	while (nbcells > files_cells_.size()) {
		string filename ("../../data/data_raw/");
		filename += i2str (simu_id_);
		filename += "/c_";
	  	filename += i2str (nbcells-1);
		filename += ".txt";
		ofstream* f = new ofstream (filename.c_str ());
		files_cells_.push_back(f);
	}
	
  	if (cpt_ % TESTY_TIMESTEP_WEIGHTS == 0) {
  		for (unsigned int i = 0; i < nbcols; i++) {
			neuron = &(solver_->cols_get().col_get (i)->state_get ());
	  		ofstream* f = files_weights_[i];
	  	  	*f << "# step " << i2str (cpt_) << endl;
	  	  	neuron->print_weights (*f);
			*f << endl;
		}
	}
	
  	if ((RANDOM_MOVE && behav_->get_nb_goal_reached () > 5) || (!RANDOM_MOVE && cpt_ % TESTY_TIMESTEP_OUTPUTS == 0)) {
  		for (unsigned int i = 0; i < nbcols; i++) {
  			neuron = &(solver_->cols_get().col_get (i)->state_get ());
  			ofstream* f = files_outputs_[i];
	  	  	*f << "# step " << i2str (cpt_) << endl;
	  		*f << robot_->position_get ().x_get () << " "
	    		<< robot_->position_get ().y_get () << " "
	      		<< neuron->output ()
	      		<< endl;
	      	*f << endl << endl;
      	}
	}
	
	if ((RANDOM_MOVE && behav_->get_nb_goal_reached () > 5) || (!RANDOM_MOVE && cpt_ % TESTY_TIMESTEP_CELLS == 0)) {
		for (unsigned int i = 0; i < nbcells; i++) {
			ofstream* f = files_cells_[i];
	      	*f << robot_->position_get ().x_get () << " "
	      		<< robot_->position_get ().y_get () << " "
	      		<< solver_->hippo_get().cell_get (i).output ()
	      		<< endl;
	      	*f << endl << endl;
    	}
	}
    
	cpt_++;
}
