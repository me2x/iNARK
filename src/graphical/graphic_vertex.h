#ifndef GRAPHIC_VERTEX_H
#define GRAPHIC_VERTEX_H

#include <qt4/QtGui/qgraphicsitem.h>
#include <memory>
class Graphic_Vertex : public QGraphicsObject
{
    Q_OBJECT

public:
     enum { Graphic_Vertex_type = UserType + 1 };
    int type() const
    {// Enable the use of qgraphicsitem_cast with this item.
       return Graphic_Vertex_type;
    }
    Graphic_Vertex();
    void setLayer(int i);
    QRectF boundingRect() const;
    QRectF rect();
    void setArrowTarget(); //serve per deselezionare quando viene rilasciato dal click crea arrow. 
    void setRect(QRectF rect);
    QGraphicsTextItem * text;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    std::map<std::weak_ptr<Graphic_Vertex>,std::weak_ptr<Graphic_Edge> > starting_lines,arriving_lines; //weak ptrs?
    void redraw_lines();
    //stores the points where the arriving lines start (starting_points), and where the starting lines arrive (arriving_points)
signals:
    void riquadroCliccatoSx();
    void riquadroCliccatoDx();
    void riquadroMosso();
    void riquadroDoubleClick();
    
protected:
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void advance();
private:
    QBrush brush;
    QRectF rekt;
    bool being_moved, arrow_target;
};

#endif // GRAPHIC_VERTEX_H
