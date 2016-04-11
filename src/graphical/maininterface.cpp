#include "maininterface.h"
#include "ui_maininterface.h"
//#include <QString>
#include "graph/graph_common.hpp"
#include <QFileDialog>
//#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
MainInterface::MainInterface(QWidget* parent): QWidget(parent), ui(new Ui::MainInterface)
{
    using namespace commons;
    ui->setupUi(this);
    //Layer and Types scopes are set up from the enum in graph/graph_common
    for (int i = 0; i< SEARCH_LAYER_END;i++)
        ui->chose_layer->insertItem(i, QString::fromStdString(commons::get_search_layer_names(static_cast<Search_Layers>(i))));
    for (int i = 0; i< SEARCH_TYPE_END; i++)
        ui->chose_type->insertItem(i, QString::fromStdString(commons::get_search_type_name(static_cast<Search_Types>(i))));
    connect(ui->Load_Button,SIGNAL(clicked(bool)),this,SLOT(load_file()));
    connect(ui->Save_Button,SIGNAL(clicked(bool)),this,SLOT(save_file()));
}

MainInterface::~MainInterface()
{
delete ui;
}
void MainInterface::load_file()
{
    QString filename = QFileDialog::getOpenFileName(
      this,
      tr("Open source graph file"),
      QDir::homePath(),
      "All Files (*.*);;Graph source file (*.xml)"
    );
    if (!filename.isNull())
    {
        using boost::property_tree::ptree;
        ptree pt;
        read_xml(filename.toStdString(),pt);
        BOOST_FOREACH(ptree::value_type &v,pt.get_child("root.components"))
            {
                std::cout <<"xml letto" <<std::endl;
            }
    }
}

void MainInterface::save_file()
{
    QString filename = QFileDialog::getSaveFileName(
      this,
      tr("Save current source graph file"),
      QDir::homePath(),
      "All Files (*.*);;Graph source file (*.xml)"
    );
    if (!filename.isNull())
    {
        std::cout <<"save the file somehow" <<std::endl;
    }
}