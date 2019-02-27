#include "weighinfo.h"
#include "ui_weighinfo.h"
#include <QImage>
#include <QDir>
#include <QDateTime>
#include <QString>

WeighInfo::WeighInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WeighInfo)
{
    ui->setupUi(this);
    ui->btn_ok->setFocus();
//    QString headPhoto = QDir::currentPath() + "/members/" + name + ".jpeg";
//    QImage image = QImage(headPhoto);
//    ui->lb_head->setPixmap(QPixmap::fromImage(image).scaled(ui->lb_head->width(), ui->lb_head->height()));
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
    ui->le_todayWeight->setText(todayWeight_);

    QString headPhoto = QDir::currentPath() + "/members/" + name_ + ".jpeg";
    QImage image = QImage(headPhoto);
    ui->lb_head->setPixmap(QPixmap::fromImage(image).scaled(ui->lb_head->width(), ui->lb_head->height()));

    QDateTime date = QDateTime::currentDateTime();
    QString today = date.toString("yyyy_MM_dd");

    QString rWeighPic = QDir::currentPath() +  "/" + today + "_rwPicture" + "/" + num_ + "_" + rWeight_ + ".jpeg";
    QImage rImage = QImage(rWeighPic);
    if(!rImage.size().isEmpty()) {
        ui->lb_rWeight->setPixmap(QPixmap::fromImage(rImage).scaled(ui->lb_rWeight->width(), ui->lb_rWeight->height()));
    }

    QString vWeighPic = QDir::currentPath() +  "/" + today + "_vwPicture" + "/" + num_ + "_" + vWeight_ + ".jpeg";
    QImage vImage = QImage(vWeighPic);
    if(!vImage.size().isEmpty()) {
        ui->lb_vWeight->setPixmap(QPixmap::fromImage(vImage).scaled(ui->lb_vWeight->width(), ui->lb_vWeight->height()));
    }
}

void WeighInfo::clear()
{
    ui->le_name->setText("");
    ui->le_num->setText("");
    ui->le_rWeigh->setText("");
    ui->le_vWeigh->setText("");
    ui->le_todayWeight->setText("");
}

void WeighInfo::on_btn_ok_clicked()
{
    this->close();
    // this->done(0);
    emit flag(true);
}

void WeighInfo::on_btn_cancel_clicked()
{
    this->close();
    emit flag(false);
}
