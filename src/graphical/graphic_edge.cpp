#include "graphic_edge.h"
#include <boost/math/constants/constants.hpp>
#include <QPen>
Graphic_Edge::set_data(std::weak_ptr<Graphic_Vertex> from, std::weak_ptr<Graphic_Vertex> to, int fromport, int toport,std::shared_ptr<std::map<QGraphicsLineItem*,std::weak_ptr<Graphic_Edge>> >arrows)
{
    this->arrows=arrows;
    arrival_point = to;
    start_point = from;
    start_port = fromport;
    stop_port = toport;
    QPen blackpen = QPen(Qt::black);
    QPointF* starting_point = new QPointF(from->x()+from->boundingRect().center().x(),from->y()+from->boundingRect().bottom());
    QPointF* arrival_point2 = new QPointF (to->x()+to->boundingRect().center().x(),to->y()+to->boundingRect().top());
    QLineF newline(*arrival_point2,*starting_point);
    line = (to->scene()->addLine(newline,blackpen));
    from->arriving_lines.insert(std::make_pair(to,this));
    std::pair<ProvaRiquadro*,std::weak_ptr<Graphic_Edge>> tmppair = std::make_pair(from, this);
    to->starting_lines.insert(tmppair);
    //std::cout<<"crash test: startinglines arrow addreess "<<tmppair.second<<std::endl;
    double angle = ::acos(newline.dx() / newline.length());
    if (newline.dy() >= 0)
        angle = (boost::math::constants::pi<double>() * 2) - angle;
    QPointF arrowP1 = newline.p1() + QPointF(sin(angle + boost::math::constants::pi<double>() / 3) * 10, cos(angle + boost::math::constants::pi<double>() / 3) * 10);
    QPointF arrowP2 = newline.p1() + QPointF(sin(angle + boost::math::constants::pi<double>() - boost::math::constants::pi<double>() / 3) * 10,cos(angle + boost::math::constants::pi<double>() - boost::math::constants::pi<double>() / 3) * 10);
    QLineF tmp(*arrival_point2,arrowP1);
    
    dx_line = (to->scene()->addLine(tmp,blackpen));
    QLineF tmp2(*arrival_point2,arrowP2);
    std::cout<<"crash test: draw line"<<std::endl;
    sx_line = (to->scene()->addLine(tmp2,blackpen));
    
    line->setFlag(QGraphicsItem::ItemIsSelectable);
    label_start = new QGraphicsTextItem();
    label_start->setPos(starting_point->x()-(newline.dx()/2), starting_point->y()-newline.dy()+(2*newline.dy()/3)-5); //stessa y ottenuta cosi. 
    label_start->setPlainText(fromport!=0? QString::number(fromport): "");
    to->scene()->addItem(label_start);
    label_stop = new QGraphicsTextItem();
    label_stop->setPos(arrival_point2->x()+(newline.dx()/2), arrival_point2->y()+(newline.dy()/3)-5);
    label_stop->setPlainText(toport!=0? QString::number(toport): "");
    to->scene()->addItem(label_stop);
    std::cout<< "arrow to be inserted"<<std::endl;
    std::cout << "sizw is: "<<arrows->size()<<std::endl;
    arrows->insert(std::make_pair(line,this)); 
    std::cout<< "arrow created"<<std::endl;
}

void Graphic_Edge::update()
{
    std::cout<<"arrow update start_point"<<std::endl;
    std::cout<<"arrow update arrival point is: "<<arrival_point<<std::endl;
    QGraphicsScene* scene = arrival_point->scene();
    std::cout<<"arrow update scenee assigned"<<std::endl;
    scene->removeItem(line); 
    scene->removeItem(sx_line);
    scene->removeItem(dx_line);
    arrows->erase(arrows->find(line));
    delete line;
    delete sx_line;
    delete dx_line;
    std::cout<<"arrow update item removed"<<std::endl;
    QPen blackpen = QPen(Qt::black);
    QPointF* starting_point = new QPointF(start_point->x()+start_point->boundingRect().center().x(),start_point->y()+start_point->boundingRect().bottom());
    QPointF* arrival_point_point = new QPointF (arrival_point->x()+arrival_point->boundingRect().center().x(),arrival_point->y()+arrival_point->boundingRect().top());
    QLineF newline(*arrival_point_point,*starting_point);
    line= scene->addLine(newline,blackpen);
    double angle = ::acos(newline.dx() / newline.length());
    if (newline.dy() >= 0)
        angle = (boost::math::constants::pi<double>() * 2) - angle;
    QPointF arrowP1 = newline.p1() + QPointF(sin(angle + boost::math::constants::pi<double>() / 3) * 10, cos(angle + boost::math::constants::pi<double>() / 3) * 10);
    QPointF arrowP2 = newline.p1() + QPointF(sin(angle + boost::math::constants::pi<double>() - boost::math::constants::pi<double>() / 3) * 10,cos(angle + boost::math::constants::pi<double>() - boost::math::constants::pi<double>() / 3) * 10);
    QLineF tmp(*arrival_point_point,arrowP1);
    std::cout<<"crash test: draw line"<<std::endl;
    dx_line = scene->addLine(tmp,blackpen);
    QLineF tmp2(*arrival_point_point,arrowP2);
    std::cout<<"crash test: draw line"<<std::endl;
    sx_line = scene->addLine(tmp2,blackpen);
    arrows->insert(std::make_pair(line, this));
    label_start->setPos(starting_point->x()-newline.dx()+(2*newline.dx()/3), starting_point->y()-newline.dy()+(2*newline.dy()/3)-5); //stessa y ottenuta cosi. 
    label_stop->setPos(arrival_point_point->x()+(newline.dx()/3), arrival_point_point->y()+(newline.dy()/3)-5);
}
