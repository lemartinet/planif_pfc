#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>

class QGraphicsSceneMouseEvent;
class Coord;

enum NodeType { CELL, COL, ROBOT };

class Node : public QObject, public QGraphicsItem
{
	Q_OBJECT

public:
	Node(NodeType nodetype, int no);
	~Node();

	int no_get() const { return no_; }
	void color_set(double val) { rate_ = val; }
	NodeType nodetype_get() const { return nodetype_; }
	void move(const Coord& coord_webots);

	enum { Type = UserType + 1 };
	int type() const { return Type; }

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
	void sig_node_clicked(int no);

private:
	NodeType nodetype_;
	int no_;
	int nodesize_;
	double rate_;
};

#endif
