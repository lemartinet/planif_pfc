#ifndef COLUMN_HH_
# define COLUMN_HH_
#include "action.hh"
#include "hippo.hh"
#include "neuralnet.hh"

class Columns;
class Minicol;
class Coord;

class Column
{
public:
	Column (Neuralnet& net, Hippo& state, int no);

	~Column ();

  	double mean_inf_activation ();
  	double mean_sup_activation ();
  	bool activated ();

  inline
  Minicol* minicol_get (int i) { return minicols_[i]; }

  inline
  int size () { return minicols_.size (); }

  inline
  int no_get () { return no_; }

  inline
  Minicol* last_mincol_get () { return lastmincol_; }

  inline
  string& path_get () { return path_; }
  
	bool goal_activity_setted ();
	void goal_activity_set (double val);
	void goal_activity_unset ();
	void inhib (bool val);

	void learn_set (bool learn);
 
	Minicol* lateral_learning (Action* action, Column& dest, bool increase, string & message);

  void del_minicols (Column& dest);
  
  Minicol* minicol_get (Action& action);

  void synch ();

  void draw (ostream& os);
  void draw_links (ostream& os);

  Minicol* minicol_get_nodest (int no);

	Minicol* best_minicol ();
	Minicol* best_mean_minicol ();
	
	Coord* mean_position ();


private:
  int                no_;
  string             path_;
  Neuralnet*         net_;
  Hippo*             hippo_state_;
  Minicol*           lastmincol_;
  vector<Minicol*>  minicols_;
};

bool operator== (Column& c1, Column& c2);

#endif
