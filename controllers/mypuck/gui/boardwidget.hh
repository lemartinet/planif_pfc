#ifndef BOARDWIDGET_HH
# define BOARDWIDGET_HH

#include "boardwindow.hh"
#include "controlrobot.hh"

class Behavior;

class BoardWidget : public QMainWindow, public Ui::BoardWindow
{
  Q_OBJECT

public:
  BoardWidget (Behavior& behavior);

  void update () { robotgui_.update (); }

protected:
  void closeEvent(QCloseEvent *);

private:
  ControlRobot robotgui_;

private slots:
	void on_show_state_clicked ();
	void on_show_inf_clicked ();
	void on_show_sup_clicked ();
	void on_show_pc_clicked ();
	void on_show_col_clicked ();
};

#endif
