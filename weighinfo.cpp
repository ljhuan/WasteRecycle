#include "weighinfo.h"
#include "ui_weighinfo.h"
#include <QImage>
#include <QDir>
#include <QDateTime>
#include <QString>
#include <QDebug>

WeighInfo::WeighInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WeighInfo)
{
    ui->setupUi(this);
    ui->btn_ok->setFocus();

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
    ui->le_todayWeight->setText(todayWeight_);

    QString headPhoto = QDir::currentPath() + "/members/" + name_ + ".jpeg";
    QImage image = QImage(headPhoto);
    int hw = ui->lb_head->width();
    int hh = ui->lb_head->height();
    qDebug() << ">>>>>>> hw:" << hw << " hh:" << hh;
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
}

void WeighInfo::clear()
{
    ui->le_name->clear();
    ui->le_num->clear();
    ui->le_rWeigh->clear();
    ui->le_vWeigh->clear();
    ui->le_todayWeight->clear();
    ui->lb_head->clear();
    ui->lb_rWeight->clear();
    ui->lb_vWeight->clear();
    vWeight_.clear();
    rWeight_.clear();
    name_.clear();
    num_.clear();
    todayWeight_.clear();
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
