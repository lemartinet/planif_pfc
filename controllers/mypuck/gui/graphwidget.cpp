/*****************************************************************************
**
** Copyright (C) 2006-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "graphwidget.h"
#include "edge.h"
#include "node.h"
#include "params.hh"
#include <QGraphicsScene>
#include <QWheelEvent>
#include <iostream>

GraphWidget::GraphWidget ()
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    setScene(scene);
    scene_ = scene;

    setCacheMode(CacheBackground);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorViewCenter);
    setWindowTitle(tr("Columns viewer"));
    
    // repere pour le monde de Webots : min (-0.64, -1.5), max (1.13, 0.0)
    double w_xmin = -0.64, w_ymin = -1.7, w_xmax = 1.13, w_ymax = 0.0;
    Coord::webots_coord_set (w_xmin, w_ymin, w_xmax,  w_ymax);
	// repere pour le gui : min (0, 0), max (500, 500)
	double g_xmin = 0, g_ymin = 0, g_xmax = 500, g_ymax = 500;
	Coord::gui_coord_set (g_xmin, g_ymin, g_xmax, g_ymax);
    setMinimumSize(g_xmax, g_ymax);

	SIZE_POP = Params::get_int("SIZE_POP");
	col_nodes_ = new Node*[SIZE_POP];
	cell_nodes_ = new Node*[SIZE_POP];
	edges_ = new Edge**[SIZE_POP];
	for (int i = 0; i < SIZE_POP; ++i) {
		col_nodes_[i] = cell_nodes_[i] = 0;
		edges_[i] = new Edge*[SIZE_POP];
		for (int j = 0; j < SIZE_POP; ++j)
			edges_[i][j] = 0;
	}
}

GraphWidget::~GraphWidget ()
{
	// scene_ supprime automatiquement tous les items de la scene
	// (Edge, Node, ...) -> ne pas les supprimer ailleurs
	delete scene_;
	delete [] col_nodes_;
	delete [] cell_nodes_;
}

Node* GraphWidget::node_get (NodeType type, int no) const
{
	if (type == COL)
		return col_nodes_[no];
	else if (type == CELL)
		return cell_nodes_[no];
	else
		return 0;
}

void GraphWidget::type_hide (NodeType type)
{
	if (type == COL)
		for (int i = 0; i < SIZE_POP; ++i) {
			if (col_nodes_[i] != 0)
				col_nodes_[i]->hide();
		}
	else if (type == CELL)
		for (int i = 0; i < SIZE_POP; ++i) {
			if (cell_nodes_[i] != 0)
				cell_nodes_[i]->hide();
		}
}

void GraphWidget::type_show (NodeType type)
{
	if (type == COL)
		for (int i = 0; i < SIZE_POP; ++i) {
			if (col_nodes_[i] != 0)
				col_nodes_[i]->show();
		}
	else if (type == CELL)
		for (int i = 0; i < SIZE_POP; ++i) {
			if (cell_nodes_[i] != 0)
				cell_nodes_[i]->show();
		}
}

Edge* GraphWidget::edge_get (NodeType type, int from, int to) const
{
	if (type == COL)
		return edges_[from][to];
	else
		return 0;
}

void GraphWidget::edge_hide (NodeType type, int to)
{
	if (type != COL)
		return;
	for (int i = 0; i < SIZE_POP; ++i) {
		for (int j = 0; j < SIZE_POP; ++j) {
			if (edges_[i][j] != 0)
				edges_[i][j]->hide();
		}
	}
}

void GraphWidget::edge_show (NodeType type, int to)
{
	if (type != COL)
		return;
	for (int i = 0; i < SIZE_POP; ++i) {
		for (int j = 0; j < SIZE_POP; ++j) {
			if (edges_[i][j] != 0)
				edges_[i][j]->show();
		}
	}
}

Node* GraphWidget::add_node (NodeType type, int no)
{
	Node* node = new Node (type, no);
	scene_->addItem (node);
	if (type == COL)
		col_nodes_[no] = node;
	else if (type == CELL)
		cell_nodes_[no] = node;
	return node;
}

Edge* GraphWidget::add_edge (NodeType type, int from, int to)
{
  Node* node1 = node_get (type, from);
  Node* node2 = node_get (type, to);
  Edge*  edge = 0;

  if (!node1 || !node2)
    return 0;
  edge = new Edge (node1, node2);
  scene_->addItem (edge);
  edges_[from][to] = edge;
  return edge;
}

void GraphWidget::keyPressEvent(QKeyEvent *event)
{
  QGraphicsView::keyPressEvent(event);
}

void GraphWidget::timerEvent(QTimerEvent *event)
{
  QGraphicsView::timerEvent(event);
}

void GraphWidget::wheelEvent(QWheelEvent *event)
{
  scaleView(pow((double)2, -event->delta() / 240.0));
}

void GraphWidget::closeEvent (QCloseEvent*)
{
  hide ();
}

void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    QRectF sceneRect = this->sceneRect();
    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersect(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);

    // Text
    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
                    sceneRect.width() - 4, sceneRect.height() - 4);
    QString message(tr(""));
    
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(14);
    painter->setFont(font);
    painter->setPen(Qt::lightGray);
    painter->drawText(textRect.translated(2, 2), message);
    painter->setPen(Qt::black);
    painter->drawText(textRect, message);
    
//    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
//                    sceneRect.width() - 4, sceneRect.height() - 4);
//    QString message(tr("Cortical network"));
//    QFont font = painter->font();
//    font.setPointSize(14);
//    painter->setFont(font);
//    painter->setPen(Qt::black);
//    painter->drawText(textRect, message);
}

void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}
