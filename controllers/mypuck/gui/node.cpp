#include "node.h"
#include "math.hh"
#include "coord.hh"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <iostream>
#include <cstdio>

Node::Node(NodeType nodetype, int no) : nodetype_(nodetype), no_(no), rate_(0)
{
	if (nodetype_ == COL)
		nodesize_ = 20;
	else if (nodetype_ == CELL)
		nodesize_ = 15;
	else
		nodesize_ = 20;
}

Node::~Node()
{
}

void Node::move(const Coord& coord_webots)
{
	setPos ((int)Coord::convertx (coord_webots), (int)Coord::converty (coord_webots));
}

QRectF Node::boundingRect() const
{
    qreal marge = 10;
    return QRectF(-10 - marge, -10 - marge, 23 + marge, 23 + marge);
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
	    painter->drawText(-15, -11, QString(s));
	    painter->setPen(QPen(Qt::white, 0));
	    sprintf(s, "%d", no_ + 1);
	    painter->drawText(-10, 4, QString(s));
    }
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mousePressEvent(event);
	if (nodetype_ == COL) {
		emit sig_node_clicked (no_);
	}
}
