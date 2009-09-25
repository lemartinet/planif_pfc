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
#include "neurosolver.hh"
#include "behavior.hh"
#include "mystr.hh"
#include "params.hh"
#include "cell.hh"

extern Params* params;

Testy::Testy (RobotDevice& robot, Behavior& behav, Neurosolver& solver) : 
	robot_(&robot), behav_(&behav), solver_(&solver), weights0_(0.0), 
	weights1_(0.0), cpt_(0), cpt_goal(0), simu_id_(0) {}

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

double Testy::weights_sum_mean ()
{
  Colomn*   col = 0;
  int       nbcols = solver_->cols_get().size ();
  double    mean = 0.0;

  for (int i = 0; i < nbcols; i++)
    {
      col = solver_->cols_get().col_get (i);
      mean += col->weights_mean ();
    }
  return mean / (double)nbcols;
}


double Testy::weights_sum_max ()
{
  Colomn*   col = 0;
  Colomns*  cols = &(solver_->cols_get ());
  int       nbcols = cols->size ();
  double    val = 0.0;
  double    max = 0.0;

  for (int i = 0; i < nbcols; i++)
    {
      col = cols->col_get (i);
      val = col->weights_mean ();
      if (val > max)
	max = val;
    }
  return max;
}


// A Modifier
void Testy::end_simu ()
{
	// utile pour etudier la convergence des poids PCells->colonnes
	weights1_ = weights_sum_max ();
	string filename ("../../data/data_raw/");
	filename += i2str (simu_id_);
	filename += "/weights_mean.txt";
	ofstream file (filename.c_str(), ios::app);
	file << "w1 = " << weights0_ << endl;
	file << "w2 = " << weights1_ << endl; 
	file << "w2 - w1 = " << weights1_ - weights0_ << endl;
}


void Testy::init ()
{	
	cpt_ = 0;

  	weights0_ = 0.0;
  	weights1_ = 0.0;
  	
  	simu_id_ = params->get_int ("SIMULATION_ID");
}


void Testy::synch ()
{
	// si on est en mode echantillonage des activites, 
	// on enregistre a chaque pas de temps
	static const int RANDOM_MOVE = params->get_int("RANDOM_MOVE");

	unsigned int nbcols = solver_->cols_get().size ();
  	unsigned int nbcells = solver_->hippo_get().size ();
  	int nbsyn = 0;
  	Neuron* neuron = 0;
  	Cell* cell = 0;

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
	  		nbsyn = neuron->size ();
	  		ofstream* f = files_weights_[i];
	  	  	*f << "# step " << i2str (cpt_) << endl;
	  	  	for (int j = 0; j < nbsyn; j++) {
	  	  		cell = (Cell*) &(neuron->syn_get (j)->from_get ());
	      		*f << cell->pos_get ().x_get () << " "
		      		<< cell->pos_get ().y_get () << " "
		      		<< neuron->syn_get (j)->w_get ()
		      		<< endl;
	    	}
	    	*f << endl << endl;
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
	      		<< solver_->hippo_get().cell_get (i)->output ()
	      		<< endl;
	      	*f << endl << endl;
    	}
	}
    
  	if (cpt_ == 50)
    	weights0_ = weights_sum_max ();

	cpt_++;
}
