QTDIR_INC = /usr/include/qt4
MY_INC	  = -Icontroller -Igui -Insolver -Itools -I.
QT_INC 	  = -I$(QTDIR_INC)/QtCore -I$(QTDIR_INC)/QtGui -I$(QTDIR_INC)

CFLAGS    = -W $(QT_INC) $(MY_INC) -g -O2 
LIBRARIES = -lQtGui -lQtCore -gdb -lCppController

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
	tools/log.cc\
	tools/logger.cc\
	gui/controlrobot.cc\
	gui/moc_controlrobot.cc\
	gui/boardwidget.cc\
	gui/moc_boardwidget.cc\
	gui/gui.cc\
	mypuck.cc

CPP_SOURCES = \
	gui/edge.cpp\
	gui/node.cpp \
	gui/graphwidget.cpp\
	gui/moc_graphwidget.cpp

include $(WEBOTS_HOME)/projects/default/controllers/Makefile.include

myclean:
	find . -name "*.o" | xargs -r rm
	find . -name "*.d" | xargs -r rm

moc:
	find . -name "moc*.c*[cp]" | xargs rm
	moc-qt4 nsolver/columns.hh -o nsolver/moc_columns.cc
	moc-qt4 nsolver/hippo.hh -o nsolver/moc_hippo.cc
	moc-qt4 gui/controlrobot.hh -o gui/moc_controlrobot.cc
	moc-qt4 gui/boardwidget.hh -o gui/moc_boardwidget.cc
	moc-qt4 gui/graphwidget.h -o gui/moc_graphwidget.cpp
	
ui:
	rm gui/boardwindow.hh
	uic-qt4 gui/boardwindow.ui -o gui/boardwindow.hh
