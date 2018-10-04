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
    scale(0.8, 0.8);
    //setMinimumSize(400, 400);
    scene_ = scene;
    
    // repere pour le monde de Webots : min (-0.64, -1.5), max (1.13, 0.0))
	// repere pour le gui : min (0, 0), max (600, 600)
 	conv_ = new RepereConverter (-0.64, -1.70, 1.13,  0.0, 0, 0, 600, 600);
	scene_->setSceneRect(conv_->x2min_get (), conv_->y2min_get (), fabs (conv_->x2max_get () - conv_->x2min_get ()), fabs (conv_->y2max_get () - conv_->y2min_get ()));
}

GraphWidget::~GraphWidget ()
{
	// supprime automatiquement tous les items de la scene
	// (Edge, Node, ...) -> ne pas les supprimer ailleurs
	delete scene_;
	delete conv_;
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

Node* GraphWidget::add_node (NodeType type, int no, const Coord& coord_webots, int size, QColor col)
{
  //assert (coord_webots.x_get () != 0.0 && coord_webots.y_get () != 0.0);

  Node*   node = new Node (this);

  node->nodetype_set (type);
  node->no_set (no);
  node->color_set (col);
  node->nodesize_set (size);
  node->setPos ((int) conv_->convertx (coord_webots), (int)conv_->converty (coord_webots));
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
