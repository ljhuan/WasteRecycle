#include "pricesetdialog.h"
#include "ui_pricesetdialog.h"
#include <string>
#include <QString>
#include <QDebug>
#include <QDir>
#include "sqloper.h"

// 一年废纸总量，单位（年吨）
#define YEARLY_AMOUNTS   ((330)*(5))
// 一年的开支（ 1年场地租金 - 工人1年工资/年吨 - 水电费 - 设备折旧费 - 设备消耗费），单位元
#define YEARLY_EXPENSES  (34000+(4800+3900+4100)*12+400*4+1500*12+5000+400*3*12)
// #define KEEP2DECIPLACES(x) ((int)(x*100)/(100.0))

// 回收价格计算公式：
// 纸厂收购价*折扣*黄牛折扣 - 运费/吨 - 1年场地租金/年吨 - 工人一年工资/年吨 - 水电费/年吨 - 设备折旧费 - 设备消耗费


PriceSetDialog::PriceSetDialog(QWidget *parent, SqlOper* pOper) :
    QDialog(parent),
    ui(new Ui::PriceSetDialog),
    oper(pOper)
{
    ui->setupUi(this);
    updatePrices();
}

PriceSetDialog::~PriceSetDialog()
{
    delete ui;
}

float PriceSetDialog::getAPrice()
{
    return storePrice_A_;
}

float PriceSetDialog::getBPrice()
{
    return storePrice_B_;
}

float PriceSetDialog::getCPrice()
{
    return storePrice_C_;
}

float PriceSetDialog::getDPrice()
{
    return storePrice_D_;
}

void PriceSetDialog::updatePrices()
{
    PriceInfo prices;
    oper->sqlPriceQuery(prices);
    ui->le_factoryPrice_A->setText( prices.m_factoryPrice1);
    ui->le_factoryPrice_B->setText( prices.m_factoryPrice2);
    ui->le_factoryPrice_C->setText( prices.m_factoryPrice3);
    ui->le_storePrice_A->setText( prices.m_price1);
    ui->le_storePrice_B->setText( prices.m_price2);
    ui->le_storePrice_C->setText( prices.m_price3);
    ui->le_storePrice_D->setText( prices.m_price4);
    storePrice_A_ = prices.m_price1.toFloat();
    storePrice_B_ = prices.m_price2.toFloat();
    storePrice_C_ = prices.m_price3.toFloat();
    storePrice_D_ = prices.m_price4.toFloat();
}

void PriceSetDialog::on_btn_priceRecommend_clicked()
{
    qDebug() << "on_btn_priceRecommend_clicked IN";
    int iFactPrice_A = ui->le_factoryPrice_A->text().toInt();
    // int iFactPrice_B = ui->le_factoryPrice_B->text().toInt();
    int iFactPrice_C = ui->le_factoryPrice_C->text().toInt();

    // 好纸A： iFactPrice_A*0.95*0.97 - 50 - 34000(年)/310*5吨 - （4800+3900+4100）*12/310*5吨 - 400*4/310*5吨 - 1500*12/310*5吨-设备折旧费用- 400*3*12/310*5吨
    float fStorePrice_A = (iFactPrice_A*0.95*0.97 - 50 - YEARLY_EXPENSES/(YEARLY_AMOUNTS*(1.0))) / 2000;
    ui->le_storePrice_A->setText(QString::fromStdString(std::to_string(fStorePrice_A).substr(0, 4)));
    // 差纸C： iFactPrice_C*0.84*0.98
    float fStorePrice_C = (iFactPrice_C*0.84*0.98 - 50 - YEARLY_EXPENSES/(YEARLY_AMOUNTS*(1.0))) / 2000;
    ui->le_storePrice_C->setText(QString::fromStdString(std::to_string(fStorePrice_C).substr(0, 4)));
    // 超市纸B：好纸*百分比 + 差纸*百分比
    float fStorePrice_B = fStorePrice_A * 0.5 + fStorePrice_C * 0.5;
    ui->le_storePrice_B->setText(QString::fromStdString(std::to_string(fStorePrice_B).substr(0, 4)));
    float fStorePrice_D = fStorePrice_C - 0.05;
    // fStorePrice_D = KEEP2DECIPLACES(fStorePrice_D);
    ui->le_storePrice_D->setText(QString::fromStdString(std::to_string(fStorePrice_D).substr(0, 4)));
    qDebug() << "on_btn_priceRecommend_clicked OUT";
}

void PriceSetDialog::on_btn_priceSet_clicked()
{
    qDebug() << "on_btn_priceSet_clicked IN";
    PriceInfo prices;
    prices.m_factoryPrice1 = ui->le_factoryPrice_A->text();
    prices.m_factoryPrice2 = ui->le_factoryPrice_B->text();
    prices.m_factoryPrice3 = ui->le_factoryPrice_C->text();
    prices.m_price1 = ui->le_storePrice_A->text();
    prices.m_price2 = ui->le_storePrice_B->text();
    prices.m_price3 = ui->le_storePrice_C->text();
    prices.m_price4 = ui->le_storePrice_D->text();
    storePrice_A_ = prices.m_price1.toFloat();
    storePrice_B_ = prices.m_price2.toFloat();
    storePrice_C_ = prices.m_price3.toFloat();
    storePrice_D_ = prices.m_price4.toFloat();

    oper->sqlPriceSet(prices);

    this->close();
    this->done(0);
    qDebug() << "on_btn_priceSet_clicked OUT";
}
