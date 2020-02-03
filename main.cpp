#include "tetrix.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Tetrix w;
    w.show();
    return a.exec();
}
