#ifndef MAININTERFACE_H
#define MAININTERFACE_H

#include <qt4/QtGui/qwidget.h>
#include <memory>
#include <graph/graph.hpp>
#include "graphic_edge.h"
#include "graphic_vertex.h"
#include <QGraphicsScene>
#include <QTimer>
namespace Ui
{
class MainInterface;
}

class MainInterface : public QWidget
{
    Q_OBJECT

private:
    Ui::MainInterface* ui;
    
public:
    explicit MainInterface(QWidget* parent = 0);
    ~MainInterface();
    
public slots:
    //COMPONENT CREATION NB, se sono solo this, this, possono essere trasformati in private slots
    //called on button click, set up popop
    void Layer_1_press_event();
    void Layer_2_press_event();
    void Layer_3_press_event();
    void Layer_4_press_event();
    void Layer_5_press_event();
    //called on popup->accepted()
    void create_L1_obj();
    void create_L2_obj();
    void create_L3_obj();
    void create_L4_obj();
    void create_L5_obj();
    //UPDATE
    //called on double click, set up the popup 
    void start_update_L1_object(); 
    void start_update_L2_object();
    void start_update_L3_object();
    void start_update_L4_object();
    void start_update_L5_object();
    //called on popup->accepted()
    void finalize_update_L1_object();
    void finalize_update_L2_object();
    void finalize_update_L3_object();
    void finalize_update_L4_object();
    void finalize_update_L5_object();
    
    // called from click on the component. needed to draw line, and cancel components
    void component_clicked();
    // timer driven function to show where the line is being drawn
    void redraw_line();
    // undo the current line creation
    void break_line_drawing();
    // capture line item, capture right click on the scene (cancel line drawing)
    void mousePressEvent(QMouseEvent * e);
    // called on all the popup->rejected()
    void no_data();
    // create the arrow item
    void finalize_line();
    //check if an arrow or a component is selected and handles deletion (ie. for components deletes all the array related to him)
    void delete_items();
    
    //SEARCH FUNCTIONS
    //search data TO BE FILLED on creation. use enums.
    //void get_search_level();
    //void get_search_type();
    //textual. search the node and if exists accept, else refuse. they are not signals, has to be done in the start search
    //void get_starting_node();
    //void get_target_node();
    //launch the actual search.
    void start_search();
    
    //LOAD and STORE
    void load_file();
    void save_file();
    
private:
    //in order to eliminate both the item and the graphical item the scene has to be updated before the reset of the shared_ptr
    void delete_vertex(std::shared_ptr<Graphic_Vertex>& to_be_deleted);
    void delete_edge(std::shared_ptr<Graphic_Edge>& to_be_deleted);
    source_graph sg;
    std::map<std::shared_ptr<Graphic_Vertex>,vertex_t> vertices;
    //following map is passed to all edges, that will update it whenever a move action is performed. the map has to be unique and is initialized here in the main window.
    //even if ownership is shared edge removal should only be performed in main. 
    //the edges should only perform updates: ie one removal and one insert, since the key (aka the line item) is changed
    std::shared_ptr<std::map<QGraphicsLineItem*,std::shared_ptr<Graphic_Edge> > >arrows; //old style ptr because Qt uses those, not sure of using shared_ptr as value.
    QGraphicsScene* scene;
    QTimer* timer;
    std::shared_ptr<Graphic_Vertex> starting_object,arrival_object;
    std::shared_ptr<Graphic_Edge> selected_edge;
    //popup vari ed eventuali :S
    
    
    
};

#endif // MAININTERFACE_H
