QTDIR_INC = /usr/include/qt4
MYINC	  = -Icontroller -Igui -Igui/controlors -Igui/widgets/graphwidget -Insolver -Itools -Igui/widgets/boardwidget -I. -Itesty
INCQT 	  = -I$(QTDIR_INC)/QtCore -I$(QTDIR_INC)/QtGui -I$(QTDIR_INC)
LINKING   = -lQtGui -lQtCore -gdb -lCppController -lController

CFLAGS    = -W $(INCQT) $(MYINC) -g -O2 -I/usr/local/webots/include/controllers/cpp
LIBRARIES = $(LINKING)

CC_SOURCES = \
	controller/action.cc\
	controller/behavior.cc\
	controller/device.cc\
	controller/obstacleavoidance.cc\
	nsolver/minicol.cc\
	nsolver/column.cc\
	nsolver/columns.cc\
	nsolver/moc_columns.cc\
	nsolver/computeunit.cc\
	nsolver/synapse.cc\
	nsolver/hippo.cc\
	nsolver/moc_hippo.cc\
	nsolver/cell.cc\
	nsolver/neuralnet.cc\
	nsolver/neuron.cc\
	nsolver/neurosolver.cc\
	nsolver/egoactionpop.cc\
	nsolver/alloactionpop.cc\
	nsolver/dcell.cc\
	tools/coord.cc\
	tools/math.cc\
	tools/mystr.cc\
	tools/params.cc\
	gui/repere_converter.cc\
	gui/controlors/controlcols.cc\
	gui/controlors/moc_controlcols.cc\
	gui/controlors/controlrobot.cc\
	gui/controlors/controlcells.cc\
	gui/controlors/moc_controlcells.cc\
	gui/widgets/boardwidget/boardwidget.cc\
	gui/widgets/boardwidget/moc_boardwidget.cc\
	gui/gui.cc\
	testy/testy.cc\
	mypuck.cc

CPP_SOURCES = \
	gui/widgets/graphwidget/edge.cpp\
	gui/widgets/graphwidget/graphwidget.cpp\
	gui/widgets/graphwidget/moc_graphwidget.cpp\
	gui/widgets/graphwidget/node.cpp

include /usr/local/webots/projects/default/controllers/Makefile.include

myclean:
	find . -name "*.o" | xargs -r rm
	find . -name "*.d" | xargs -r rm

moc:
	find . -name "moc*.c*[cp]" | xargs rm
	moc-qt4 nsolver/columns.hh -o nsolver/moc_columns.cc
	moc-qt4 nsolver/hippo.hh -o nsolver/moc_hippo.cc
	moc-qt4 gui/controlors/controlcols.hh -o gui/controlors/moc_controlcols.cc
	moc-qt4 gui/controlors/controlcells.hh -o gui/controlors/moc_controlcells.cc
	moc-qt4 gui/widgets/boardwidget/boardwidget.hh -o gui/widgets/boardwidget/moc_boardwidget.cc
	moc-qt4 gui/widgets/graphwidget/graphwidget.h -o gui/widgets/graphwidget/moc_graphwidget.cpp
	
ui:
	rm gui/widgets/boardwidget/boardwindow.hh
	uic-qt4 gui/widgets/boardwidget/boardwindow.ui -o gui/widgets/boardwidget/boardwindow.hh
