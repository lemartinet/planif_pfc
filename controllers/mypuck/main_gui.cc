#include <QApplication>
#include <QTimer>
#include <QObject>
#include "boardwidget.hh"
#include "behavior.hh"
#include "computation.hh"

int main(int argc, char** argv)
{
	Computation comp;
	QApplication app(argc, argv);
	BoardWidget widget(comp.behavior_get());
	widget.show();
	comp.start();
	return app.exec();
}
