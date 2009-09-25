#include <vector>
#include <sstream>
#include <iostream>
#include <assert.h>
#include "column.hh"
#include "hippo.hh"
#include "cell.hh"
#include "params.hh"
#include "mystr.hh"

extern Params* params;

using namespace std;

Colomn::Colomn (Neuralnet& net, vector<ComputeUnit*>& pop, int no, int level, bool draw)
{
  stringstream ss;
  string       val;
  static const double VR = params->get_double("VR");
  static const double VF = params->get_double("VF");

  no_ = no;
  ss << no;
  ss >> val;
  path_ = string ("c") + string (val);
  path_ += string (":");
  goals_ = net.add_neuron (VR, VF, path_, no, 200, 0.9, 10.0, -0.5, level);
  if (level == 1) {
  	state_ = net.add_neuron_max (VR, VF, path_, no, level);
  }
  else { 
  	//state_ = net.add_neuron (VR, VF, path_, no, 200, 0.9, 2.0, -2.5, level);
  	state_ = net.add_neuron_max (VR, VF, path_, no, level);
  }
  sup_ = net.add_neuron_max (VR, VF, path_, no, level); 
  inf_ = net.add_neuron_max (VR, VF, path_, no, level); 
  net.add_synapse (*state_, *inf_, 1.0, true);
  net.add_synapse (*goals_, *sup_, 1.0, true);
  mode_ = LEARNING;
//  net.add_synapse (*state_, *goals_, 1.0, true);
  net_ = &net;
  pop_state_ = &pop;
  //hippo_goals_ = &goals;
  lastmincol_ = 0;
  //connect_hippo_to_neuron (state, *state_);
  //connect_hippo_to_neuron (goals, *goals_);
	level_ = level;
	draw_ = draw;
	winner_ = true;
	connect_pop_to_neuron ();
}

Colomn::~Colomn ()
{
  int size = minicols_.size ();

  net_->del_neuron (state_);
  net_->del_neuron (goals_);  
  net_->del_neuron (sup_);
  net_->del_neuron (inf_);
  for (int i = 0; i < size; i++)
    delete minicols_[i];
}

void Colomn::mode_set (enum e_mode mode)
{
  	int size = minicols_.size ();
 
  	if (mode_ != mode) {
    	mode_ = mode;
      	if (mode_ == LEARNING) {
      		net_->add_synapse (*state_, *goals_, 1.0, true);
      	}
      	else {
      		net_->del_synapse (*state_, *goals_);
      	}
      	for (int i = 0; i < size; i++) {
      		minicols_[i]->mode_set (mode);
      	}
    }
}

Minicol* Colomn::lateral_learning (Action* action, Colomn& dest, bool increase, string & message)
{  	
//  	Minicol* minicol = 0;
//  	Minicol* m1 = minicol_get (*action);
//  	Minicol* m2 = minicol_get_nodest (dest.no_get ());
//  	if (m1) {
//  		minicol = m1;
//  	}
//  	else if (m2) {
//  		minicol = m2;
//  	}
	Minicol* minicol = minicol_get_nodest (dest.no_get ());
  	
  	if (minicol) {
  		// la minicolonne existe déjà : on modifie les poids
  		net_->lateral_learning (minicol->inf_get (), dest.inf_get (), increase);
      	net_->lateral_learning (dest.sup_get (), minicol->sup_get (), increase);
      	
      	if (level_ == 0) {
	      	// on moyenne l'orientation
	      	message = "update minicol " + i2str (no_) + "->" + i2str (dest.no_get ()) + " : " 
	      			+ d2str (minicol->action_get ().angle_get ());
	      	minicol->adapt_action (action);
	      	message += " / " + d2str (minicol->action_get ().angle_get ());
      	}
		return 0;
  	}
  	else {
  		// on recrute une nouvelle minicolonne
      	minicol = new Minicol (*net_, action, *this, dest, path_, no_, minicols_.size (), level_);
      	//cout << "new minicol " << this->no_get() << "->" << dest.no_get() << " : " << action->angle_get() << endl;
      	minicols_.push_back (minicol);
      	// Intra-colomns connections
      	net_->add_synapse (minicol->sup_get (), *sup_, 1.0, true);
      	net_->add_synapse (*inf_, minicol->inf_get (), 1.0, true);
      	//net_->add_synapse (*state_, minicol->inf_get (), 0.5, true);
      	// Inter-columns connections
      	net_->lateral_learning (minicol->inf_get (), dest.inf_get (), true);
      	net_->lateral_learning (dest.sup_get (), minicol->sup_get (), true);
      	message = "new minicol " + i2str (no_) + "->" + i2str (dest.no_get ()) + " : " + d2str (action->angle_get ());
      	return minicol;
    }
}

bool Colomn::spiking ()
{
  int size = minicols_.size ();

  for (int i = 0; i < size; i++)
    {
      if (minicols_[i]->spiking ())
	return true;
    }
  return false;
}

Minicol* Colomn::minicol_get (Action& action)
{
	int  size = minicols_.size ();

	for (int i = 0; i < size; i++) {
		if (level_ == 0 && minicols_[i]->action_get () == action) {
			return minicols_[i];
		}
	}
	return 0;
}

void Colomn::del_minicols (Colomn& dest)
{
  Minicol*                      minicol = 0;
  vector<Minicol *>::iterator   it;
  int                           size = minicols_.size ();

  /*
  for (vector<Minicol *>::iterator it = minicols_.begin (); it != minicols_.end (); ++it)
    {
      if ((*it)->to_get () == dest)
	{
	  if (*it == lastmincol_)
	    lastmincol_ = 0;
	  delete *it;
	  minicols_.erase (it);
	  }
	}
  */
  for (int i = 0; i < size; i++)
    {
      if (minicols_[i]->to_get () == dest)
	{
	  minicol = minicols_[i];
	  if (minicol == lastmincol_)
	    lastmincol_ = 0;	
	  it = find (minicols_.begin (), minicols_.end (), minicol);
	  delete *it;
	  minicols_.erase (it);
	  i--;
	  size--;
	}
    }
}

Minicol* Colomn::best_minicol ()
{
  int     nbmincols = minicols_.size ();
  double  val = 0.0;
  double  max = 0.0;
  int     maxpos = 0;

  if (nbmincols == 0)
    return 0;
  for (int i = 0; i < nbmincols; i++)
    {
      val = minicols_[i]->activation ();
      if (val > max)
	{
	  max = val;
	  maxpos = i;
	}
    }
  return minicols_[maxpos];
}

Minicol* Colomn::best_mean_minicol ()
{
  int     nbmincols = minicols_.size ();
  double  val = 0.0;
  double  max = 0.0;
  int     maxpos = 0;

  if (nbmincols == 0)
    return 0;
  for (int i = 0; i < nbmincols; i++)
    {
      val = minicols_[i]->mean_value_get ();
      //cout << minicols_[i]->from_get().no_get() << "->" << minicols_[i]->to_get().no_get() 
      //		<< " : " << minicols_[i]->activation() << " / " << val << endl;
      if (val > max)
	{
	  max = val;
	  maxpos = i;
	}
    }
  return minicols_[maxpos];
}

void Colomn::lastmincol_update ()
{
  Minicol* best = best_mean_minicol ();
  if (best && best->spiking ())
    lastmincol_ = best;
}

void update_state_IP (Neuron& state)
{
  if (state.spiking ())
    state.synch_IP ();
}

void Colomn::synch ()
{
	int nbmincols = minicols_.size ();
	for (int i = 0; i < nbmincols; i++) {
		minicols_[i]->update_value ();
    }

	lastmincol_update ();
  	if (mode_ == LEARNING || level_ == 1) {
  		if (pop_state_) {
	    	update_links_pop_neuron ();
      		pos_update ();
      		// a faire aussi avec le neurone goal si on le reutilise
    	}
  		update_state_IP (*state_);
  	}
}

void Colomn::draw (ostream& os)
{
  int          size = minicols_.size ();
  stringstream ss;
  string       val;

  ss << no_;
  ss >> val;
  os << "c" << val << " [label=\"{<n" << goals_->no_get () << "> G | <n" << state_->no_get () << "> H} | {<n" << sup_->no_get () << "> Prec | {";
  for (int i = 0; i < size; i++)
    {
      minicols_[i]->draw (os);
      if (i < size - 1)
	os << " | ";
    }
  os << "} | <n" << inf_->no_get () << "> Suiv} \"];" << endl;
}

void Colomn::draw_links (ostream& os)
{
  int size = minicols_.size ();

  for (int i = 0; i < size; i++)
    minicols_[i]->draw_links (os);
}

void Colomn::pos_update ()
{
	if (level_ == 0) {
		int    size = state_->size ();
		Cell*  cell = 0;
		Coord  moy;
		Coord  tmp;
		double norm = 0.0;
		double weight = 0.0;
		
		for (int i = 0; i < size; i++) {
			cell = dynamic_cast<Cell *> (&(state_->syn_get (i)->from_get ()));
			tmp = cell->pos_get ();
			weight = state_->syn_get (i)->w_get ();
			tmp *= weight;
			moy += tmp;
			norm += weight;
		}
		moy /= norm;
		pos_ = moy;
	}
}

Minicol* Colomn::minicol_get_nodest (int no)
{
  int sizecol = size ();
 
  for (int i = 0; i < sizecol; i++)
    {
      if (minicol_get (i)->to_get ().no_get () == no)
	return minicol_get (i);
    }
  return 0;
}

double Colomn::weights_mean ()
{
  int       size = state_->size ();
  double    mean = 0.0;

  for (int i = 0; i < size; i++)
    mean += state_->syn_get (i)->w_get ();

  return mean; // / (double)size;
}

bool operator== (Colomn& c1, Colomn& c2)
{
  return c1.no_get() == c2.no_get();
}

void Colomn::connect_pop_to_neuron ()
{
	static const double NEURON_SPIKING_THRESH = params->get_double("NEURON_SPIKING_THRESH");
	
	int nbunits = pop_state_->size ();
	ComputeUnit* unit = 0;
	double init_val = 0;
	double rj = 0;
	
	if (level_ == 0) {
		for (int i = 0; i < nbunits; i++) {
			unit = pop_state_->at (i);
			rj = unit->output ();
			init_val = rj > NEURON_SPIKING_THRESH ? rj : 0.0; 
			state_->add_synapse (*unit, init_val, false);
		}
	}
	else if (level_ == 1 && winner_) {
		for (int i = 0; i < nbunits; i++) {
			unit = pop_state_->at (i);
			if (unit->level_get () < state_->level_get () ) {
				rj = unit->output ();
				if (rj > NEURON_SPIKING_THRESH && state_->syn_get(*unit) == 0) {
					state_->add_synapse (*unit, 0.5, true);
//					state_->add_synapse (*unit, rj, false);
//	    			printf("new lien %d (%d) -> %d (%d) : %f\n", dynamic_cast<Neuron*>(unit)->no_col_get(), unit->level_get (), 
//	    					no_, state_->level_get (), rj);
				}
				else if (state_->syn_get(*unit) != 0) {
					Synapse* s = state_->syn_get(*unit);
//					s->w_set (max (s->w_get (), rj));
					printf("update lien %d (%d) -> %d (%d) : %f\n", dynamic_cast<Neuron*>(unit)->no_col_get(), unit->level_get (), 
	    					no_, state_->level_get (), s->w_get ());					
				}
				else {
//					printf("lien %d (%d) -> %d (%d) : none\n", dynamic_cast<Neuron*>(unit)->no_col_get(), unit->level_get (), 
//							no_, state_->level_get ());
				}
	    	}
		}
	}
	// on veut que l'activite du neurone cree soit > a NEURON_SPIKING_THRESH
//	state_->boost_weights ();
}

// Met a jour la col avec pop (cable les nouvelles unités)
// ne gere pas la supp d'unités dans la pop
void Colomn::update_links_pop_neuron ()
{
	static const double NEURON_SPIKING_THRESH = params->get_double("NEURON_SPIKING_THRESH");
	
	double init_val = 0.0;
	double ri = state_->output ();
	
	// pour l'instant on ne gere pas lvl0 et lvl1 pareil...
	if (level_ == 0) {
		int  nbnewcells = pop_state_->size () - state_->size ();
		int  unitpos = state_->size ();
		ComputeUnit* unit = 0;
	
		for (int i = 0; i < nbnewcells; i++) {
			unit = pop_state_->at (unitpos + i);
			init_val = (ri > NEURON_SPIKING_THRESH) ? ri : 0.0;
			state_->add_synapse (*unit, init_val, false);
		}
	}
	else if (level_ == 1 && winner_) {
		connect_pop_to_neuron ();
	}
}
