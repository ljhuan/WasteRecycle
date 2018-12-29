#ifndef MONTHLYSTATICS_H
#define MONTHLYSTATICS_H

#include <QDialog>

namespace Ui {
class monthlyStatics;
}

class monthlyStatics : public QDialog
{
    Q_OBJECT

public:
    explicit monthlyStatics(QWidget *parent = 0);
    monthlyStatics(float weight, float average, float price, int days, QWidget *parent = 0);
    ~monthlyStatics();

private:
    Ui::monthlyStatics *ui;
};

#endif // MONTHLYSTATICS_H
