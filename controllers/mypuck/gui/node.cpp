/****************************************************************************
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

#include "node.h"
#include "edge.h"
#include "graphwidget.h"
#include "mystr.hh"
#include "math.hh"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <iostream>
#include <sstream>
#include <algorithm>

Node::Node (GraphWidget *graphWidget)
  : graph(graphWidget), nodesize_(20), rate_(0)
{
    setFlag(ItemIsMovable);
    setZValue(1);
}

Node::~Node ()
{
}

void Node::addEdge_succ (Edge *edge)
{
    edgeList_succ << edge;
    edge->adjust();
}

void Node::addEdge_pred (Edge *edge)
{
    edgeList_pred << edge;
    edge->adjust();
}

void Node::delEdge_succ (Edge *edge)
{
  QList<Edge *>::iterator it;

  it = find (edgeList_succ.begin (), edgeList_succ.end (), edge);
  edgeList_succ.erase (it);
}

void Node::delEdge_pred (Edge *edge)
{
  QList<Edge *>::iterator it;

  it = find (edgeList_pred.begin (), edgeList_pred.end (), edge);
  edgeList_pred.erase (it);
}

void Node::move (const Coord& coord_webots)
{
	setPos ((int)Coord::convertx (coord_webots), (int)Coord::converty (coord_webots));
	update_pos ();
}

QList<Edge *> Node::edges_succ () const
{
    return edgeList_succ;
}

QList<Edge *> Node::edges_pred () const
{
    return edgeList_pred;
}

void Node::calculateForces()
{
  newPos = pos ();
}

bool Node::advance()
{
    if (newPos == pos())
        return false;

    setPos(newPos);
    return true;
}

QRectF Node::boundingRect() const
{
    qreal adjust = 10;
    return QRectF(-10 - adjust, -10 - adjust,
                  23 + adjust, 23 + adjust);
}

QPainterPath Node::shape() const
{
    QPainterPath path;
    path.addEllipse(-10, -10, 20, 20);
    return path;
}

void Node::color_set (double val) 
{ 
	rate_ = val; 
	update (); 
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setPen(QPen(Qt::black, 0));
	if (nodetype_ != ROBOT) {
		int r,g,b;
		color_palette (rate_, r, g, b);
		painter->setBrush(QColor (r,g,b));
	}
	else {
		painter->setBrush(QColor (255,255,255));
	}
	painter->drawEllipse(-10, -10, nodesize_, nodesize_);
    
    if (nodetype_ != ROBOT) {
	    char s[128];
	    sprintf(s, "%.2f", rate_);
	    QString qs1;
	    qs1.append(s);
	    painter->drawText(-15, -11, qs1);
	    painter->setPen(QPen(Qt::white, 0));
	    sprintf(s, "%d", no_);
	    QString qs2;
	    qs2.append(s);
	    painter->drawText(-10, 4, qs2);
    }
}

void Node::update_pos ()
{
  foreach (Edge *edge, edgeList_succ)
    edge->adjust ();
  foreach (Edge *edge, edgeList_pred)
    edge->adjust ();
  graph->itemMoved();
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionChange:
      update_pos ();
      break;
    default:
      break;
    };
    
    return QGraphicsItem::itemChange(change, value);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  update();
  QGraphicsItem::mousePressEvent(event);
  graph->trigger_sig_node_clicked (nodetype_, no_);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  update();
  QGraphicsItem::mouseReleaseEvent(event);
}

Edge* Node::edge_get (Node& node)
{
  foreach (Edge *edge, edgeList_succ)
    {
      if (edge->destNode () == &node)
        return edge;
    }
  return 0;
}
