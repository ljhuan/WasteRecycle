#ifndef WEIGHINFO_H
#define WEIGHINFO_H

#include <QDialog>
#include <QString>
#include "sqloper.h"

namespace Ui {
class WeighInfo;
}

class WeighInfo : public QDialog
{
    Q_OBJECT

public:
    explicit WeighInfo(QWidget *parent = 0, SqlOper* pOper = nullptr);
    ~WeighInfo();

    void flush();
    void clear();

    QString name_;
    QString num_;
    QString rWeight_;
    QString vWeight_;
    QString yestodayWeight_;
    SqlOper* oper_ = nullptr;

signals:
    void flag(bool);

public slots:

    void on_btn_ok_clicked();

private slots:

    void on_btn_cancel_clicked();

    void on_btn_prize_clicked();

private:
    Ui::WeighInfo *ui;
};

#endif // WEIGHINFO_H
