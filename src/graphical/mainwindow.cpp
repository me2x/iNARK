#include "mainwindow.h"
#include "maininterface.h"

MainWindow::MainWindow()
{
    MainInterface* interface = new MainInterface();
    setCentralWidget(interface);
}

MainWindow::~MainWindow(){}

