#include <iostream>
#include "columns.hh"
#include "neuron.hh"
#include "params.hh"


Colomns::Colomns () : cpt_(0), mode_(LEARNING), newcol_(0), newmincol_(0), learning_(false) 
{
	string s = "";
};


Colomns::~Colomns ()
{
  int size = colomns_.size ();

  for (int i = 0; i < size; i++) {
    delete colomns_[i];
  }
}

void Colomns::mode_set (enum e_mode mode)
{
  	int size = colomns_.size ();

  	if (mode_ != mode) {
  		mode_ = mode;
  		for (int i = 0; i < size; i++) {
  			colomns_[i]->mode_set (mode);
  		}
    }
}

Colomn* Colomns::best_state_col (int level)
{
	int      size = colomns_.size ();
	double   max = 0.0;
	double   activation = 0.0;
	int      best = 0;
	
	if (size == 0) {
		return 0;
	}
	for (int i = 0; i < size; i++) {
		activation = colomns_[i]->state_activation ();
		if (colomns_[i]->level_get () == level && activation > max) {
			max = activation;
			best = i;
		}
	}
	if (max > 0) {
		return colomns_[best];
	}
	else return 0;
}

// Colomn* Colomns::best_goal_col ()
// {
//   int      size = colomns_.size ();
//   double   max = 0.0;
//   double   activation = 0.0;
//   int      best = 0;

//   if (size == 0)
//     return 0;
//   for (int i = 0; i < size; i++)
//     {
//       activation = colomns_[i]->goals_activation ();
//       if (activation > max)
// 	{
// 	  max = activation;
// 	  best = i;
// 	}
//     }
//   return colomns_[best];
// }

Colomn& Colomns::add_colomn (int level, vector<ComputeUnit*>& pop, bool draw)
{
	Colomn* col = new Colomn (net_, pop, cpt_, level, draw);
	cpt_++;
	colomns_.push_back (col);
	pop_.push_back (&col->state_get());
	newcol_ = col;
	
	if (draw) {
		emit sig_addcol (col->no_get ());
	}
	
	return *col;
}

void Colomns::del_colomn (Colomn* col)
{
  int                         nbcols = colomns_.size ();
  vector<Colomn *>::iterator  it;  
  int                         no = col->no_get ();

  //robot_console_printf ("del_colomn (%i)\n", no);
  if (!col)
    return;
  for (int i = 0; i < nbcols; i++)
    colomns_[i]->del_minicols (*col);
  it = find (colomns_.begin (), colomns_.end (), col);
  assert (it != colomns_.end ());
  delete *it;
  colomns_.erase (it);
  emit sig_delcol (no);
}

void Colomns::synch ()
{
  	int nbcols = colomns_.size ();
  
  	if (learning_) {
    	net_.synch_learn ();
  	}
  	else {
    	net_.synch ();
  	}
  	for (int i = 0; i < nbcols; i++) {
    	colomns_[i]->synch ();
  	}
  	newcol_ = 0;
  	newmincol_ = 0;
}

void Colomns::draw (ostream& os)
{
  int size = colomns_.size ();

  for (int i = 0; i < size; i++)
    colomns_[i]->draw (os);
}

void Colomns::draw_links (ostream& os)
{
  int size = colomns_.size ();

  for (int i = 0; i < size; i++)
    colomns_[i]->draw_links (os);
  net_.draw_links (os);
}

void Colomns::propagate ()
{
	// TODO: a ameliorer : attendre une "epsilon-convergence" du reseau
	// tenir compte du nb de propagation qu'on peut faire en TIME_STEP ms ???
  	for (int i = 0; i < 100; i++) {
  		synch ();
  	}
}

Colomn* Colomns::nocol_get (int no)
{
  int sizecols = size ();

  for (int i = 0; i < sizecols; i++)
    {
      if (col_get (i)->no_get () == no)
	return col_get (i);
    }
  return 0;
}

bool Colomns::lateral_learning (Colomn& from, Colomn& to, Action* action, bool increase, string & message)
{
	Minicol*  mincol1 = from.lateral_learning (action, to, increase, message);
  	if (!mincol1) {
    	return false;
  	}
  	newmincol_ = mincol1;
  	emit sig_addlink (from.no_get (), to.no_get ());
  	//cout << "from " << from.no_get () << " to " << to.no_get () << " " << action->angle_get() << endl;
  	return true;
}

void Colomns::reset ()
{
  int size = colomns_.size ();
  
  cpt_ = 0;
  for (int i = 0; i < size; i++)
    del_colomn (colomns_[0]);
    //delete colomns_[i];
  //colomns_.clear ();
  assert (net_.size () == 0);
  newcol_ = 0;
  newmincol_ = 0;
  //reset_ = true;
}

void Colomns::show_activities (int level)
{
	bool written = false;
	int size = colomns_.size ();
	for (int i = 0; i < size; i++) {
		if (colomns_.at (i)->level_get () == level) {
			printf("%d/%f ", i, colomns_.at (i)->state_activation ());
			written = true;	
		}
	}
	if (written) {
		printf("\n");
	}
}

void Colomns::winner_set (int col_winner)
{
//	cancel_inhib ();
	int size = colomns_.size ();
	for (int i = 0; i < size; i++) {
		if (colomns_.at (i)->no_get () != col_winner
			&& colomns_.at (i)->level_get () == 1) {
			colomns_.at (i)->winner_set (false);
		}
		else {
			colomns_.at (i)->winner_set (true);	
		}
	}
}

void Colomns::winner_reset ()
{
	int size = colomns_.size ();
	for (int i = 0; i < size; i++) {
		if (colomns_.at (i)->level_get () == 1) {
			colomns_.at (i)->winner_set (false);
		}
	}	
}
