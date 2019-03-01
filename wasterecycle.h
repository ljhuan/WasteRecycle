#ifndef WASTERECYCLE_H
#define WASTERECYCLE_H

#include <list>
#include <map>
#include <mutex>
#include <QMainWindow>
#include <QString>
#include <QTableView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QEvent>
#include <QSerialPort>
#include <QTimer>
#include <QTextBrowser>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QImage>
#include <QtCharts>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
// #include <QThread>

#include "pricesetdialog.h"
#include "sqloper.h"
#include "cameramodel.h"
#include "opennetstream.h"
#include "opencv2\opencv.hpp"
#include "baidu_face_api.h"
#include "image_base64.h"
#include "cv_help.h"
#include "liveness.h"
#include "manager.h"
#include "compare.h"
#include "set/setting.h"
#include "image_buf.h"
#include "weighinfo.h"

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) > (y) ? (y) : (x))

typedef struct tagDeviceTableViewInfo
{
    QString strSubserial;
    int iChannelNo;
    bool bEncrypt;
    int  iVideoLevel;
} DeviceTableViewInfo;

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

    static void __stdcall videoDataHandler(DataType enType, char* const pData, int iLen, void* pUser);
    static void __stdcall messageHandler(const char* szSessionId, unsigned int iMsgType, unsigned int iErrorCode,const char *pMessageInfo, void *pUser);

    QLabel* label = nullptr;
    QChartView *chartView = nullptr;

    template<typename T>
    void dataProcess(T& data, int & coefficient, int base) {
        coefficient = 1;
        while(int(data)/base) {
            data /= base;
            coefficient *= base;
        }
    }

    template<typename T>
    void drawChart(std::map<QDateTime, T> & data) {
        for (auto serie : chartView->chart()->series()) {
            auto axisX = chartView->chart()->axisX(serie);
            auto axisY = chartView->chart()->axisY(serie);
            chartView->chart()->removeAxis(axisX);
            chartView->chart()->removeAxis(axisY);
        }
        chartView->chart()->removeAllSeries();


        QLineSeries *series = new QLineSeries();
        QPen pen;
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(4);
        pen.setColor(QColor(21, 100, 255));
        series->setPen(pen);//折现序列的线条设置
        //散点图(用于边框)
        QScatterSeries *series1 = new QScatterSeries();
        series1->setMarkerShape(QScatterSeries::MarkerShapeCircle);//圆形的点
        series1->setBorderColor(QColor(21, 100, 255));  //边框颜色
        series1->setBrush(QBrush(QColor(21, 100, 255)));//背景颜色
        series1->setMarkerSize(12);                     //点大小

        //散点图(用于中心)
        QScatterSeries *series2 = new QScatterSeries();
        series2->setMarkerShape(QScatterSeries::MarkerShapeCircle);//圆形的点
        series2->setBorderColor(QColor(21, 100, 255));//边框颜色
        series2->setBrush(QBrush(Qt::white));//背景颜色
        series2->setMarkerSize(8);//点大小


        if (data.size() == 0) return;
        T max = data.begin()->second;
        T min = data.begin()->second;
        for (auto element : data) {
            max = MAX(max, element.second);
            min = MIN(min, element.second);
            series->append(element.first.toMSecsSinceEpoch(), element.second);
            series1->append(element.first.toMSecsSinceEpoch(), element.second);
            series2->append(element.first.toMSecsSinceEpoch(), element.second);
        }

        chartView->chart()->addSeries(series);
        chartView->chart()->addSeries(series1);
        chartView->chart()->addSeries(series2);

        connect(series2, &QScatterSeries::hovered, this, &WasteRecycle::slotPointHoverd);//用于鼠标移动到点上显示数值

        // ...
        QDateTimeAxis *axisX = new QDateTimeAxis();
        axisX->setFormat("yyyy-MM-dd");
        auto it = data.rbegin();
        ++it;
        int days = data.begin()->first.daysTo(it->first);
        ++days;
        int param = (5 - days%5);
        param = (param==5 ? 0:param);

        axisX->setRange(data.begin()->first, it->first.addDays(param+1));
        // axisX->setTickCount((days+param)/5 + 1);
        axisX->setTickCount(6);
        qDebug() << "days=" << days << " param=" << param;

        chartView->chart()->setAxisX(axisX, series);
        chartView->chart()->setAxisX(axisX, series1);
        chartView->chart()->setAxisX(axisX, series2);

        QValueAxis* axisY = new QValueAxis();

        int coefficient = 1;
        dataProcess(max, coefficient, 10);
        int top = (int)(max+1)*coefficient;
        coefficient = 1;
        dataProcess(min, coefficient, 10);
        int bottom = (int)(min)*coefficient;

        axisY->setRange(bottom, top);
        chartView->chart()->setAxisY(axisY, series);
        chartView->chart()->setAxisY(axisY, series1);
        chartView->chart()->setAxisY(axisY, series2);

        chartView->resize(1000, 618);
        chartView->chart()->setAnimationDuration(QChart::SeriesAnimations);
        // chartView->chart()->createDefaultAxes();
        chartView->chart()->setTitle(QString::fromLocal8Bit("走势图"));
        chartView->chart()->legend()->hide();
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->show();
    }
public slots:
    void slotPointHoverd(const QPointF &point, bool state);

signals:
    void newTrackImage(const QString & imagePath);

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

    void on_btn_totalWeightChart_clicked();

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

    void slotDeviceTableViewPressed(const QModelIndex & index);

    // void on_btn_picCapture_clicked();

    void on_btn_roughWeightCapture_clicked();

    void on_btn_vechileWeightCapture_clicked();

    void replyCompareFinished(QNetworkReply* reply);

    void replyTokenFinished(QNetworkReply* reply);

    void on_btn_openRoughWeightDir_clicked();

    void on_btn_openVehicleWeightDir_clicked();

    void on_btn_reconnection_clicked();

    void clicked_rightMenu(const QPoint &pos);  //右键信号槽函数

    // delete
    void deleteData();
    void on_btn_monthlyStatics_clicked();

    void on_btn_capture_clicked();

    void on_btn_register_clicked();

    void on_btn_analyze_clicked();

    void on_btn_select_clicked();

    void analyze(const QString imgPath);

    void on_btn_faceAnalyze_clicked();

    void on_btn_logout_clicked();

    void setFlag(bool);
    void on_le_RoughWeigh_textChanged(const QString &arg1);

protected:
    void changeEvent(QEvent * event);

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

     // 读取电子秤头数据相关变量和函数
     QSerialPort *m_serial = nullptr;
     void initActionsConnections();
     void readWeighBridgeData();
     void putWeighBridgeData(QByteArray &wbd);
     QByteArray weighBridgeData;
     float fWeight;

     // 定时器相关参数和函数
     QTimer *m_pTimer = nullptr;


     // 获取码流相关变量及函数
     // ui controler
     QTableView* deviceTableView_;
     QTextBrowser* jsonTextBrowser_;
     // QMenu* realPlayMenu_;

     // ui data
     CameraModel* deviceTableModel_;

     enum TableIndex {
         DeviceTableIndex,
         JsonInfoTableIndex
     };

     bool bInit_ = false;
     QString acToken_;

     // user data
     QString sessionId_;
     QString devSerial_;
     int Channel_;
     bool bRealPlayStarted_ = false;
     QString videoPath_;
     QThread* thread_ = nullptr;

     bool  GetDeviceTableViewInfo(DeviceTableViewInfo& stDeviceInfo);
     int switchVideoLevel(int videoLevel);
     void showErrInfo(QString caption);
     void libInit();
     void getDeviceList();
     void startPlay();
     // QThread* thread_ = nullptr;

     const QString& curVideoPath();
     void setVideoPath(const QString devSerial);

     // baidu ai 人脸识别相关变量及函数
     QNetworkAccessManager *compareManager = nullptr;
     QNetworkAccessManager *tokenManager = nullptr;
     QString token_;
     QImage* rwImg1 = nullptr;
     QImage* vwImg2 = nullptr;

     void accquireToken();
     void startCompare();
     void postCompareData(QString & qstrImg1, QString & qstrImg2);
     void jsonCompareDataParser(QByteArray & relpyJson);

     // 右键菜单相关参数
     QMenu *rightMenu = nullptr;
     QAction *deleteAction = nullptr;

     // 百度离线人脸SDK相关参数
     BaiduFaceApi *api_ = nullptr;
     std::mutex apiMutex_;
     std::thread* t_ = nullptr;
     bool stop_ = false;
     bool pause_ = false;
     void face_collect_opencv_video();
     void identify(const QString & imgPath);
     void parseInfo(QString& info);
     void remove(const QString imgPath);
     WeighInfo* wi_ = nullptr;
     bool searchUnloadingTableByName(QString & name);
     // 信息界面点击确认 flag=true，点击取消 flag=false;
     bool flag_ = false;
     bool hasReturnZero = true;

     // 存储10个稳定过3秒的重量为可用重量
     std::mutex validWeightMapMutex;
     std::map<int, float> validWeightMap;
     int count = 0;
     float theTimingWeigh = 0;

     // 定时器相关参数和函数
     QTimer * weighTimer = nullptr;
     void weighTimerHandleTimeout();
};

#endif // WASTERECYCLE_H
