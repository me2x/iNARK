#ifndef GRAPHIC_EDGE_H
#define GRAPHIC_EDGE_H

#include <memory>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
class Graphic_Vertex;
class Graphic_Edge
{
    public:
        Graphic_Edge();
        void set_data(std::weak_ptr<Graphic_Vertex> from, std::weak_ptr<Graphic_Vertex> to, int fromport, int toport,std::shared_ptr<std::map<QGraphicsLineItem*,std::weak_ptr<Graphic_Edge>> >arrows); //get data. draw first.
        ~Graphic_Edge();
        void update(); //delete old qgraphicsitem, create new ones.

        //scene only uses old styled pointers. 
        //thats why old styled pointers are used and delete functions have to be used.
        //shared pointers would only introduce overhead
        QGraphicsLineItem *line,*sx_line,*dx_line;
        QGraphicsTextItem *label_start,*label_stop;
    private:
        //IMPORTANT: THIS HAVE TO BE MANUALLY HANDLED
        std::shared_ptr<std::map<QGraphicsLineItem*,std::weak_ptr<Graphic_Edge>> >arrows;
        int start_port,stop_port;
        std::weak_ptr<Graphic_Vertex> start_point;
        std::weak_ptr<Graphic_Vertex> arrival_point;
};

#endif // GRAPHIC_EDGE_H
