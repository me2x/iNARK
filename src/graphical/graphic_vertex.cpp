#include "graphic_vertex.h"

void Graphic_Vertex::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
}
Graphic_Vertex::~Graphic_Vertex()
{
    delete text;
}

QRectF Graphic_Vertex::boundingRect() const
{
}

#include "graphic_vertex.moc"
