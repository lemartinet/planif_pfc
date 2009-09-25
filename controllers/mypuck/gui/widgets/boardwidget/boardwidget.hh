#ifndef BOARDWIDGET_HH
# define BOARDWIDGET_HH

#include "boardwindow.hh"

class Gui;

class BoardWidget : public QMainWindow, public Ui::BoardWindow
{
  Q_OBJECT

public:
  BoardWidget (Gui& gui) : gui_(&gui)
  	{ setupUi (this); }

protected:
  void closeEvent(QCloseEvent *);

private:
  Gui* gui_;

private slots:
  void on_execution_behavior_run_clicked ();
  void on_execution_behavior_pause_clicked ();
  void on_execution_behavior_reset_clicked ();
  void on_execution_view_activities_clicked ();
  void on_show_state_clicked ();
  void on_show_inf_clicked ();
  void on_show_sup_clicked ();
};

#endif
