#include "weighinfo.h"
#include "ui_weighinfo.h"
#include <QImage>
#include <QDir>

WeighInfo::WeighInfo(QString num, QString name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WeighInfo)
{
    ui->setupUi(this);

    QString headPhoto = QDir::currentPath() + "/members/" + name + ".jpeg";
    QImage image = QImage(headPhoto);
    ui->lb_head->setPixmap(QPixmap::fromImage(image).scaled(ui->lb_head->width(), ui->lb_head->height()));
}

WeighInfo::~WeighInfo()
{
    delete ui;
}

void WeighInfo::on_btn_sure_clicked()
{
    this->close();
    this->done(0);
}
