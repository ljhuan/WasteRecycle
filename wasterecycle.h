#ifndef WASTERECYCLE_H
#define WASTERECYCLE_H

#include <list>
#include <map>
#include <QMainWindow>
#include <QString>
#include <QTableView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QEvent>
#include <QSerialPort>
#include <QTimer>
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

    // void on_btn_modify_clicked();

    void on_vslider_percent_valueChanged(int value);

    void on_le_RoughWeigh_editingFinished();

    void on_btn_Statistics_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_le_RoughWeigh_returnPressed();

    void on_le_VehicleWeigh_returnPressed();

    void on_btn_delete_clicked();

    void on_btn_search_clicked();

    void on_btn_charts_clicked();

    void on_btn_averageChart_clicked();

    void priceChanged();

    void openSerialPort();

    void handleTimeout();  //定时器超时处理函数

    void on_btn_rWrite_clicked();

    void on_btn_vWrite_clicked();

    void on_btn_rClear_clicked();

    void on_btn_vClear_clicked();

    void on_le_com_textChanged(const QString &arg1);

    void on_tableView_unloading_doubleClicked(const QModelIndex &index);

private:
    Ui::WasteRecycle *ui;

    float fLevel1;
    float fLevel2;
    float fLevel3;
    float fLevel4;

    // bool bModifyFlag;

    // 卸货中的客户列表
    // std::map<int, ClientData> unloadingClientMap;
    // ClientData currentClient;
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
     // void updateListWidget(float level);
     void updateUnloadingTableView(float level);
     void updateTableView(float level);

     // 检查数据的合法性
     bool check();

     // 控制LineEdit是否可编辑，防止误输入
     void setTextEnabled(bool);

     // 更新数据统计栏
     void updateStatistics();
     void statistics();

     // tableView的初始化
     void initTableView();

     // 方向键处理
     void keyPressEvent(QKeyEvent *e);

     // enter键处理
     void enterEvent(QEvent* e);

     // 鼠标移入事件处理
     bool eventFilter(QObject *obj, QEvent *e);

     // 价格弹出显示
     void showPrice(float level);

     // clear
     void clearData();

     // next
     void nextVehicle();

     // delete
     void deleteData();

     // update charts table
     void updateTableCharts();

     void dataRecoveryFromTableView(const QModelIndex &index);

     void dataRecoveryFromUnloadingTableView(const QModelIndex &index);

     void saveCurrentData();

     void modifyData();

     void storeData(float level);

     bool checkVehicleWeighIsValid();

     bool bModify = false;
     bool bModifyUnloading = false;

     PriceSetDialog* priceSetWin;
     SqlOper* oper;
     QStandardItemModel* model;
     QStandardItemModel* model_unloading;

     bool bPriceInit;

     // 读取电子秤头数据相关参数和函数
     QSerialPort *m_serial = nullptr;
     void initActionsConnections();
     void readWeighBridgeData();
     void putWeighBridgeData(QByteArray &wbd);
     QByteArray weighBridgeData;
     float fWeight;

     // 定时器相关参数和函数
     QTimer *m_pTimer = nullptr;
};

#endif // WASTERECYCLE_H
