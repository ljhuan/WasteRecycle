#ifndef WEIGHINFO_H
#define WEIGHINFO_H

#include <QDialog>
#include <QString>

namespace Ui {
class WeighInfo;
}

class WeighInfo : public QDialog
{
    Q_OBJECT

public:
    explicit WeighInfo(QString num, QString name, QWidget *parent = 0);
    ~WeighInfo();

    QString name_;
    QString num_;
    QString rWeight_;
    QString vWeight_;
    QString finalPrice_;
    QString todayWeight_;

private slots:
    void on_btn_sure_clicked();

private:
    Ui::WeighInfo *ui;
};

#endif // WEIGHINFO_H
