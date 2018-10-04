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

#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>
#include <QList>
#include "coord.hh"

class Edge;
class GraphWidget;
class QGraphicsSceneMouseEvent;

enum e_NodeType { CELL, COL, ROBOT };
typedef enum e_NodeType NodeType;

class Node : public QGraphicsItem
{
public:
    Node(GraphWidget *graphWidget);
    ~Node();
    
    inline
      void no_set (int no) { no_ = no; }

    inline
      int no_get () { return no_; }
    
    inline
      void activ_set (bool activ) { activ_ = activ; update (); }
    
    inline
      void lightlevel_set (double val) { rate_ = val; lightlevel_ = (int)(val * 200.0 + 50); update (); }

    inline
      void color_set (QColor col) { col_ = col; }

    inline
      NodeType nodetype_get () { return nodetype_; }

    inline
      void nodetype_set (NodeType type) { nodetype_ = type; }

    inline
      void nodesize_set (int size) { nodesize_ = size; }

    void addEdge_succ (Edge *edge);
    void addEdge_pred (Edge *edge);

    void delEdge_succ (Edge *edge);
    void delEdge_pred (Edge *edge);

    void move (const Coord& coord_webots);

    QList<Edge *> edges_succ () const;
    QList<Edge *> edges_pred () const;

    enum { Type = UserType + 1 };
    int type() const { return Type; }

    void calculateForces();
    bool advance();

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void update_pos ();

    Edge* edge_get (Node& node);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    
private:
    QList<Edge *> edgeList_pred;
    QList<Edge *> edgeList_succ;
    QPointF       newPos;
    GraphWidget*  graph;
    int           no_;
    bool          activ_;
    int           lightlevel_;
    double rate_;
    QColor        col_;
    int           nodesize_;
    NodeType      nodetype_;
};

#endif
