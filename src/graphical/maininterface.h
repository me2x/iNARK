#ifndef MAININTERFACE_H
#define MAININTERFACE_H

#include <qt4/QtGui/qwidget.h>

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
};

#endif // MAININTERFACE_H
