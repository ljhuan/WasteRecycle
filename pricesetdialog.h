#ifndef PRICESETDIALOG_H
#define PRICESETDIALOG_H

#include <QDialog>
#include <QString>
#include "sqloper.h"

namespace Ui {
class PriceSetDialog;
}

class PriceSetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PriceSetDialog(QWidget *parent = 0, SqlOper* pOper = nullptr);
    ~PriceSetDialog();

    float getAPrice();
    float getBPrice();
    float getCPrice();
    float getDPrice();
    void updatePrices();

    QString threshold_;
    QString url_;
    QString interval_;
    QString differWeigh_;

private slots:
    void on_btn_priceRecommend_clicked();

    void on_btn_priceSet_clicked();

    void on_btn_ok_clicked();

    void on_btn_cancel_clicked();

private:
    Ui::PriceSetDialog *ui;
    SqlOper* oper;
    float storePrice_A_ = 0.0;
    float storePrice_B_ = 0.0;
    float storePrice_C_ = 0.0;
    float storePrice_D_ = 0.0;
};

#endif // PRICESETDIALOG_H
