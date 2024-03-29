#ifndef CONTROLROBOT_HH
# define CONTROLROBOT_HH

#include <QObject>
#include "graphwidget.h"

class Hippo;
class Node;
class Edge;
class Neurosolver;
class Column;
class Minicol;
class Columns;

class ControlRobot : private QObject
{
	Q_OBJECT
	
public:
	ControlRobot (const Hippo& hippo, Neurosolver& neuro);
	~ControlRobot () {};

	void update ();
	void show_sup () { show_inf_sup_or_state_ = 2; }  
	void show_inf () { show_inf_sup_or_state_ = 1; }
	void show_state () { show_inf_sup_or_state_ = 0; }
	GraphWidget* widget_get () { return &widget_; }
	void show_pc ();
	void show_col ();
	
private:
	void update_col_appearance (Node& node, Column& col);
	void update_mincols ();
	void update_cols ();	
	void update_cells ();

private:
	const Hippo& hippo_;
	Neurosolver& neuro_;
	GraphWidget widget_;
	Node* robot_;
	int show_inf_sup_or_state_;
	bool show_pc_;
	bool show_col_;
	int refresh_;
  
private slots:
	void slot_setgoalcol (int no);
};

#endif
