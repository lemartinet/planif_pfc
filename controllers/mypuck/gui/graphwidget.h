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

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QtGui/QGraphicsView>
#include <QGraphicsScene>
#include "node.h"
#include "edge.h"
#include "coord.hh"

using namespace std;

class GraphWidget : public QGraphicsView
{
    Q_OBJECT

public:
    GraphWidget ();
    ~GraphWidget ();

    Node* node_get (NodeType type, int no);
    Edge* edge_get (NodeType type, int from, int to);
    Node* add_node (NodeType type, int no, int size);
    Edge* add_edge (NodeType type, int from, int to);
    QGraphicsScene* scene_get () { return scene_; }
    QPoint& mouse_pos_get () { return mouse_pos_; }
    void type_del (NodeType type);
    void edge_type_del (NodeType type);
    void edge_hide (NodeType type, int to);
    void edge_show (NodeType type, int to);
    
	void trigger_sig_node_clicked (NodeType type, int no);
	void itemMoved();

protected:
    void keyPressEvent(QKeyEvent *event);
    void timerEvent(QTimerEvent *event);
    void wheelEvent(QWheelEvent *event);
    void closeEvent(QCloseEvent *);
    void mouseMoveEvent (QMouseEvent* event);
    void drawBackground(QPainter *painter, const QRectF &rect);
    void scaleView(qreal scaleFactor);

private:
    QGraphicsScene*   scene_;
    QPoint            mouse_pos_;

signals:
    void sig_node_clicked (int no);
};

#endif
