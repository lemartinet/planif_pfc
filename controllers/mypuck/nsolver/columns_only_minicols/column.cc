#include <vector>
#include <sstream>
#include <iostream>
#include <assert.h>
#include "column.hh"
#include "minicol.hh"
#include "columns.hh"
#include "hippo.hh"
#include "params.hh"
#include "mystr.hh"
#include "coord.hh"

extern Params* params;

using namespace std;

Column::Column (Neuralnet& net, Hippo& state, int no)
{
	stringstream ss;
	string       val;

	no_ = no;
	ss << no;
	ss >> val;
	path_ = string ("c") + string (val);
	path_ += string (":");
	net_ = &net;
	hippo_state_ = &state;
	// on ajoute une minicol sans selectivite d'orientation
	Minicol * minicol = new Minicol (&state, *net_, 0, *this, 0, path_, no_, minicols_.size ());
    minicols_.push_back (minicol);
	lastmincol_ = minicol;
}

Column::~Column ()
{
  int size = minicols_.size ();
  for (int i = 0; i < size; i++)
    delete minicols_[i];
}

void Column::learn_set (bool learn)
{
	int size = minicols_.size ();
	for (int i = 0; i < size; i++) {
		minicols_[i]->learn_set (learn);
	}
}

Minicol* Column::lateral_learning (Action* action, Column& dest, bool increase, string & message)
{
	// TODO : a remplacer par un truc plus biomim	
	Minicol* minicol = minicol_get_nodest (dest.no_get ());
  	
  	if (minicol) {
  		// la minicolonne existe déjà : on modifie les poids
  		int nb = dest.size ();
		for (int i = 0; i < nb; i++) {
	  		net_->lateral_learning (minicol->inf_get (), dest.minicol_get (i)->inf_get (), increase);
    	  	net_->lateral_learning (dest.minicol_get (i)->sup_get (), minicol->sup_get (), increase);
    	}
      	// on moyenne l'orientation
      	message = "update minicol " + i2str (no_) + "->" + i2str (dest.no_get ()) + " : " 
      			+ d2str (minicol->action_get ()->angle_get ());
      	minicol->adapt_action (action);
      	message += " / " + d2str (minicol->action_get ()->angle_get ());
    	return 0;
  	}
  	else {
  		// on recrute une nouvelle minicolonne
      	minicol = new Minicol (minicols_[size ()-1], hippo_state_, *net_, action, *this, &dest, path_, no_, minicols_.size ());
      	//cout << "new minicol " << this->no_get() << "->" << dest.no_get() << " : " << action->angle_get() << endl;
      	minicols_.push_back (minicol);
      	// connexions inter-columns
      	int nb = dest.size ();
		for (int i = 0; i < nb; i++) {
	  		net_->lateral_learning (minicol->inf_get (), dest.minicol_get (i)->inf_get (), true);
    	  	net_->lateral_learning (dest.minicol_get (i)->sup_get (), minicol->sup_get (), true);
    	}
      	message = "new minicol " + i2str (no_) + "->" + i2str (dest.no_get ()) + " : " + d2str (action->angle_get ());
      	return minicol;
    }
}

Minicol* Column::minicol_get (Action& action)
{
	int  size = minicols_.size ();
	for (int i = 0; i < size; i++) {
		Action* a = minicols_[i]->action_get ();
		if (a && *a == action) {
			return minicols_[i];
		}
	}
	return 0;
}

void Column::del_minicols (Column& dest)
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
      if (*(minicols_[i]->to_get ()) == dest)
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

Minicol* Column::best_minicol ()
{
  int     nbmincols = minicols_.size ();
  double  val = 0.0;
  double  max = 0.0;
  int     maxpos = 0;

  if (nbmincols == 0)
    return 0;
  for (int i = 0; i < nbmincols; i++)
    {
      val = minicols_[i]->inf_activation ();
      if (val > max)
	{
	  max = val;
	  maxpos = i;
	}
    }
  return minicols_[maxpos];
}

Minicol* Column::best_mean_minicol ()
{
  int     nbmincols = minicols_.size ();
  double  val = 0.0;
  double  max = 0.0;
  int     maxpos = 0;

  if (nbmincols == 0)
    return 0;
  for (int i = 0; i < nbmincols; i++)
    {
      val = minicols_[i]->mean_value ();
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

void Column::synch ()
{
	int nbminicol = minicols_.size ();
    Minicol::increment_counter_value ();
	for (int i = 0; i < nbminicol; i++) {
		minicol_get (i)->synch ();
  	}
  	
	Minicol* best = best_mean_minicol ();
	if (best && best->inf_get ().activated ()) {
		lastmincol_ = best;
	}
}

void Column::draw (ostream& os)
{
  int          size = minicols_.size ();
  stringstream ss;
  string       val;

  ss << no_;
  ss >> val;
  os << "c" << val << " [label=\"{<n0> G | <n0> H} | {<n0> Prec | {";
  for (int i = 0; i < size; i++)
    {
      minicols_[i]->draw (os);
      if (i < size - 1)
	os << " | ";
    }
  os << "} | <n0> Suiv} \"];" << endl;
}

void Column::draw_links (ostream& os)
{
  int size = minicols_.size ();

  for (int i = 0; i < size; i++)
    minicols_[i]->draw_links (os);
}

Minicol* Column::minicol_get_nodest (int no)
{
  	int sizecol = size ();
  	for (int i = 0; i < sizecol; i++) {
    	if (minicol_get (i)->to_get () != 0 && minicol_get (i)->to_get ()->no_get () == no) {
    		return minicol_get (i);
    	}
    }
  	return 0;
}

bool Column::goal_activity_setted () 
{
	int nb = minicols_.size ();
	for (int i = 0; i < nb; i++) {
		if (minicols_[i]->goal_activity_setted ()) {
			return true;	
		}
    }
	return false; 
}

void Column::goal_activity_set (double val) 
{
	int nb = minicols_.size ();
	for (int i = 0; i < nb; i++) {
		minicols_[i]->goal_activity_set (val);
    }
}

void Column::goal_activity_unset () 
{ 
	int nb = minicols_.size ();
	for (int i = 0; i < nb; i++) {
		minicols_[i]->goal_activity_unset ();
    }
}

void Column::inhib (bool val) 
{
	int nb = minicols_.size ();
	for (int i = 0; i < nb; i++) {
		minicols_[i]->inhib (val); 
    }
}

double Column::mean_inf_activation () 
{
	int nb = minicols_.size ();
	double sum = 0;
	for (int i = 0; i < nb; i++) {
		sum += minicols_[i]->inf_get ().output (); 
    }
	return sum / nb; 
}

double Column::mean_sup_activation () 
{
	int nb = minicols_.size ();
	double sum = 0;
	for (int i = 0; i < nb; i++) {
		sum += minicols_[i]->sup_get ().output (); 
    }
	return sum / nb;  
}

bool Column::activated () 
{
	int nb = minicols_.size ();
	for (int i = 0; i < nb; i++) {
		if (minicols_[i]->inf_get ().activated ()) {
			return true;
		} 
    }
	return false;  
}

Coord* Column::mean_position ()
{
	int nb = minicols_.size ();
	Coord* p = new Coord;
	for (int i = 0; i < nb; i++) {
		*p += minicols_[i]->pos_get (); 
    }
    *p /= nb;
    return p;  
}

bool operator== (Column& c1, Column& c2)
{
  return c1.no_get() == c2.no_get();
}

