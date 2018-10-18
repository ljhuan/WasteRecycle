#ifndef WASTERECYCLE_H
#define WASTERECYCLE_H

#include <list>
#include <map>
#include <QMainWindow>
#include <QString>
#include "pricesetdialog.h"
#include "sqloper.h"

namespace Ui {
class WasteRecycle;
}

class ClientData {
public:
    ClientData():m_num(-1), m_qstrRw(""), m_qstrVw("") {}
    int m_num;
    QString m_qstrRw;
    QString m_qstrVw;
    // 记录时间的参数（todo）
};

class WasteRecycle : public QMainWindow
{
    Q_OBJECT

public:
    explicit WasteRecycle(QWidget *parent = 0);
    ~WasteRecycle();

private slots:
    void on_btn_Level1_clicked();

    void on_btn_Level2_clicked();

    void on_btn_Level3_clicked();

    void on_btn_Level4_clicked();

    void on_btn_Next_clicked();

    void on_cbb_NumSwitch_activated(const QString &arg1);

    void on_btn_PriceSet_clicked();

    void on_le_VehicleWeigh_textChanged(const QString &arg1);

    void on_btn_modify_clicked();

    void on_vslider_percent_valueChanged(int value);

private:
    Ui::WasteRecycle *ui;

    float fLevel1;
    float fLevel2;
    float fLevel3;
    float fLevel4;

    // bool bModifyFlag;

    // 卸货中的客户列表
    std::map<int, ClientData> unloadingClientMap;
    ClientData currentClient;
    int toBeUseIndex;

     // price calculate
     float priceCalculate(float level);
     void updatePrice();
     void finalPrice(float level);
     void writeData(float level);
     void updateListWidget(float level);
     bool check();

     PriceSetDialog* priceSetWin;
     SqlOper* oper;
};

#endif // WASTERECYCLE_H
