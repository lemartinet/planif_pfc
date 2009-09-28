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

#include <QDebug>
#include <QGraphicsScene>
#include <QWheelEvent>

#include <iostream>
#include <math.h>

GraphWidget::GraphWidget ()
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(0, 0, 50, 50);
    setScene(scene);
    setCacheMode(CacheBackground);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorViewCenter);
    setWindowTitle(tr("Columns viewer"));
    setMouseTracking (true);
//    scale(0.8, 0.8);
    //setMinimumSize(400, 400);
    scene_ = scene;
    
    // repere pour le monde de Webots : min (-0.64, -1.5), max (1.13, 0.0)
    double w_xmin = -0.64, w_ymin = -1.7, w_xmax = 1.13, w_ymax = 0.0;
    Coord::webots_coord_set (w_xmin, w_ymin, w_xmax,  w_ymax);
	// repere pour le gui : min (0, 0), max (500, 500)
	double g_xmin = 0, g_ymin = 0, g_xmax = 500, g_ymax = 500;
	Coord::gui_coord_set (g_xmin, g_ymin, g_xmax, g_ymax);
	// taille du graph
	scene_->setSceneRect(g_xmin, g_ymin, fabs (g_xmax - g_xmin), fabs (g_ymax - g_ymin));
}

GraphWidget::~GraphWidget ()
{
	// supprime automatiquement tous les items de la scene
	// (Edge, Node, ...) -> ne pas les supprimer ailleurs
	delete scene_;
}

void GraphWidget::itemMoved ()
{
}

Node* GraphWidget::node_get (NodeType type, int no)
{
  Node* node = 0;

  foreach (QGraphicsItem *item, scene()->items())
    {
      if ((node = qgraphicsitem_cast<Node *>(item)) && node->nodetype_get () == type && node->no_get () == no)
	return node;
    }
  return 0;
}

void GraphWidget::type_del (NodeType type)
{
	foreach (QGraphicsItem *item, scene()->items()) {
		Node* node = qgraphicsitem_cast<Node*>(item);
		if (node && node->nodetype_get () == type) {
			node->hide ();
			scene()->removeItem (node);
		}
    }
}

void GraphWidget::edge_type_del (NodeType type)
{
	foreach (QGraphicsItem *item, scene()->items()) {
		Edge* edge = qgraphicsitem_cast<Edge*>(item);
		if (edge && edge->sourceNode ()->nodetype_get () == type && edge->destNode ()->nodetype_get () == type) {
			edge->hide ();
			scene()->removeItem (edge);
		}
    }
}

Edge* GraphWidget::edge_get (NodeType type, int from, int to)
{
  Edge*  edge = 0;

  foreach (QGraphicsItem *item, scene()->items())
    {
      if ((edge = qgraphicsitem_cast<Edge *>(item)) && edge->sourceNode ()->nodetype_get () == type && edge->sourceNode ()->no_get () == from && edge->destNode ()->nodetype_get () == type && edge->destNode ()->no_get () == to)
	return edge;
    }
  return 0;
}

void GraphWidget::edge_hide (NodeType type, int to)
{
  Edge*  edge = 0;

  foreach (QGraphicsItem *item, scene()->items())
    {
      if ((edge = qgraphicsitem_cast<Edge *>(item)) 
      		&& ((edge->destNode ()->nodetype_get () == type && edge->destNode ()->no_get () == to)
      			|| (edge->sourceNode ()->nodetype_get () == type && edge->sourceNode ()->no_get () == to)))
		edge->hide ();
    }
}

void GraphWidget::edge_show (NodeType type, int to)
{
  Edge*  edge = 0;

  foreach (QGraphicsItem *item, scene()->items())
    {
      if ((edge = qgraphicsitem_cast<Edge *>(item)) 
      		&& ((edge->destNode ()->nodetype_get () == type && edge->destNode ()->no_get () == to)
      			|| (edge->sourceNode ()->nodetype_get () == type && edge->sourceNode ()->no_get () == to)))
		edge->show ();
    }
}

Node* GraphWidget::add_node (NodeType type, int no, int size)
{
	Node* node = new Node (this);
	node->nodetype_set (type);
	node->no_set (no);
	node->nodesize_set (size);
	node->setPos (0, 0);
	scene_->addItem (node);
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

    // Shadow
    QRectF sceneRect = this->sceneRect();
//    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
//    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
//    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
//	painter->fillRect(rightShadow, Qt::darkGray);
//    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
//	painter->fillRect(bottomShadow, Qt::darkGray);

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

void GraphWidget::mouseMoveEvent (QMouseEvent* event)
{
  mouse_pos_ = event->pos ();
}

void GraphWidget::trigger_sig_node_clicked (NodeType type, int no) 
{
	if (type == COL) { 
		emit sig_node_clicked (no);
	} 
}
