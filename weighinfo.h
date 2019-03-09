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
    explicit WeighInfo(QWidget *parent = 0);
    ~WeighInfo();

    void flush();
    void clear();

    QString name_;
    QString num_;
    QString rWeight_;
    QString vWeight_;
    QString todayWeight_;

signals:
    void flag(bool);

private slots:
    void on_btn_ok_clicked();

    void on_btn_cancel_clicked();

private:
    Ui::WeighInfo *ui;
};

#endif // WEIGHINFO_H
