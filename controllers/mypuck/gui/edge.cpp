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

#include <QPainter>
#include <cmath>
#include <iostream>
#include <assert.h>

#include "edge.h"
#include "node.h"

using namespace std;

const double Pi = 3.14159265358979323846264338327950288419717;
double TwoPi = 2.0 * Pi;

Edge::Edge(Node *sourceNode, Node *destNode)
    : arrowSize(10)
{
  //activ_ = true;
    setAcceptedMouseButtons(0);
    source = sourceNode;
    dest = destNode;
    source->addEdge_succ (this);
    dest->addEdge_pred (this);
    adjust();
    lightlevel_ = 50;
    arrow_actived_ = false;
}

Edge::~Edge()
{
}

Node *Edge::sourceNode() const
{
    return source;
}

void Edge::setSourceNode(Node *node)
{
    source = node;
    adjust();
}

Node *Edge::destNode() const
{
    return dest;
}

void Edge::setDestNode(Node *node)
{
    dest = node;
    adjust();
}

void Edge::adjust()
{
    if (!source || !dest)
        return;

    QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
    qreal length = line.length();
    QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);

    prepareGeometryChange();
    sourcePoint = line.p1() + edgeOffset;
    destPoint = line.p2() - edgeOffset;
}

QRectF Edge::boundingRect() const
{
    if (!source || !dest)
        return QRectF();

    qreal penWidth = 1;
    qreal extra = (penWidth + arrowSize) / 2.0;

    return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
                                      destPoint.y() - sourcePoint.y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!source || !dest)
        return;
    // Draw the line itself
    //QLineF line(QPointF (sourcePoint + QPointF(10, 10)), QPointF (destPoint + QPointF (10, 10)));
    QLineF line (sourcePoint, destPoint);
//    painter->setPen(QPen(QColor (col_).light (lightlevel_), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	painter->setPen(QPen(QColor (col_).light (0), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    assert (lightlevel_ >= 50);
//    if (dominant ())
      painter->drawLine(line);

    // Draw the arrows if there's enough room
    double angle = ::acos(line.dx() / line.length());
    if (line.dy() >= 0)
        angle = TwoPi - angle;

    QPointF sourceArrowP1 = sourcePoint + QPointF(sin(angle + Pi / 3) * arrowSize,
                                                  cos(angle + Pi / 3) * arrowSize);
    QPointF sourceArrowP2 = sourcePoint + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                                  cos(angle + Pi - Pi / 3) * arrowSize);   
    QPointF destArrowP1 = destPoint + QPointF(sin(angle - Pi / 3) * arrowSize,
                                              cos(angle - Pi / 3) * arrowSize);
    QPointF destArrowP2 = destPoint + QPointF(sin(angle - Pi + Pi / 3) * arrowSize,
                                              cos(angle - Pi + Pi / 3) * arrowSize);

    if (arrow_actived_)
      painter->setBrush(QColor (Qt::blue).light (250));
    else
      //painter->setBrush(QColor (col_).light (lightlevel_));
      painter->setBrush(QColor (col_).light (0));
    //painter->drawPolygon(QPolygonF() << line.p1() << sourceArrowP1 << sourceArrowP2);
    painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);        
}

bool Edge::dominant ()
{
  Edge*   back = dest->edge_get (*source);

  //return false;
  if (back && back->lightlevel_get () > lightlevel_)
    {
      //assert (false);
      return false;
    }
  return true;
}
