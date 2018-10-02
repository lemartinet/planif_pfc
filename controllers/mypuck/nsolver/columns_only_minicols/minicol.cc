#include <sstream>
#include <iostream>
#include "minicol.hh"
#include "column.hh"
#include "hippo.hh"
#include "params.hh"
#include "math.hh"
#include "action.hh"

extern Params* params;

//int Minicol::counter = 0;
int Minicol::counter_value = 0;

Minicol::Minicol (Hippo* hippo, Neuralnet& net, Action * action, Column& src, Column * dest, string& path, int no_col, int no)
{
	static const double VR = params->get_double("VR");
  	static const double VF = params->get_double("VF");
  	hippo_ = hippo;
  	path_ = path;
  	no_ = no;
  	sup_ = net.add_neuron_max (VR, VF, path_, no_col); //<- here
  	inf_ = net.add_neuron (VR, VF, path_, no_col, 200.0, 0.9, 2.0, -2.5); // old:  8.0, -1.2
  	action_ = (action?action->clone ():action);
  	net_ = &net;
  	src_ = &src;
  	dest_ = dest;
	learn_ = true;
	connect_hippo_to_minicol ();
	for (int i = 0; i < MEAN_SIZE; i++) {
		last_values_[i] = inf_->output();
	}
	val = inf_->output();
//	for (int i = 0; i < COMPET_SIZE; i++) {
//		activ_[i] = 0;
//	}
}

Minicol::Minicol (Minicol* mini_src, Hippo* hippo, Neuralnet& net, Action * action, Column& src, Column * dest, string& path, int no_col, int no)
{
	static const double VR = params->get_double("VR");
	static const double VF = params->get_double("VF");
	hippo_ = hippo;
	path_ = path;
	no_ = no;
	sup_ = net.add_neuron_max (VR, VF, path_, no_col); //<- here
	inf_ = net.add_neuron (VR, VF, path_, no_col, 200.0, 0.9, 2.0, -2.5); // old:  8.0, -1.2
	action_ = (action?action->clone ():action);
	net_ = &net;
	src_ = &src;
	dest_ = dest;
	learn_ = true;
	// connexion de la nouvelle minicol hippo, minicols voisines, ...
	// a partir d'une minicol existante de la colonne 
	copy_inf_connections(mini_src);
	copy_sup_connections(mini_src);
	for (int i = 0; i < MEAN_SIZE; i++) {
		last_values_[i] = inf_->output();
	}
	val = inf_->output();
//	for (int i = 0; i < COMPET_SIZE; i++) {
//		activ_[i] = 0;
//	}
}

Minicol::~Minicol ()
{
  net_->del_neuron (sup_);
  net_->del_neuron (inf_);
  delete action_;
}

void Minicol::draw (ostream& os)
{
  os << "{<n" << sup_->no_get () << "> s" << no_ << " | <n" << inf_->no_get () << "> i" << no_ << "}";
}

void Minicol::draw_links (ostream& os)
{
  stringstream ss;
  string       val;

  ss << dest_->no_get ();
  ss >> val;
  os << inf_->path_get () << " -> " << "c" << val << "[color=red]" << ";" << endl;
}

void Minicol::learn_set (bool learn)
{
	if (learn_ != learn) {
		learn_ = learn;
      	if (learn_) {
        	net_->del_synapse (*sup_, *inf_);
      	}
      	else {
        	net_->add_synapse (*sup_, *inf_, 1.0, true);
      	}
    }
}

void Minicol::adapt_action (Action * action)
{
	double new_angle = mean_angles (action_->angle_get(), action->angle_get());
//	cout << "update action ";
//	cout << src_->no_get() << "->" << dest_->no_get() << " : " << action_->angle_get() << " + ";
	action_->angle_set(new_angle);
//	cout << action->angle_get() << " = " << action_->angle_get() << endl; 
}
  
double Minicol::weights_mean ()
{
	int size = inf_->size ();
	double mean = 0.0;
	
	for (int i = 0; i < size; i++) {
		mean += inf_->syn_get (i)->w_get ();
	}
	return mean;
}

bool Minicol::goal_activity_setted ()
{
	return sup_->intra_setted();
}

void Minicol::goal_activity_set (double val)
{
	sup_->intra_set(val);	
}

void Minicol::goal_activity_unset ()
{
	sup_->intra_unset();
}

void Minicol::inhib (bool val)
{
	inf_->inhibed_set(val);
}

void Minicol::update_IP ()
{
	// pour le moment seulement inf_ est adapté
	if (inf_->activated ()) {
		inf_->synch_IP ();
	}
}

void Minicol::connect_hippo_to_minicol ()
{
	int nbcells = hippo_->size ();
	Cell* cell = 0;
	double init_val = 0;
	double rj = 0;

  	for (int i = 0; i < nbcells; i++) {
		cell = hippo_->cell_get (i);
	    rj = cell->output ();
		init_val = rj > 0.2 ? rj : 0.0;
	    //init_val = drand48 () * 0.1;
	    //init_val = exp (- (diff * diff));      
	    inf_->add_synapse (*cell, init_val, false);
    }
    
    // TODO a virer
    // on veut que l'activiter du neurone cree soit > a NEURON_SPIKING_THRESH
    inf_->boost_weights ();
}

// Mettre a jour la col avec hippo (cabler avec nouvelles cellules)
// TODO : a corriger
void Minicol::update_links_hippo_neuron ()
{
	int nbnewcells = hippo_->size () - inf_->size (); // pb ici car inf varie aussi maintenant
	int cellpos = inf_->size ();
	Cell* cell = 0;

	for (int i = 0; i < nbnewcells; i++) {
		double init_val = 0.0;
		//double rj = cell.output ();
		double ri = inf_->output ();
		//double diff = rj - ri;
		cell = hippo_->cell_get (cellpos + i);
		
		if (ri > 0.3) {
			init_val = ri;// * 6.0;
			//init_val = init_val > 1.0 ? 1.0 : init_val;
		}
		else {
			init_val = 0.0;
			//init_val = (neuron.size () == 0) ? cell.output () : neuron.output ();
			//init_val = drand48 () * 0.1;
			//init_val = exp (- (diff * diff));
			inf_->add_synapse (*cell, init_val, false);
		}
	}
}

void Minicol::copy_inf_connections (Minicol* src)
{
	int size = src->inf_get ().size ();
	for (int i = 0; i < size; i++) {
		Synapse* s = src->inf_get ().syn_get (i);
		if (s->from_get ().no_get () != src->sup_get ().no_get ()) {
			this->inf_get ().add_synapse (s->from_get (), s->w_get (), false);
			// TODO pb ici car on créé deux connections HP -> minicol
		}
	}
}

void Minicol::copy_sup_connections (Minicol* src)
{
	// TODO attention quand on connectera les HD et HP
	int nbneurons = net_->size();
	for (int j = 0; j < nbneurons; j++) {
		Neuron* nj = net_->get (j);
		int size = nj->size ();
		if (nj->no_get () != src->inf_get ().no_get()
			&& nj->no_get () != src->sup_get ().no_get()) {
			for (int i = 0; i < size; i++) {
				Synapse* s = nj->syn_get (i);
				if (s->from_get ().no_get () == src->sup_get ().no_get ()) {
					nj->add_synapse (this->sup_get (), s->w_get (), false);
				}
			}
		}
	}
}

// approxime la position du centroid
void Minicol::pos_update ()
{
	int size = inf_->size ();
  	Cell*  cell = 0;
  	Coord  moy;
  	Coord  tmp;
  	double norm = 0.0;
  	double weight = 0.0;

  	for (int i = 0; i < size; i++) {
    	cell = dynamic_cast<Cell *> (&(inf_->syn_get (i)->from_get ()));
		if (cell) {
	      	tmp = cell->pos_get ();
	      	weight = inf_->syn_get (i)->w_get ();
	      	tmp *= weight;
	      	moy += tmp;
	      	norm += weight;
		}
    }
  	moy /= norm;
  	pos_ = moy;
}

void Minicol::synch ()
{
	update_value ();
	if (learn_ && hippo_) {
		// TODO decommenter si on a ajoute des cellules de lieux en meme tps que prefrontal
		// update_links_hippo_neuron ();
		update_IP ();
	    pos_update ();		
  	}
}
