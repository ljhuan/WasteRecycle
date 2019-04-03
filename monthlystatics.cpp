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
    QString qstrAverage = QString::fromLocal8Bit("近一个月平均价为：") + QString("%1").arg(average) + QString::fromLocal8Bit(" 元/吨");
    QString qstrPrice = QString::fromLocal8Bit("近一个月总开销为：") + QString("%1").arg(price) + QString::fromLocal8Bit(" 元");
    QString qstrWeight = QString::fromLocal8Bit("近一个月总量为：") + QString("%1").arg(weight/2000.0) + QString::fromLocal8Bit(" 吨");
    QString qstrDailyWeight = QString::fromLocal8Bit("近一个月平均每天回收量为：") + QString("%1").arg(weight/(2000.0*days)) + QString::fromLocal8Bit(" 吨");
    ui->lb_monthAverage->setText(qstrAverage);
    ui->lb_monthTotalPrice->setText(qstrPrice);
    ui->lb_monthTotalWeight->setText(qstrWeight);
    ui->lb_dailyWeight->setText(qstrDailyWeight);
}

monthlyStatics::~monthlyStatics()
{
    delete ui;
}
