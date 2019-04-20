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
    QString itemAverage = days > 31 ? (QString("%1").arg(days) + QString::fromLocal8Bit("平均价为：")) : QString::fromLocal8Bit("近一个月平均价为：");
    QString qstrAverage = itemAverage + QString("%1").arg(average) + QString::fromLocal8Bit(" 元/吨");
    QString itemPrice = days > 31 ? (QString("%1").arg(days) + QString::fromLocal8Bit("总开销为：")) : QString::fromLocal8Bit("近一个月总开销为：");
    QString qstrPrice = itemPrice + QString("%1").arg(price) + QString::fromLocal8Bit(" 元");
    QString itemWeight = days > 31 ? (QString("%1").arg(days) + QString::fromLocal8Bit("总量为：")) : QString::fromLocal8Bit("近一个月总量为：");
    QString qstrWeight = itemWeight + QString("%1").arg(weight/2000.0) + QString::fromLocal8Bit(" 吨");
    QString itemDailyWeight = days > 31 ? (QString("%1").arg(days) + QString::fromLocal8Bit("平均每天回收量为：")) : QString::fromLocal8Bit("近一个月平均每天回收量为：");
    QString qstrDailyWeight = itemDailyWeight + QString("%1").arg(weight/(2000.0*days)) + QString::fromLocal8Bit(" 吨");
    ui->lb_monthAverage->setText(qstrAverage);
    ui->lb_monthTotalPrice->setText(qstrPrice);
    ui->lb_monthTotalWeight->setText(qstrWeight);
    ui->lb_dailyWeight->setText(qstrDailyWeight);
}

monthlyStatics::~monthlyStatics()
{
    delete ui;
}
