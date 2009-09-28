#include <iostream>
#include "columns.hh"
#include "params.hh"


Columns::~Columns ()
{
  int size = columns_.size ();

  for (int i = 0; i < size; i++)
    delete columns_[i];
}

void Columns::learn_set (bool learn)
{
  	int size = columns_.size ();
  	if (learn_ != learn) {
  		learn_ = learn;
  		for (int i = 0; i < size; i++) {
  			columns_[i]->learn_set (learn);
  		}
    }
}

Column* Columns::best_state_col ()
{
  int      size = columns_.size ();
  double   max = 0.0;
  double   activation = 0.0;
  int      best = 0;

  if (size == 0)
    return 0;
  for (int i = 0; i < size; i++)
    {
    	// TODO: à quoi ca sert ???
      activation = columns_[i]->mean_inf_activation ();
      if (activation > max)
	{
	  max = activation;
	  best = i;
	}
    }
  return columns_[best];
}

Column& Columns::add_column ()
{
  Column*  col = new Column (net_, *state_, cpt_);

  cpt_++;
  columns_.push_back (col);
  newcol_ = col;
  emit sig_addcol (col->no_get ());

  return *col;
}

void Columns::del_column (Column* col)
{
  int                         nbcols = columns_.size ();
  vector<Column *>::iterator  it;  
  int                         no = col->no_get ();

  //robot_console_printf ("del_column (%i)\n", no);
  if (!col)
    return;
  for (int i = 0; i < nbcols; i++)
    columns_[i]->del_minicols (*col);
  it = find (columns_.begin (), columns_.end (), col);
  assert (it != columns_.end ());
  delete *it;
  columns_.erase (it);
  emit sig_delcol (no);
}

void Columns::synch ()
{
  	int nbcols = columns_.size ();
  
  	if (learn_) {
    	net_.synch_learn ();
  	}
  	else {
    	net_.synch ();
  	}
  	for (int i = 0; i < nbcols; i++) {
    	columns_[i]->synch ();
  	}
  	newcol_ = 0;
  	newmincol_ = 0;
}

void Columns::draw (ostream& os)
{
  int size = columns_.size ();

  for (int i = 0; i < size; i++)
    columns_[i]->draw (os);
}

void Columns::draw_links (ostream& os)
{
  int size = columns_.size ();

  for (int i = 0; i < size; i++)
    columns_[i]->draw_links (os);
  net_.draw_links (os);
}

void Columns::propagate ()
{
	// a ameliorer : attendre une "epsilon-convergence" du reseau
	// tenir compte du nb de propagation qu'on peut faire en TIME_STEP ms ???
  	for (int i = 0; i < 100; i++) {
  		synch ();
  	}
}

Column* Columns::nocol_get (int no)
{
  int sizecols = size ();

  for (int i = 0; i < sizecols; i++)
    {
      if (col_get (i)->no_get () == no)
	return col_get (i);
    }
  return 0;
}

bool Columns::lateral_learning (Column& from, Column& to, Action* action, bool increase, string & message)
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

void Columns::reset ()
{
  int size = columns_.size ();
  
  cpt_ = 0;
  for (int i = 0; i < size; i++)
    del_column (columns_[0]);
    //delete columns_[i];
  //columns_.clear ();
  assert (net_.size () == 0);
  newcol_ = 0;
  newmincol_ = 0;
  //reset_ = true;
}

void Columns::inhib_all ()
{
  int size = columns_.size ();

  for (int i = 0; i < size; i++)
    columns_[i]->inhib (true);
}
