#ifndef WASTERECYCLE_H
#define WASTERECYCLE_H

#include <list>
#include <map>
#include <QMainWindow>
#include <QString>
#include <QTableView>
#include <QStandardItem>
#include <QStandardItemModel>
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

    void on_le_RoughWeigh_editingFinished();

    void on_btn_Statistics_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

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

     // 更新回收价格
     void updatePrice();

     // 计算价格
     void finalPrice(float level);

     // 将信息写入数据库
     void writeData(float level);

     // 更新过车记录表
     void updateListWidget(float level);
     void updateTableView(float level);

     // 检查数据的合法性
     bool check();

     // 控制LineEdit是否可编辑，防止误输入
     void setTextEnabled(bool);

     // 更新数据统计栏
     void updateStatistics();
     void statistics();

     void initTableView();

     PriceSetDialog* priceSetWin;
     SqlOper* oper;
     QStandardItemModel* model;
};

#endif // WASTERECYCLE_H
