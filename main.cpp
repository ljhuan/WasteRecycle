#include "wasterecycle.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WasteRecycle w;
    w.show();

    return a.exec();
}
