#include "gui.hh"
#include "behavior.hh"

void Gui::clean ()
{
  if (boardwidget_)
    delete boardwidget_;
  if (colscontrol_)
    delete colscontrol_;
  if (colswidget_)
    delete colswidget_;
  if (robotcontrol_)
    delete robotcontrol_;
  if (cellscontrol_)
    delete cellscontrol_;
  if (eventloop_)
    delete eventloop_;
  if (app_)
    delete app_;
}

Gui::~Gui ()
{
  clean ();
}

void Gui::init (int& argc, char**& argv, Behavior& behavior, RobotDevice& robot) 
{
  	clean ();
  	app_ = new QApplication (argc, argv);
  	eventloop_ = new QEventLoop ();
  	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
  	boardwidget_ = new BoardWidget (*this);
  	colswidget_ = new GraphWidget ();
  	boardwidget_->show ();
  	//colswidget_->show ();
  	colscontrol_ = new ControlCols (behavior.neurosolver_get ().cols_get (), *colswidget_);
  	robotcontrol_ = new ControlRobot (robot, *colswidget_);
  	cellscontrol_ = new ControlCells (behavior.neurosolver_get ().hippo_get (), *colswidget_);
}

void Gui::draw ()
{
  if (eventloop_)
    eventloop_->processEvents(QEventLoop::AllEvents);
}

void Gui::update ()
{
  if (colscontrol_)
    colscontrol_->update ();
  if (robotcontrol_)
    robotcontrol_->update ();
  if (cellscontrol_)
    cellscontrol_->update ();
}
