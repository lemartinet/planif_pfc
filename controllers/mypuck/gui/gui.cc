#include "gui.hh"
#include "boardwidget.hh"

Gui::Gui (int& argc, char**& argv, Behavior& behavior) : QApplication (argc, argv)
{
  	eventloop_ = new QEventLoop ();
  	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
	boardwidget_ = new BoardWidget (behavior);
  	boardwidget_->show ();
}

Gui::~Gui ()
{
	delete boardwidget_;
	delete eventloop_;
}

void Gui::synch ()
{
	boardwidget_->update ();
	boardwidget_->show ();
	eventloop_->processEvents(QEventLoop::AllEvents);
}
