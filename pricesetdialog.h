#ifndef PRICESETDIALOG_H
#define PRICESETDIALOG_H

#include <QDialog>
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

private slots:
    void on_btn_priceRecommend_clicked();

    void on_btn_priceSet_clicked();

private:
    Ui::PriceSetDialog *ui;
    SqlOper* oper;
    float storePrice_A_ = 0.0;
    float storePrice_B_ = 0.0;
    float storePrice_C_ = 0.0;
    float storePrice_D_ = 0.0;
};

#endif // PRICESETDIALOG_H
