QTDIR_INC = /usr/include/qt4
MY_INC	  = -Icontroller -Igui -Insolver -Itools -I.
QT_INC 	  = -I$(QTDIR_INC)/QtCore -I$(QTDIR_INC)/QtGui -I$(QTDIR_INC)

CFLAGS    = -W $(QT_INC) $(MY_INC) -g -O3 -pg 
LIBRARIES = -lQtGui -lQtCore -gdb -pg

CC_SOURCES = \
	controller/action.cc\
	controller/behavior.cc\
	controller/device.cc\
	controller/obstacleavoidance.cc\
	nsolver/minicol.cc\
	nsolver/column.cc\
	nsolver/columns.cc\
	nsolver/computeunit.cc\
	nsolver/hippo.cc\
	nsolver/cell.cc\
	nsolver/neuron.cc\
	nsolver/neurosolver.cc\
	nsolver/egoaction.cc\
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
	gui/moc_node.cpp

include $(WEBOTS_HOME)/projects/default/controllers/Makefile.include

myclean:
	find . -name "*.o" | xargs -r rm
	find . -name "*.d" | xargs -r rm

ui:
	rm gui/boardwindow.hh
	find . -name "moc_*.c*[cp]" | xargs rm
	uic-qt4 gui/boardwindow.ui -o gui/boardwindow.hh
	moc-qt4 gui/controlrobot.hh -o gui/moc_controlrobot.cc
	moc-qt4 gui/boardwidget.hh -o gui/moc_boardwidget.cc
	moc-qt4 gui/node.h -o gui/moc_node.cpp

myall: ui all
