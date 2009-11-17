#include "boardwidget.hh"
#include "behavior.hh"
#include "params.hh"
#include "column.hh"
#include "neuron.hh"
#include <iostream>

BoardWidget::BoardWidget (Behavior& behavior) : 
	robotgui_(behavior.neurosolver_get ().hippo_get (), behavior.neurosolver_get ())
{ 
	setupUi (this);
	robotgui_.widget_get ()->setParent (network);
	robotgui_.widget_get ()->move (230,10);
}

void BoardWidget::closeEvent (QCloseEvent *) {}

void BoardWidget::on_show_state_clicked () 
{
	robotgui_.show_state ();
}

void BoardWidget::on_show_inf_clicked () 
{
	robotgui_.show_inf ();
}

void BoardWidget::on_show_sup_clicked () 
{
	robotgui_.show_sup ();
}

void BoardWidget::on_show_pc_clicked () 
{
	robotgui_.show_pc ();
}

void BoardWidget::on_show_col_clicked () 
{
	robotgui_.show_col ();
}

void BoardWidget::on_show_col2_clicked () 
{
	robotgui_.show_col2 ();
}

void BoardWidget::on_modulation_spin_valueChanged(const QString& text){
	Params::set("LVL1_MODULATION", text.toStdString());
	if (Behavior::behavior_get().nb_trial_get() < 6)
		return; 
	const vector<Column*>& cols = 
		Behavior::behavior_get().neurosolver_get().cols_get().pop_col_get();
	vector<Column*>::const_iterator it;
	for (it = cols.begin(); it != cols.end(); it++) {
		Column* col = *it;
		if (col->level_get() != 0) {
			continue;	
		}
		Neuron& sup = col->sup_get();
		double* w = sup.syn_mod_get(); 
		if (w != 0) {
			cout << "update: " << text.toDouble() << endl; 
			*w = text.toDouble();
		}
	}
}
