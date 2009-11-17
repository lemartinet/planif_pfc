#include "edge.h"
#include "node.h"
#include "math.hh"
#include <QPainter>
#include <cmath>

#define arrowSize 10

Edge::Edge(Node* sourceNode, Node* destNode):
		source(sourceNode), dest(destNode), rate_(0), best_(false)
{
}

Edge::~Edge()
{
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

    QLineF line2(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
    qreal length = line2.length();
    QPointF edgeOffset((line2.dx() * 10) / length, (line2.dy() * 10) / length);
    prepareGeometryChange();
    sourcePoint = line2.p1() + edgeOffset;
    destPoint = line2.p2() - edgeOffset;

	int r,g,b;
	color_palette (rate_, r, g, b);
	painter->setBrush(QColor (r,g,b));
	int size = best_?4:1;
	painter->setPen(QPen(QColor (r,g,b), size, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

	// Draw the line itself
	QLineF line (sourcePoint, destPoint);
	painter->drawLine(line);
	
	// Draw the arrows if there's enough room
	double angle = ::acos(line.dx() / line.length());
	if (line.dy() >= 0)
		angle = 2 * M_PI - angle;

	QPointF sourceArrowP1 = sourcePoint
		+ QPointF(sin(angle + M_PI / 3) * arrowSize, cos(angle + M_PI / 3) * arrowSize);
	QPointF sourceArrowP2 = sourcePoint
		+ QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize, cos(angle + M_PI - M_PI / 3) * arrowSize);
	QPointF destArrowP1 = destPoint
		+ QPointF(sin(angle - M_PI / 3) * arrowSize, cos(angle - M_PI / 3) * arrowSize);
	QPointF destArrowP2 = destPoint
		+ QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize, cos(angle - M_PI + M_PI / 3) * arrowSize);
	painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
}
