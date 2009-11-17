#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsItem>

class Node;

class Edge : public QGraphicsItem
{
public:
    Edge(Node* sourceNode, Node* destNode);
    ~Edge();

    Node *sourceNode() const { return source; }
    Node *destNode() const { return dest; }
	void color_set(double val) { rate_ = val; }
	void best_set(bool best) { best_ = best; }

	enum { Type = UserType + 2 };
	int type() const { return Type; }

protected:
	QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
private:
    Node *source, *dest;
    QPointF sourcePoint;
    QPointF destPoint;
    double rate_;
    bool best_;
};

#endif
