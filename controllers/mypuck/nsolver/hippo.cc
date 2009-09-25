#include "hippo.hh"
#include "math.hh"
#include "params.hh"
#include "cell.hh"
#include "computeunit.hh"

extern Params* params;

Hippo::~Hippo ()
{
  int nbcells = cellmap_.size ();
  int i = 0;

  for (i = 0; i < nbcells; i++)
    delete cellmap_[i];
}

void Hippo::cell_add (Coord pos)
{
	Coord noised_pos (pos.x_get () + (drand48 () - 0.5) / 10, pos.y_get () + (drand48 () - 0.5) / 10);
	Cell* cell = new Cell (noised_pos, cpt_++, this);

	cellmap_.push_back (cell);
	lastadded_ = cell;
	emit sig_addcell (cell->no_get ());
}

bool Hippo::synch (const Coord & signal)
{
	// Minimum activation of a cell, under wich a new cell should be created.
	static const double CELL_FIRE_MIN = params->get_double("CELL_FIRE_MIN");
	static const double CELL_SIMULT_MIN = params->get_double("CELL_SIMULT_MIN");
	int size = cellmap_.size ();
	bool winner = false;

	position_.x_set (signal.x_get ());
	position_.y_set (signal.y_get ());

	lastadded_ = 0;
	
	// TODO: tester avec le meme critere d'ajout que Arleo2004
//	int sum = 0;
	for (int i = 0; i < size; i++) {
		Cell* cell = dynamic_cast<Cell *>(cellmap_[i]);
		cell->compute();
		if (cell->output () > CELL_FIRE_MIN) {
			winner = true;
		}
//		sum += (cellmap_[i]->compute () >= CELL_FIRE_MIN)?1:0;
	}
//	printf ("sum: %d\n", sum);
	if (iadd_ && !winner) {
//	if (sum < CELL_SIMULT_MIN) {
		cell_add (signal);
		Cell* cell = dynamic_cast<Cell *>(cellmap_[cellmap_.size () - 1]);
		cell->compute ();
		return true;
	}
	return false;
}

void Hippo::draw (ostream& os)
{
  int size = cellmap_.size ();

  os << "subgraph cluster_hippo" << " {" << endl;
  os << "style=filled;" << endl;
  os << "color=grey;" << endl;
  os << "node [style=filled,color=white];" << endl;
  for (int i = 0; i < size; i++) {
  	double cell_output = cellmap_[i]->output (); 
    os << "c" << &cell_output << " [label=\"" << i << ":" << cellmap_[i]->output () << "\"]" << endl;
  } 
  os << "label = \"hippocampe\";" << endl;
  os << "}" << endl;
}

void Hippo::reset ()
{
  int nbcells = cellmap_.size ();

  for (int i = 0; i < nbcells; i++)
    delete cellmap_[i];

  cellmap_.clear ();
  lastadded_ = 0;
  cpt_ = 0;
  emit sig_reset ();
}

 Cell* Hippo::cell_get (int cell) 
 {
 	return dynamic_cast<Cell*>(cellmap_[cell]); 
 }
