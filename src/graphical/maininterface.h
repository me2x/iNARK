#ifndef MAININTERFACE_H
#define MAININTERFACE_H

#include <qt4/QtGui/qwidget.h>
#include <memory>
#include <graph/graph.hpp>
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
  /*  
public slots:
    //COMPONENT CREATION
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
    void get_search_level();
    void get_search_type();
    //textual. search the node and if exists accept, else refuse
    void get_starting_node();
    void get_target_node();
    //launch the actual search.
    void start_search();
    */
};

#endif // MAININTERFACE_H
