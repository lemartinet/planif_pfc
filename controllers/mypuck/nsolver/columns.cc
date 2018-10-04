#include "columns.hh"
#include "column.hh"
#include "action.hh"
#include "neuron.hh"
#include "params.hh"
#include <algorithm>
#include <iostream>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/if.hpp>

using namespace boost::lambda;

Columns::Columns () : cpt_(0), newcol_(0), learning_(false) {};

Columns::~Columns ()
{
	for_each (colomns_.begin (), colomns_.end (), bind (delete_ptr(), _1));
}

Column* Columns::best_state_col (int level) const
{
	Column* best_col = 0;
	vector<Column*>::const_iterator it;
	for (it = colomns_.begin (); it != colomns_.end (); it++) {
		if ((*it)->level_get () != level) {
			continue;
		}
		if (!best_col || (best_col->state_activation () < (*it)->state_activation ())) {
			best_col = *it;	
		}
	}
	return best_col;
}

Minicol* Columns::best_minicol (int level) const
{
	Minicol* best_minicol = 0;
	vector<Column*>::const_iterator it;
	for (it = colomns_.begin (); it != colomns_.end (); it++) {
		if ((*it)->level_get () != level) {
			continue;
		}
		Minicol* tmp = (*it)->best_minicol ();
		if (!best_minicol || (tmp && best_minicol->activation () < tmp->activation ())) {
			best_minicol = tmp;	
		}
	}
	return best_minicol;
}

Column& Columns::add_colomn (int level, const vector<ComputeUnit*>& pop, bool draw, ComputeUnit* ego_action)
{
	Column* col = new Column (net_, *this, pop, cpt_, level, draw, ego_action);
	cpt_++;
	colomns_.push_back (col);
	pop_.push_back (&col->state_get());
	newcol_ = col;
	
	if (draw) {
		emit sig_addcol (col->no_get ());
	}
	
	return *col;
}

double Columns::nb_spiking_cells (int level) const
{
//	return count_if (pop_.begin (), pop_.end (), 
//		bind (&ComputeUnit::level_get, _1) == level && bind (&ComputeUnit::spiking, _1));
	double sum = 0;
	vector<ComputeUnit*>::const_iterator it;
	for (it = pop_.begin (); it != pop_.end (); it++) {
		if ((*it)->level_get () == level) {
			sum += (*it)->output ();	
		}
	}
	return sum;
}

void Columns::synch ()
{
	if (learning_) {
    	net_.synch_learn ();
  	}
  	else {
    	net_.synch ();
  	}
  	for_each (colomns_.begin (), colomns_.end (), bind (&Column::synch, _1));
  	newcol_ = 0;
  	
  	winner_set (0);
  	winner_set (1);
//  	printf ("nb_spiking_cells: %f\n", nb_spiking_cells (0));
}

void Columns::draw (ostream& os) const
{
	for_each (colomns_.begin (), colomns_.end (), bind (&Column::draw, _1, var (os)));
}

void Columns::draw_links (ostream& os) const
{
	for_each (colomns_.begin (), colomns_.end (), bind (&Column::draw_links, _1, var (os)));
	net_.draw_links (os);
}

Column* Columns::nocol_get (int no) const
{
	vector<Column*>::const_iterator it;
	it = find_if (colomns_.begin (), colomns_.end (), bind (&Column::no_get, _1) == no);
	return (it == colomns_.end ()) ? 0 : *it;
}

void Columns::lateral_learning (Column& from, const Column& to, Action* action, bool increase, string & message)
{
	// fonction intermediaire uniquement utile pour l'affichage (sig_addlink)
	bool new_minicol = from.lateral_learning (action, to, increase, message);
	if (new_minicol) {
		emit sig_addlink (from.no_get (), to.no_get ());
		//cout << "from " << from.no_get () << " to " << to.no_get () << " " << action->angle_get() << endl;
	}
}

void Columns::show_activities (int level) const
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

void Columns::winner_set (int level)
{
	int winner = -1;
	Column* col = best_state_col (level);
	if (col && col->state_get ().spiking ()) {
		winner = col->no_get ();
	}
	vector<Column*>::iterator it;
	for (it = colomns_.begin (); it != colomns_.end (); it++) {
		if ((*it)->no_get () == winner) {
			(*it)->winner_set (true);
		}
		else if ((*it)->level_get () == level) {
			(*it)->winner_set (false);	
		}
	}
//	printf("winner level %d is %d\n", level, winner);
}
