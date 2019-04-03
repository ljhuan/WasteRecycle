#include "price.h"
#include "ui_price.h"
#include "mylineedit.h"
#include <iostream>
#include <QDebug>
#include <QMessageBox>

price::price(QWidget *parent, SqlOper* pOper) :
    QDialog(parent),
    ui(new Ui::price),
    oper_(pOper)
{
    ui->setupUi(this);
    ui->btn_ok->setFocus();

    LineEditMap_[QString::fromLocal8Bit("黄板")] = new BaseLineEdit(QString::fromLocal8Bit("黄板"), (float)0.0, ui->le_1);
    LineEditMap_[QString::fromLocal8Bit("超1")] = new BaseLineEdit(QString::fromLocal8Bit("超1"), (float)0.0, ui->le_2);
    LineEditMap_[QString::fromLocal8Bit("书本")] = new BaseLineEdit(QString::fromLocal8Bit("书本"), (float)0.0, ui->le_3);
    LineEditMap_[QString::fromLocal8Bit("报纸")] = new BaseLineEdit(QString::fromLocal8Bit("报纸"), (float)0.0, ui->le_4);
    LineEditMap_[QString::fromLocal8Bit("统货")] = new BaseLineEdit(QString::fromLocal8Bit("统货"), (float)0.0, ui->le_5);
    LineEditMap_[QString::fromLocal8Bit("超2")] = new BaseLineEdit(QString::fromLocal8Bit("超2"), (float)0.0, ui->le_6);
    LineEditMap_[QString::fromLocal8Bit("铁")] = new BaseLineEdit(QString::fromLocal8Bit("铁"), (float)0.0, ui->le_7);
    LineEditMap_[QString::fromLocal8Bit("不锈钢")] = new BaseLineEdit(QString::fromLocal8Bit("不锈钢"), (float)0.0, ui->le_8);
    LineEditMap_[QString::fromLocal8Bit("民用")] = new BaseLineEdit(QString::fromLocal8Bit("民用"), (float)0.0, ui->le_9);
    LineEditMap_[QString::fromLocal8Bit("超3")] = new BaseLineEdit(QString::fromLocal8Bit("超3"), (float)0.0, ui->le_10);
    LineEditMap_[QString::fromLocal8Bit("铝")] = new BaseLineEdit(QString::fromLocal8Bit("铝"), (float)0.0, ui->le_11);
    LineEditMap_[QString::fromLocal8Bit("铜")] = new BaseLineEdit(QString::fromLocal8Bit("铜"), (float)0.0, ui->le_12);

    std::list<QString> elements;
    QStringList tables = pOper->sqlGetAllTableName();
    if (!tables.contains("prices")) {
        QString createChartsTable = "create table prices (kind varchar(100), value varchar(10));";
        pOper->createTable(createChartsTable);
    } else {
        // 先从数据库price表中读取每一类的价格，再写入内存map中
        QString queryPricesTable = "select * from prices;";
        elements = pOper->queryTablePrices(queryPricesTable);

        for (auto e : elements) {
            QStringList line = e.split("  ");
            QString kind =  line.at(0);
            QString value = line.at(1);
            LineEditMap_[kind]->value_ = value.toFloat();
            LineEditMap_[kind]->showPrice();
        }
    }

    connect(ui->le_1, SIGNAL(clicked()), this, SLOT(lineEditClicked()));
    connect(ui->le_2, SIGNAL(clicked()), this, SLOT(lineEditClicked()));
    connect(ui->le_3, SIGNAL(clicked()), this, SLOT(lineEditClicked()));
    connect(ui->le_4, SIGNAL(clicked()), this, SLOT(lineEditClicked()));
    connect(ui->le_5, SIGNAL(clicked()), this, SLOT(lineEditClicked()));
    connect(ui->le_6, SIGNAL(clicked()), this, SLOT(lineEditClicked()));
    connect(ui->le_7, SIGNAL(clicked()), this, SLOT(lineEditClicked()));
    connect(ui->le_8, SIGNAL(clicked()), this, SLOT(lineEditClicked()));
    connect(ui->le_9, SIGNAL(clicked()), this, SLOT(lineEditClicked()));
    connect(ui->le_10, SIGNAL(clicked()), this, SLOT(lineEditClicked()));
    connect(ui->le_11, SIGNAL(clicked()), this, SLOT(lineEditClicked()));
    connect(ui->le_12, SIGNAL(clicked()), this, SLOT(lineEditClicked()));

}

price::~price()
{
    delete ui;
    for (auto e : LineEditMap_) {
        delete (e.second);
    }
}

void price::lineEditClicked()
{
    MyLineEdit *myle=qobject_cast<MyLineEdit*>(sender());
    myle->clear();
}

void price::on_btn_ok_clicked()
{
    // 遍历所有le 当le中内容不是种类名称以及跟value值不同时，存入数据库
    for (auto e : LineEditMap_) {
        BaseLineEdit* second = e.second;
        if (second->name_ != second->le_->text() && second->value_ != second->le_->text().toFloat()) {
            // 存入数据库
            QString insertSql =  "insert into prices values('" + second->name_ + "'" + ", '" + second->le_->text() + "');";
            oper_->insertTable(insertSql);
            qDebug() << second->name_ << " " << second->value_ << "->" << second->le_->text();
            second->value_ = second->le_->text().toFloat();
        }
    }

    this->close();
    // QMessageBox::warning(this, QString::fromLocal8Bit("提醒"), QString::fromLocal8Bit("价格未修改！"), QString::fromLocal8Bit("关闭"));
}


void price::on_btn_cancel_clicked()
{
    for (auto e : LineEditMap_) {
        BaseLineEdit* second = e.second;
        QString msg;
        if (second->value_ != 0) {
            msg = QString("%1").arg(second->value_);
        } else {
            msg = second->name_;
        }
        second->le_->setText(msg);
    }
}

BaseLineEdit::BaseLineEdit(QString name, float value, QLineEdit* le):
    name_(name),
    value_(value),
    le_(le)
{
    le_->setText(name_);
}

BaseLineEdit::~BaseLineEdit()
{

}

void BaseLineEdit::showPrice()
{
    le_->setText(QString("%1").arg(value_));
}

