#include "cpriceset.h"

CPriceSet::CPriceSet(QWidget *parent) : QWidget(parent) {
    setGeometry(0, 0, 640, 480);
    setWindowTitle("PriceSet");
}

void CPriceSet::view()
{
    show();
}
