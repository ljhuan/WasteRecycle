#include "monthlystatics.h"
#include "ui_monthlystatics.h"

monthlyStatics::monthlyStatics(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::monthlyStatics)
{
    ui->setupUi(this);
}

monthlyStatics::monthlyStatics(float weight, float average, float price, int days, QWidget *parent):
    QDialog(parent),
    ui(new Ui::monthlyStatics)
{
    ui->setupUi(this);
    QString qstrAverage = "近一个月平均价为：" + QString("%1").arg(average) + " 元/吨";
    QString qstrPrice = "近一个月总开销为：" + QString("%1").arg(price) + " 元";
    QString qstrWeight = "近一个月总量为：" + QString("%1").arg(weight/2000.0) + " 吨";
    QString qstrDailyWeight = "近一个月平均每天回收量为：" + QString("%1").arg(weight/(2000.0*days)) + " 吨";
    ui->lb_monthAverage->setText(qstrAverage);
    ui->lb_monthTotalPrice->setText(qstrPrice);
    ui->lb_monthTotalWeight->setText(qstrWeight);
    ui->lb_dailyWeight->setText(qstrDailyWeight);
}

monthlyStatics::~monthlyStatics()
{
    delete ui;
}
