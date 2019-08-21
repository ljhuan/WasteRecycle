#ifndef PRICE_H
#define PRICE_H

#include <QDialog>
#include <QLineEdit>
#include <map>
#include "sqloper.h"

namespace Ui {
class price;
}

class BaseLineEdit {
public:
    BaseLineEdit(QString name, float value, QLineEdit* le);
    ~BaseLineEdit();

    void showPrice();

    QString name_;
    float value_;
    QLineEdit* le_ = nullptr;
};

class price : public QDialog
{
    Q_OBJECT

public:
    explicit price(QWidget *parent = 0, SqlOper* pOper = nullptr);
    ~price();

    std::map<QString, BaseLineEdit*> LineEditMap_;
    std::map<QString, float> paperPrices_;

private slots:
    void lineEditClicked();

    void on_btn_ok_clicked();

    void on_btn_cancel_clicked();

private:
    Ui::price *ui;
    SqlOper* oper_ = nullptr;
    friend class BaseLineEdit;
};

#endif // PRICE_H
