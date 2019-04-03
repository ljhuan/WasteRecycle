#include "weighinfo.h"
#include "ui_weighinfo.h"
#include <QImage>
#include <QDir>
#include <QDateTime>
#include <QString>
#include <QDebug>

WeighInfo::WeighInfo(QWidget *parent, SqlOper* pOper) :
    QDialog(parent),
    ui(new Ui::WeighInfo),
    oper_(pOper)
{
    ui->setupUi(this);
    ui->btn_ok->setFocus();
    ui->btn_prize->hide();
    ui->lb_prize->hide();

    // 创建今天的prize表
    QDateTime date = QDateTime::currentDateTime();

//    QDate yestoday = date.date().addDays(-1);
//    QString yestodayTableName = "prize_" + yestoday.toString("yyyy_MM_dd");

//    QStringList tables = oper_->sqlGetAllTableName();
//    if (tables.contains(yestodayTableName)) {
//        QString sql = "drop table " + yestodayTableName;
//        oper_->dropTable(sql);
//    }

    QStringList tables = oper_->sqlGetAllTableName();
    if(!tables.contains("prize")) {
        QString sql = "create table prize (time varchar(100), name varchar(20));";
        oper_->createTable(sql);
    } else {
        QString sql = "delete from prize where time < '" + date.date().toString("yyyy-MM-dd") + "' ;";
        oper_->deleteFromTable(sql);
    }

    this->show();
    clear();
    this->close();
}

WeighInfo::~WeighInfo()
{
    delete ui;
}

void WeighInfo::flush()
{
    ui->le_name->setText(name_);
    ui->le_num->setText(num_);
    ui->le_rWeigh->setText(rWeight_);
    ui->le_vWeigh->setText(vWeight_);
    ui->le_yestodayWeight->setText(yestodayWeight_);

    QString headPhoto = QDir::currentPath() + "/members/" + name_ + ".jpeg";
    QImage image = QImage(headPhoto);
    int hw = ui->lb_head->width();
    int hh = ui->lb_head->height();
    ui->lb_head->setPixmap(QPixmap::fromImage(image).scaled(hw, hh));

    QDateTime date = QDateTime::currentDateTime();
    QString today = date.toString("yyyy_MM_dd");

    QString rWeighPic = QDir::currentPath() +  "/" + today + "_rwPicture" + "/" + num_ + "_" + rWeight_ + ".jpeg";
    QFile rwFile(rWeighPic);
    if (rwFile.exists()) {
        QImage rImage = QImage(rWeighPic);
        if(!rImage.size().isEmpty()) {
            ui->lb_rWeight->setPixmap(QPixmap::fromImage(rImage).scaled(ui->lb_rWeight->width(), ui->lb_rWeight->height()));
        }
    }

    QString vWeighPic = QDir::currentPath() +  "/" + today + "_vwPicture" + "/" + num_ + "_" + vWeight_ + ".jpeg";
    QFile vwFile(vWeighPic);
    if (vwFile.exists()) {
        QImage vImage = QImage(vWeighPic);
        if(!vImage.size().isEmpty()) {
            ui->lb_vWeight->setPixmap(QPixmap::fromImage(vImage).scaled(ui->lb_vWeight->width(), ui->lb_vWeight->height()));
        }
    }

    // 查询昨天商贩卖货总价并计算奖金
    std::list<QString> elements;
    QDate yestoday = date.date().addDays(-1);
    QString tableName = "record_" + yestoday.toString("yyyy_MM_dd");
    QString queryRecordTable = "select * from " + tableName + " where name='" + name_ + "' order by time asc;";
    elements = oper_->queryRecords(queryRecordTable);

    float totalPrice = 0;
    QString info;
    for (auto element : elements) {
        qDebug() << "element:" << element;
        info += element + "\n";
        QStringList line = element.split("  ");
        totalPrice += line.at(5).toFloat();
    }
    ui->le_yestodayWeight->setText(QString("%1").arg(totalPrice) + QString::fromLocal8Bit("元"));
    ui->tb_records->setText(info);

    bool exist = oper_->searchTableWetherExist("prize", "name", name_);

    // 回磅时，本次卖货量大于100斤，并且尚未领奖，则会触发奖励机制
    if (vWeight_ != "" && (rWeight_.toFloat()-vWeight_.toFloat()) >= 50 && !exist) {
        QString msg = QString::fromLocal8Bit("奖励：") + QString("%1").arg((int)(totalPrice)/100  * 2) + QString::fromLocal8Bit("元");
        ui->lb_prize->setText(msg);
        ui->lb_prize->setVisible(true);
        ui->btn_prize->setVisible(true);
    }
}

void WeighInfo::clear()
{
    ui->le_name->clear();
    ui->le_num->clear();
    ui->le_rWeigh->clear();
    ui->le_vWeigh->clear();
    ui->le_yestodayWeight->clear();
    ui->lb_head->clear();
    ui->lb_rWeight->clear();
    ui->lb_vWeight->clear();
    vWeight_.clear();
    rWeight_.clear();
    name_.clear();
    num_.clear();
    yestodayWeight_.clear();
    ui->tb_records->document()->clear();
}

void WeighInfo::on_btn_ok_clicked()
{
    this->clear();
    this->close();
    // this->done(0);
    emit flag(true);
}

void WeighInfo::on_btn_cancel_clicked()
{
    this->clear();
    this->close();
    emit flag(false);
}

void WeighInfo::on_btn_prize_clicked()
{
    ui->lb_prize->setHidden(true);
    ui->btn_prize->setHidden(true);

    QDateTime date = QDateTime::currentDateTime();
    QString today = date.toString("yyyy-MM-dd");
    QString insert = "insert into prize values('" + today + "' ,'" + name_ + "');";
    oper_->insertTable(insert);
}
