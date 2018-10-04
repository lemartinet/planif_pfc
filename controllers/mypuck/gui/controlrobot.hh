#ifndef CONTROLROBOT_HH
# define CONTROLROBOT_HH

#include <QObject>

class GraphWidget;
class RobotDevice;
class Hippo;
class Node;
class Edge;
class Neurosolver;
class Column;
class Minicol;
class Columns;

// pour afficher les cellules de lieux dans QT
#define SHOW_CELLS    false
#define COLOR_STATE          Qt::darkYellow
#define COLOR_GOAL           Qt::darkRed
#define COLOR_SPIKING        Qt::darkBlue

class ControlRobot : private QObject
{
	Q_OBJECT
	
public:
	ControlRobot (const RobotDevice& robot_device, const Hippo& hippo, Neurosolver& neuro, GraphWidget& widget);
	~ControlRobot () {};

	void update ();
	void show_sup () { show_inf_sup_or_state_ = 2; }  
	void show_inf () { show_inf_sup_or_state_ = 1; }
	void show_state () { show_inf_sup_or_state_ = 0; }
	
private:
	void update_col_appearance (Node& node, Column& col);
	void update_mincol_appearance (Edge& edge, Minicol& mincol);
	void update_mincols_appearance ();
	void highlight_current_mincol ();
	void update_cols ();	
	void update_cells ();

private:
	const RobotDevice& robot_device_;
	const Hippo& hippo_;
	Neurosolver& neuro_;
	GraphWidget& widget_;
	Node* robot_;
	int show_inf_sup_or_state_;
  
private slots:
	void slot_addcell (int no);
	void slot_addcol (int no);
	void slot_addlink (int from, int to);
	void slot_setgoalcol (int no);
};

#endif
