#ifndef PROVA_H
#define PROVA_H

#include <qt4/QtGui/qwidget.h>

namespace Ui
{
class prova;
}

class prova : public QWidget
{
    Q_OBJECT

private:
    Ui::prova* ui;
};

#endif // PROVA_H
