﻿#include <Windows.h>
#include <time.h>
#include <synchapi.h>
#include <string>
#include <list>
#include <map>
#include <cctype>
#include <fstream>
#include <QDebug>
#include <QString>
#include <QComboBox>
#include <QDialog>
#include <QDateTime>
#include <QTextCodec>
#include <QMessageBox>
#include <utilitytools.h>
#include <QPixmap>
#include <QPalette>
#include <QBrush>
#include <QCoreApplication>
#include <QScrollBar>
#include <QKeyEvent>
#include <Qt>
#include <QCalendarWidget>
#include <QTimeEdit>
#include <QStringList>
#include <typeinfo>
#include <utility>
#include <QLineSeries>
#include <QtCharts>
#include <QChartView>
#include <QDateTimeAxis>
#include <QTimer>
#include <QDir>
#include <QDesktopServices>
#include <ctime>

#include "wasterecycle.h"
#include "ui_wasterecycle.h"
#include "sqloper.h"
#include "monthlystatics.h"
#include "opennetstream.h"
#include "opennetstream_p.h"
#include "weighinfo.h"
// #include "plaympeg4.h"

#define VNAME(value)  (#value)
#define TIMEOUT  (10)

QString towDecimalPlaces(QString data) {
    QString head = data.split('.').at(0);
    QString tail;
    if (data.split('.').size() > 1) {
        tail = data.split('.').at(1);
        if(tail.length() > 2) {
            tail.resize(2);
        }
    } else {
        tail = "00";
    }
    return head + '.' + tail;
}

bool isAllDigital(QString txt) {
    bool isAllDigit = true;
    for (auto e : txt.toStdString()) {
        if(!isdigit(e) && e != '.') {
            isAllDigit = false;
            break;
        }
    }
    return isAllDigit;
}

WasteRecycle::WasteRecycle(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WasteRecycle),
    toBeUseIndex(100),
    oper(new SqlOper),
    priceSetWin(nullptr),
    model(new QStandardItemModel),
    model_unloading(new QStandardItemModel),
//    fLevel1(0.0),
//    fLevel2(0.0),
//    fLevel3(0.0),
//    fLevel4(0.0),
    // bPriceInit(false),
    fWeight(0.0),
    kind_(QString::fromLocal8Bit("未分类"))
{
    ui->setupUi(this);
    priceSetWin = new PriceSetDialog(parent, oper);
    priceWin = new price(parent, oper);
    ui->le_RoughWeigh->clear();
    ui->le_VehicleWeigh->clear();

    // 按键的事件注册
//    ui->btn_Level1->installEventFilter(this);
//    ui->btn_Level2->installEventFilter(this);
//    ui->btn_Level3->installEventFilter(this);
//    ui->btn_Level4->installEventFilter(this);
    ui->btn_rClear->installEventFilter(this);
    ui->btn_vClear->installEventFilter(this);
    ui->lb_rwScaledImage->installEventFilter(this);
    ui->lb_vwScaledImage->installEventFilter(this);
    ui->lb_photo->installEventFilter(this);
    // ui->btn_huangban->installEventFilter(this);
    // ui->btn_tonghuo->installEventFilter(this);
    // ui->btn_jumin->installEventFilter(this);
    // ui->btn_chao1->installEventFilter(this);
    // ui->btn_chao2->installEventFilter(this);
    // ui->btn_chao3->installEventFilter(this);
    ui->btn_shuben->installEventFilter(this);
    ui->btn_baozhi->installEventFilter(this);
    ui->btn_tie->installEventFilter(this);
    ui->btn_buxiugang->installEventFilter(this);
    ui->btn_lv->installEventFilter(this);
    ui->btn_tong->installEventFilter(this);
    ui->btn_finalCal->installEventFilter(this);

    // 初始化tableView
    initTableView();

    QDateTime date = QDateTime::currentDateTime();
    QString today = date.toString("yyyy_MM_dd");
    std::list<QString> records, unloadings;
    records = oper->sqlQueryByDate(today);
    qDebug() << "records size:" <<  records.size();
    int i = 0;
    int k = 0;
    int q = 0;

    for (auto itor = records.begin(); i < records.size(); ++i, ++itor) {
        QStringList lines = itor->split("  ");

        if(lines.at(6).trimmed() == "") {  // 未结算的塞入unloading表格
            for(int j = 0; j < lines.length(); ++j) {
                model_unloading->setItem(q, j, new QStandardItem(lines.at(j).trimmed()));
            }
            ++q;
        } else {  // 已结算的塞入历史表格
            for(int j = 0; j < lines.length(); ++j) {
                model->setItem(k, j, new QStandardItem(lines.at(j).trimmed()));
            }
            ++k;
        }

        toBeUseIndex = lines.at(0).toInt() + 1;
    }

    unloadings = oper->sqlQueryUnloadingByDate(today);
    for (auto e : unloadings) {
        QString tim = e.split("  ").at(0);
        QString idx = e.split("  ").at(1);
        QString rW = e.split("  ").at(2);
        int row = model_unloading->rowCount();
        model_unloading->setItem(row, 0, new QStandardItem(idx));
        model_unloading->setItem(row, 1, new QStandardItem(tim));
        model_unloading->setItem(row, 2, new QStandardItem(rW));
        model_unloading->setItem(row, 3, new QStandardItem(QString::fromLocal8Bit("卸货中")));

        // ui->le_RoughWeigh->setText(rW);
        if (idx.toInt() >= toBeUseIndex) {
            toBeUseIndex = idx.toInt() + 1;
        }
    }
    model->sort(0);
    model_unloading->sort(0);

    // 隐藏各项数据统计
    ui->lb_Amount->setHidden(true);
    ui->lb_Amount0->setHidden(true);
    ui->lb_AveragePrice->setHidden(true);
    ui->lb_AveragePrice0->setHidden(true);
    ui->lb_TotalPrices->setHidden(true);
    ui->lb_TotalPrices0->setHidden(true);
    ui->lb_TotalWeight->setHidden(true);
    ui->lb_TotalWeight0->setHidden(true);
    ui->lb_TotalProfit->setHidden(true);
    ui->lb_TotalProfit0->setHidden(true);

    ui->lb_CurrNum->setText(QString::fromStdString(std::to_string(toBeUseIndex)));

    updatePrice();
//    float ps = (fLevel2-fLevel3)/(fLevel1-fLevel3)  * 100;
//    int pos = ps;
    // qDebug() << "ps:" << ps << "  pos:" << pos;

//    ui->vslider_percent->setMaximum(5);
//    ui->vslider_percent->setMinimum(-5);
//    // bPriceInit = true;
//    ui->vslider_percent->setValue(0);
    ui->le_fineAdjustment->setText(QString("%1").arg(0));
    // 配置背景图
//    QPixmap pixmap = QPixmap( ":/images/0.jpg").scaled(this->size());
//    QPalette  palette (this->palette());
//    palette .setBrush(QPalette::Background, QBrush(pixmap));
//    this->setPalette( palette );

    // 设置标签图片
    QPixmap titlePix = QPixmap(":/images/mimeiti_da.png").scaled(600, 160);
    ui->lb_display->setPixmap(titlePix);
    ui->lb_display->show();

    QPixmap pricePix = QPixmap(":/images/price_big.png").scaled(200, 80);
    ui->label_9->setPixmap(pricePix);
    ui->label_9->show();

    QPixmap yuanPix = QPixmap(":/images/yuan_big.png").scaled(80, 80);
    ui->label_8->setPixmap(yuanPix);
    ui->label_8->show();

    // 设置默认的焦点
    ui->le_RoughWeigh->setFocus();

    // connect(priceSetWin, SIGNAL(finished(int)), this, SLOT(priceChanged()));

    wi_ = new WeighInfo(this, oper);
    wi_->setWindowFilePath(QString::fromLocal8Bit("过磅信息"));
    connect(wi_, SIGNAL(flag(bool)), this, SLOT(setFlag(bool)));
    // 日历设置
    QCalendarWidget* calendar = new QCalendarWidget(this);
    calendar->hide();

    ui->dateEdit->setCalendarPopup(true);
    ui->dateEdit->setDisplayFormat("yyyy-MM-dd");
    ui->dateEdit->setDate(calendar->selectedDate());

    ui->de_date->setCalendarPopup(true);
    ui->de_date->setDisplayFormat("yyyy_MM_dd");
    ui->de_date->setDate(calendar->selectedDate());

    ui->de_from->setCalendarPopup(true);
    ui->de_from->setDisplayFormat("yyyy_MM_dd");
    ui->de_from->setDate(calendar->selectedDate());

    ui->de_to->setCalendarPopup(true);
    ui->de_to->setDisplayFormat("yyyy_MM_dd");
    ui->de_to->setDate(calendar->selectedDate());

    // 串口设置
    m_serial = new QSerialPort();
    initActionsConnections();
    // 定时器设置
    m_pTimer = new QTimer(this);
    weighTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    m_pTimer = new QTimer(this);
    connect(weighTimer, SIGNAL(timeout()), this, SLOT(weighTimerHandleTimeout()));
    // 登录摄像头并获取码流
    deviceTableView_ = new QTableView(this);
    deviceTableView_->setContextMenuPolicy(Qt::CustomContextMenu);
    deviceTableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    deviceTableView_->setSelectionMode(QAbstractItemView::SingleSelection);
    deviceTableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tw_devices->addTab(deviceTableView_, QString::fromLocal8Bit("设备列表"));

    connect(deviceTableView_, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotDeviceTableViewPressed(const QModelIndex &)));

    deviceTableModel_ = new CameraModel(this);
    deviceTableView_->setModel(deviceTableModel_);
    // deviceTableView_->setColumnWidth(1, ui->tw_devices->width() - 10);  // 该方法要放在设置完model之后调用，否则不起作用

    jsonTextBrowser_ = new QTextBrowser(this);
    ui->tw_devices->addTab(jsonTextBrowser_, tr("json"));


    thread_ = new QThread(this);
    connect(thread_, &QThread::started, this, [&](){
        libInit();
        getDeviceList();
    });
    connect(thread_, &QThread::finished, thread_, &QObject::deleteLater);
    thread_->start();
    qDebug() << "[WasteRecycle] thread start";
    // getDeviceList();

    // baidu ai 人脸识别相关设置
//    compareManager = new QNetworkAccessManager(this);
//    connect(compareManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyCompareFinished(QNetworkReply*)));

//    tokenManager = new QNetworkAccessManager(this);
//    connect(tokenManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyTokenFinished(QNetworkReply*)));

//    accquireToken();

    // 折线图
    if(chartView == nullptr)  {
        chartView = new QChartView();
        chartView->chart()->setBackgroundBrush(QBrush(QColor(248, 251, 255)));
    }
    if (label == nullptr) {
        label = new QLabel("坐标", chartView);
        label->setStyleSheet("color:blue; background-color:rgb(78, 199, 255);border:2px groove gray;border-radius:5px;padding:2px 4px;");
        QFont font;
        font.setBold(true);
        label->setFont(font);
        label->hide();
    }

    // 右键删除
    ui->tableView_unloading->setContextMenuPolicy(Qt::CustomContextMenu);  //少这句，右键没有任何反应的。
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    rightMenu = new QMenu;  //右键菜单
    deleteAction = new QAction(QString::fromLocal8Bit("删除"),this);  //删除
    rightMenu->addAction(deleteAction);

    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(clicked_rightMenu(QPoint)));
    connect(ui->tableView_unloading, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(clicked_rightMenu(QPoint)));
    connect(deleteAction, SIGNAL(triggered(bool)), this, SLOT(deleteData()));
    connect(this, SIGNAL(newTrackImage(QString)), this, SLOT(analyze(QString)));

    connect(ui->le_fineAdjustment, SIGNAL(clicked()), this, SLOT(myLineEditClicked()));

    connect(ui->le_huangban, SIGNAL(clicked()), this, SLOT(lineEditClicked()));
    connect(ui->le_chaoshi, SIGNAL(clicked()), this, SLOT(lineEditClicked()));
    connect(ui->le_tonghuo, SIGNAL(clicked()), this, SLOT(lineEditClicked()));

    /*baiduThread_ = new QThread(this);
    connect(baiduThread_, &QThread::started, this, [&](){
        // 百度人脸库初始化
        //api实例指针
        api_ = new BaiduFaceApi();
        //初始化sdk
        // 若采用证件照模式，请把id_card设为true，否则为false，证件照模式和非证件照模式提取的人脸特征值不同，
        // 不能混用
        bool id_card = false;
        api_->sdk_init(id_card);
        // 提前加载人脸库到内存
        api_->load_db_face();
        qDebug() << ">>>>>>> api load face db";
    });
    connect(baiduThread_, &QThread::finished, thread_, &QObject::deleteLater);
    baiduThread_->start();*/

    // 判断人名映射表是否存在
    /* QStringList tables = oper->sqlGetAllTableName();
    if (!tables.contains("members")) {
        QString createChartsTable = "create table members (phone varchar(100), name varchar(10), time varchar(10));";
        oper->createTable(createChartsTable);
    } else {
        std::list<QString> elements;

        QString queryMembersTable = "select * from members;";
        elements = oper->queryTableMembers(queryMembersTable);

        for (auto element : elements) {
            QStringList line = element.split("  ");
            QString phone =  line.at(0);
            QString name = line.at(1);
            phoneNameMap_[phone] = name;
        }

        for (auto e : phoneNameMap_) {
            qDebug() << ">>>>>>> members:" << e;
        }
    }*/
}

WasteRecycle::~WasteRecycle()
{
    OpenNetStream::getInstance()->stopRealPlayEx(sessionId_);
    OpenNetStream::getInstance()->freeSession(sessionId_);
    OpenNetStream::getInstance()->releaseLibrary();
    if (thread_ != nullptr && thread_->isRunning()) {
        qDebug() << "thread quit...";
        thread_->quit();
    }
    if(baiduThread_ != nullptr && baiduThread_->isRunning()) {
        qDebug() << "baidu thread quit...";
        baiduThread_->quit();
    }
    if (t_ != nullptr) {
        stop_ = true;
    }
    delete ui;
    ui = nullptr;
    delete priceSetWin;
    priceSetWin = nullptr;
    delete priceWin;
    priceWin = nullptr;
    delete oper;
    oper = nullptr;
    delete model;
    model = nullptr;
    delete model_unloading;
    model_unloading = nullptr;
    if(m_serial != nullptr) {
        m_serial->close();
        delete m_serial;
        m_serial = nullptr;
    }
    if(m_pTimer != nullptr) {
        m_pTimer->stop();
        delete m_pTimer;
        m_pTimer = nullptr;
    }
    if (chartView != nullptr) {
        delete chartView;
        chartView = nullptr;
    }
//    if(label != nullptr) {
//        delete label;
//        label = nullptr;
//    }
}

void WasteRecycle::getDeviceList() {
    qDebug() << "[WasteRecycle] getDeviceList IN";
    void* pBuf = NULL;
    int length = 0;

    int iRet = OpenNetStream::getInstance()->getDevListEx(0, 1000, &pBuf, &length);
    if (iRet != OPEN_SDK_NOERROR)
    {
        qDebug() << "Get Device List failed!";
        return ;
    }

    QString json;
    json = QString::fromLocal8Bit(static_cast<char*>(pBuf));
    OpenNetStream::getInstance()->freeData(pBuf);
    qDebug() << json;

    QByteArray jsonByte = json.toUtf8();
    deviceTableModel_->setCameraModel(jsonByte);
    jsonTextBrowser_->clear();
    Json::Reader reader;
    Json::Value	value;
    if(reader.parse(jsonByte.data(), value)) {
        jsonTextBrowser_->setText( value.toStyledString().c_str() );
    }
    ui->tw_devices->setCurrentIndex(DeviceTableIndex);
    qDebug() << "[WasteRecycle] getDeviceList OUT";
}

void WasteRecycle::setVideoPath(const QString devSerial)
{
    time_t rawtime;
    time (&rawtime);
    struct tm * timeinfo = localtime(&rawtime);
    const int buflen = 32;
    char buffer[buflen] = {0};
    strftime(buffer, buflen, "%Y%m%d%H%M%S", timeinfo);
    const QString strFlag = devSerial + "_" + buffer;
    QString path = QCoreApplication::applicationDirPath();
    videoPath_ = path + "/" + strFlag + ".mp4";
}

void WasteRecycle::accquireToken()
{
    qDebug() << "postCompareData IN";
    //设置请求地址
    QString postUrl = "https://aip.baidubce.com/oauth/2.0/token";
    QString grantType = "client_credentials";
    QString clientId = "hGa7I3CFtlEz5vYXPwGFzEIV";
    QString clientSecret = "jWYj948TSsiASGoExDDFSmI3sKdZ3f4i";
    QUrl url(postUrl + "?grant_type=" + grantType + "&client_id=" + clientId + "&client_secret=" + clientSecret);
    qDebug() << "url:" << QString(postUrl + "?grant_type=" + grantType + "&client_id=" + clientId + "&client_secret=" + clientSecret);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json; charset=UTF-8"));

    tokenManager->post(request, "");
}

void WasteRecycle::postCompareData(QString &qstrImg1, QString &qstrImg2)
{
    qDebug() << "postCompareData IN";
    //设置请求地址
    QString requestUrl = "https://aip.baidubce.com/rest/2.0/face/v3/match";
    // QString accessToken = "24.3dca1650af374957e93bf267459ba1fe.2592000.1547547675.282335-14622857";
    QUrl url(requestUrl + "?access_token=" + token_);
    QNetworkRequest request(url);

    //设置数据提交格式，这个不能自己随便写，每个平台的格式可能不一样，百度AI要求的格式为application/json
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    QJsonObject obj1, obj2;
    QJsonArray arr;
    // QJsonDocument doc;

    obj1.insert("image", qstrImg1);
    obj1.insert("image_type", "BASE64");
    obj2.insert("image", qstrImg2);
    obj2.insert("image_type", "BASE64");

    arr.append(obj1);
    arr.append(obj2);
    // doc.setArray(arr);

    QByteArray postParam = QJsonDocument(arr).toJson();

    /*QStringList list;
    list.append("[");
    list.append(QString("{\"image\":\"%1\",\"image_type\":\"BASE64\",\"liveness_control\":\"NONE\"}").arg(qstrImg1));
    list.append(",");
    list.append(QString("{\"image\":\"%1\",\"image_type\":\"BASE64\",\"liveness_control\":\"NONE\"}").arg(qstrImg2));
    list.append("]");
    QString data = list.join("");*/
    compareManager->post(request, postParam);
    qDebug() << "postCompareData OUT";
}

void WasteRecycle::jsonCompareDataParser(QByteArray &relpyJson)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(relpyJson, &err);
    QString toShowInfo;
    if(err.error == QJsonParseError::NoError) {
        if(doc.isObject()) {
            QJsonObject obj = doc.object();
            if(obj.contains("result")) {
                QJsonObject resultObj = obj.take("result").toObject();
                if(resultObj.contains("score")) {
                    double score = resultObj.take("score").toDouble();
                    qDebug() << "相似度为：" << score;
                    ui->lb_display->setText(QString("相似度为：%1").arg(score));
                }
            }
        }
    }
}

void WasteRecycle::startCompare()
{
    qDebug() << "startCompare IN";
    // 图片进行base64编码
    QByteArray ba1, ba2;
    QString qstrImg1, qstrImg2;
    QBuffer buffer1(&ba1), buffer2(&ba2);
    buffer1.open(QIODevice::WriteOnly);
    buffer2.open(QIODevice::WriteOnly);

    if (rwImg1 != nullptr) {
         rwImg1->save(&buffer1, "jpg");
         qstrImg1 = QString(ba1.toBase64());;
    }

    if (vwImg2 != nullptr) {
        vwImg2->save(&buffer2, "jpg");
        qstrImg2 = QString(ba2.toBase64());
    }

    buffer1.close();
    buffer2.close();


    postCompareData(qstrImg1, qstrImg2);


    qDebug() << "startCompare OUT";
}
const QString& WasteRecycle::curVideoPath()
{
    return videoPath_;
}

void WasteRecycle::videoDataHandler(DataType enType, char* const pData, int iLen, void* pUser)
{
//    qDebug() << __LINE__ << __FUNCTION__ <<"enType:"<< enType << "iLen:" << iLen;
//    WasteRecycle * mainWins = (WasteRecycle *)pUser;
//    if(mainWins->curVideoPath().isEmpty())
//    {
//        return ;
//    }

    // 保存录像
//    if (true)
//    {
//        std::ofstream file;
//        file.open(mainWins->curVideoPath().toStdString().c_str(), std::ios::binary|std::ios::app);
//        file.write(pData,iLen);
//        file.flush();
//        file.close();
//    }
}

void WasteRecycle::messageHandler(const char *szSessionId, unsigned int iMsgType, unsigned int iErrorCode, const char *pMessageInfo, void *pUser)
{
    qDebug() << __LINE__ << __FUNCTION__ << szSessionId << iMsgType << iErrorCode << pMessageInfo;
    WasteRecycle* pInstance = static_cast<WasteRecycle*>(pUser);
    if (!pInstance) {
        return;
    }

    switch (iMsgType) {
    case INS_PLAY_EXCEPTION:
        // emit pInstance->insPlayException(iErrorCode, pMessageInfo);
        break;
    case INS_PLAY_RECONNECT:
        break;
    case INS_PLAY_RECONNECT_EXCEPTION:
        // emit pInstance->insPlayReconnectException(iErrorCode, pMessageInfo);
        break;
    case INS_PLAY_START:
        // emit pInstance->insPlayStart();
        break;
    case INS_PLAY_STOP:
        // emit pInstance->insPlayStop();
        break;
    case INS_PLAY_ARCHIVE_END:
        // emit pInstance->insPlayBackEnd(iErrorCode);
        break;
    case INS_RECORD_FILE:
        // emit pInstance->insRecordFile(pMessageInfo);
        break;
    case INS_RECORD_SEARCH_END:
        break;
    case INS_RECORD_SEARCH_FAILED:
        // emit pInstance->insRecordSearchFailed(iErrorCode, pMessageInfo);
        break;
    }
}

void WasteRecycle::slotPointHoverd(const QPointF &point, bool state)
{
    qDebug() << "slotPointHoverd IN";
    if(state) {
        QString xTime = QDateTime::fromMSecsSinceEpoch(point.x()).toString("yyyy-MM-dd");
        qDebug() << "x:" << xTime;
        qDebug() << "y:" << point.y();
        QString text = QString("%1").arg(point.y()) + "\r\n" + xTime;
        label->setText(text);

        QPoint curPos = chartView->mapFromGlobal(QCursor::pos());
        qDebug() << "curPos x:" << curPos.x();
        qDebug() << "curPos y:" << curPos.y();
        label->move(curPos.x() - label->width()/2, curPos.y() - label->height()*1.5);  //移动数值
        label->show();  //显示出来

        QScatterSeries* serie = reinterpret_cast<QScatterSeries*>(chartView->chart()->series()[1]);
        serie->remove(point);
    } else {
        label->hide();
        QScatterSeries* serie = reinterpret_cast<QScatterSeries*>(chartView->chart()->series()[1]);
        serie->append(point);
    }
    qDebug() << "slotPointHoverd OUT";
}

void WasteRecycle::libInit()
{
    QString authAddress = "https://openauth.ys7.com";
    QString platformAddress = "https://open.ys7.com";

    QString appKey = "1db6d18b1ba24c918a42ef2b1cbed71c";
    OpenNetStream::getInstance()->initLibrary(authAddress, platformAddress, appKey);
    acToken_ = OpenNetStream::getInstance()->login();
    bInit_ = true;
    sessionId_ = OpenNetStream::getInstance()->allocSessionEx(WasteRecycle::messageHandler, this);
    OpenNetStream::getInstance()->setStreamTrans();

    // 日志等级设置
    OpenNetStream::getInstance()->setLogLevel(5);

    // token设置
    OpenNetStream::getInstance()->setAccessToken(acToken_);
    const char* pTokenExpireTime = OpenNetStream::getInstance()->getTokenExpireTime();

    if (pTokenExpireTime != NULL)
    {
        qDebug() << __LINE__ << __FUNCTION__ << "Token Expire Time:" << pTokenExpireTime;
    }
    const char* pAreaDomain = OpenNetStream::getInstance()->getAreaDomain();
    if (pAreaDomain != NULL)
    {
        qDebug() << __LINE__ << __FUNCTION__ << "Platform Domain:" << pAreaDomain;
    }
}

bool  WasteRecycle::GetDeviceTableViewInfo(DeviceTableViewInfo& stDeviceInfo)
{
    if (ui->tw_devices->currentIndex() == DeviceTableIndex) {
        QModelIndex index = deviceTableView_->currentIndex();
        if (!index.isValid()) {
            QMessageBox::information(this, tr("Device List"), tr("Please select a device in device list"));
            ui->tw_devices->setCurrentIndex(DeviceTableIndex);
            return false;
        }
        stDeviceInfo.strSubserial = deviceTableModel_->getSerial(index);
        stDeviceInfo.iChannelNo = deviceTableModel_->getCameraNo(index);
        stDeviceInfo.bEncrypt = deviceTableModel_->getIsEncrypt(index);
        stDeviceInfo.iVideoLevel = deviceTableModel_->getVideoLevel(index);
    } else {
        QMessageBox::information(this, tr("Device List"), tr("Please select a device in device list"));
        ui->tw_devices->setCurrentIndex(DeviceTableIndex);
        return false;
    }
    return true;
}

int WasteRecycle::switchVideoLevel(int videoLevel)
{
    DeviceTableViewInfo stDeviceInfo;
    if (GetDeviceTableViewInfo(stDeviceInfo) == false)
    {
        return -1;
    }

    // bool bEncrypt = stDeviceInfo.bEncrypt;
    QString devSerial = stDeviceInfo.strSubserial;
    int iChannelNo = stDeviceInfo.iChannelNo;

    int iRet = OpenNetStream::getInstance()->setVideoLevel(devSerial, iChannelNo, videoLevel);
    if(0 != iRet)
    {
        this->showErrInfo(tr("Switch Video Level"));
        return -1;
    }
    return 0;
}

void WasteRecycle::showErrInfo(QString caption)
{
    int iErrCode = OpenNetStream::getInstance()->GetLastErrorCode();
    char szBuf[64] = {0};
    sprintf(szBuf, "ErrorCode:%d", iErrCode);
    QMessageBox::information(this, caption, szBuf);
}

void WasteRecycle::startPlay()
{
    qDebug() << "[WasteRycycle] startPlay IN";
    DeviceTableViewInfo stDeviceInfo;
    if (GetDeviceTableViewInfo(stDeviceInfo) == false)
    {
        return;
    }

    bool bEncrypt = stDeviceInfo.bEncrypt;
    QString devSerial = stDeviceInfo.strSubserial;
    int iChannelNo = stDeviceInfo.iChannelNo;

    devSerial_ = devSerial;
    Channel_ = iChannelNo;

    QString safekey;
    safekey.clear();
    if(bEncrypt)
    {
        bool ok;
        safekey = QInputDialog::getText(this, tr("Input"), tr("Please Input the verification code"), QLineEdit::Password, "", &ok);
        if (ok == false)
        {
            return;
        }
    }
    //Set the video store path
     setVideoPath(devSerial);
     OpenNetStream::getInstance()->setDataCallBack(sessionId_, videoDataHandler, this);

    HWND hWnd = NULL;
    hWnd = (HWND)ui->w_playWindow->winId();

    // QThread* thread = new QThread();
    // ui->w_playWindow->moveToThread(thread);
    // connect(thread, &QThread::started, this, [&](){
        int iRet = OpenNetStream::getInstance()->startRealPlay(sessionId_, hWnd, devSerial, iChannelNo, safekey);
        if(0 != iRet)
        {
            this->showErrInfo(tr("RealPlay"));
            return;
        }
    // });
    // connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    // thread->start();

    bRealPlayStarted_ = true;
    qDebug() << "[WasteRycycle] startPlay OUT";
}

void WasteRecycle::slotDeviceTableViewPressed(const QModelIndex & index)
{
    DeviceTableViewInfo stDeviceInfo;
    if (GetDeviceTableViewInfo(stDeviceInfo) == false)
    {
        return ;
    }
    QString devSerial = stDeviceInfo.strSubserial;
    int iChannelNo = stDeviceInfo.iChannelNo;
    // int iLevel  = stDeviceInfo.iVideoLevel;

    // updateVideoLevelBtn(iLevel);

    QString safekey;
    safekey.clear();
    void* pBuf = NULL;
    int iLen = 0;
    int iRet = OpenNetStream::getInstance()->GetDevDetailInfo(devSerial, iChannelNo, false, &pBuf, &iLen);
    if(iRet != 0)
    {
        return;
    }
    QString json = QString::fromLocal8Bit(static_cast<char*>(pBuf));
    OpenNetStream::getInstance()->freeData(pBuf);
    QByteArray jsonByte = json.toUtf8();
     jsonTextBrowser_->setText(QString(jsonByte));
    Json::Reader reader;
    Json::Value	value;
    if(reader.parse(jsonByte.data(), value)) {
        Json::Value lVideoQuality = value["videoQualityInfos"];
        if (lVideoQuality.isArray())
        {
            // realPlayMenu_->clear();
            for (int i=0; i < lVideoQuality.size(); i++)
            {
                Json::Value videoQualityObj = lVideoQuality[i];
                if (!videoQualityObj["videoQualityName"].isString() || !videoQualityObj["videoLevel"].isInt())
                {
                    continue;
                }
                // std::string strVideoQualityName = videoQualityObj["videoQualityName"].asString();
                int iVideoLevel = videoQualityObj["videoLevel"].asInt();
                switchVideoLevel(iVideoLevel);
            }
        }
    }
//    thread_ = new QThread(this);
//    connect(thread_, &QThread::started, this, &WasteRecycle::startPlay);
//    connect(thread_, &QThread::finished, thread_, &QObject::deleteLater);
//    thread_->start();
    startPlay();

//    QThread* thread = new QThread(this);
//    connect(thread, &QThread::started, this, [&](){
//        face_collect_opencv_video();
//    });
//    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
//    thread->start();
    qDebug() << "main thread is running...";
    // face_collect_opencv_video();
}

void WasteRecycle::face_collect_opencv_video() {
    qDebug() << ">>>>>>> [face_collect_opencv_video] face_collect_opencv_video IN";

    // cv::VideoCapture cap("rtsp://admin:XJITJW@192.168.0.102:554/h264/ch33/main/av_stream");
    QString url = priceSetWin->url_;
    qDebug() << ">>>>>>> [face_collect_opencv_video] url:" << url;
    cv::VideoCapture cap(url.toStdString());
    if (!cap.isOpened())
    {
        std::cout << "open camera error" << std::endl;
        return;
    }
    cv::Mat frame;

    // int index = 0;
    bool save_file = false;
    float max_score = 1;
    long faceID = -1;
    int s = 0, e = 0;

    cv::RotatedRect box;
    std::vector<TrackFaceInfo> *track_info = new std::vector<TrackFaceInfo>();

    // api_->set_isCheckQuality(true);
    // api_->set_detect_in_video_interval(1000);

    int count = 0;
    int size = 0;
    while (!stop_) {
        cap >> frame;
        track_info->clear();

        if (pause_) {
            Sleep(20);
            continue;
        }

        if(count == 5) {
            count = 0;
            // emit newTrackImage(QString::fromStdString("abc"));
            std::unique_lock<std::mutex> guard(apiMutex_);
            s = clock();
            size = api_->track_max_face(track_info, frame);
            e = clock();

            qDebug() << ">>>>>>> [face_collect_opencv_video] api_->track_max_face : " << (e - s) << " ms";
        } else {
            ++count;
            size = 0;
        }
        for (int i = 0; i < size; i++) {
            TrackFaceInfo info = track_info->at(i);
            qDebug() << ">>>>>>> [face_collect_opencv_video] in net_track score is:" << info.score;
            qDebug() << ">>>>>>> [face_collect_opencv_video] in net_track face_id:" << info.face_id;

            if (info.face_id != faceID && info.score > 0.9) {
                faceID = info.face_id;
                max_score = info.score;
                save_file = true;
            } else if(max_score < info.score) {
                max_score = info.score;
                save_file = true;
            } else {
                break;
            }

            // 人脸信息
            FaceInfo ibox = info.box;
            // 角度
            std::cout << "in net_track mAngle is:" << ibox.mAngle << std::endl;
            // 人脸宽度
            // std::cout << "in net_track mWidth is:" << ibox.mWidth << std::endl;
            // 中心点X,Y坐标
            // std::cout << "in net_track mCenter_x is:" << ibox.mCenter_x << std::endl;
            // std::cout << "in net_track mCenter_y is:" << ibox.mCenter_y << std::endl;
            // std::vector<float> headPose = info.headPose;
            // 返回 x，y，z三个角度的人脸角度
//            for (int k = 0; k < headPose.size(); k++) {
//                float angle = headPose.at(k);
//                std::cout << "in net_track angle is:" << angle << std::endl;
//            }
            // 以下注释代码为检测人脸的同时可获取属性和质量，建议不要全部都打开实时获取，否则可能会视频卡顿
            // 实时获取人脸属性
            // std::string res_attr = face_attr_by_landmark(frame, info.landmarks);
            // std::cout << "attr result is:" << res_attr << std::endl;
            // 实时获取人脸质量
            //  std::string res_quality = face_quality_by_landmark(frame, info.landmarks);
            //  std::cout << "quality result is:" << res_quality << std::endl;


            // frame为视频帧,可根据采集到的人脸信息筛选需要的帧
            // 以下为保存图片到本地的示例,可根据采集信息有选择的保存
            if (save_file) {
                save_file = false;
                QDir dir;
                QString dirName = "track";
                if(!dir.exists(dirName)) {
                    dir.mkdir(dirName);
                }
                s = clock();
                std::string fileName = QDir::currentPath().toStdString() + "/track/track_" + std::to_string(info.face_id) + "_" + std::to_string(info.score) + ".jpg";
                cv::imwrite(fileName, frame);
                e = clock();
                qDebug() << ">>>>>>> [face_collect_opencv_video] write image : " << (e - s) << " ms";
                // std::this_thread::sleep_for(std::chrono::milliseconds(500));
                // analyze(QString::fromStdString(fileName));

//                std::string featureFileName = QDir::currentPath().toStdString() + "/track/track_" + std::to_string(info.face_id) + "_" + std::to_string(info.score) + "_feature.jpg";
//                // 画人脸框
//                box = CvHelp::bounding_box(info.landmarks);
//                CvHelp::draw_rotated_box(frame, box, cv::Scalar(0, 255, 0));
//                // 画关键点轮廓
//                CvHelp::draw_shape(info.landmarks, frame, cv::Scalar(0, 255, 0));
//                cv::imwrite(featureFileName, frame);

                // 暂停获取新的视频帧，等待人脸识别结果
                on_btn_startAnalyze_clicked();
                emit newTrackImage(QString::fromStdString(fileName));
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        if (!frame.empty()) {
//            IplImage img = frame;
//            QImage image = QImage((const uchar*)img.imageData, img.width, img.height, QImage::Format_RGB888).rgbSwapped();
//            int w = ui->lb_show->width();
//            int h = ui->lb_show->height();
//            ui->lb_show->setPixmap(QPixmap::fromImage(image).scaled(w, h));
//            cv::Mat outFrame;
//            cv::resize(frame, outFrame, cv::Size(640, 480));
//            // cv::resizeWindow("face", 640, 480);
//            imshow("face", outFrame);
//            cv::waitKey(1);
//            outFrame.release();
        } else {
            std::cout << "mat is empty" << std::endl;
            cap.open(url.toStdString());
        }
        // Sleep(10);
        // frame.release();

        /*imshow("face", frame);
        cv::waitKey(1);*/
        //  frame.release();
    }

    delete track_info;
    qDebug() << "[face_collect_opencv_video] face_collect_opencv_video OUT";
}

void WasteRecycle::initTableView() {
    model->setHorizontalHeaderItem(0, new QStandardItem(QString::fromLocal8Bit("号码")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString::fromLocal8Bit("时间")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QString::fromLocal8Bit("毛重")));
    model->setHorizontalHeaderItem(3, new QStandardItem(QString::fromLocal8Bit("车重")));
    model->setHorizontalHeaderItem(4, new QStandardItem(QString::fromLocal8Bit("净重")));
    model->setHorizontalHeaderItem(5, new QStandardItem(QString::fromLocal8Bit("单价")));
    model->setHorizontalHeaderItem(6, new QStandardItem(QString::fromLocal8Bit("价格")));
    model->setHorizontalHeaderItem(7, new QStandardItem(QString::fromLocal8Bit("姓名")));
    model->setHorizontalHeaderItem(8, new QStandardItem(QString::fromLocal8Bit("种类")));

    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:rgb(230, 253, 255);"
                                                           "color: black;padding-left: 4px;border: 1px solid #6c6c6c;}");


    ui->tableView->verticalHeader()->setStyleSheet("QHeaderView::section {background-color:rgb(230, 253, 255);"
                                                         "color: black;padding-left: 4px;border: 1px solid #6c6c6c;}");
    //根据内容来确定列宽度
     ui->tableView->resizeColumnsToContents();
     ui->tableView->horizontalHeader();
    ui->tableView->setColumnWidth(0, 40);
    ui->tableView->setColumnWidth(1, 160);
    ui->tableView->setColumnWidth(2, 55);
    ui->tableView->setColumnWidth(3, 55);
    ui->tableView->setColumnWidth(4, 55);
    ui->tableView->setColumnWidth(5, 50);
    ui->tableView->setColumnWidth(6, 60);
    ui->tableView->setColumnWidth(7, 55);
    ui->tableView->setColumnWidth(8, 50);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);  // 设置选中模式为选中行
    ui->tableView->setSelectionMode( QAbstractItemView::SingleSelection);  // 设置选中单个
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 设置不可编辑

    model_unloading->setHorizontalHeaderItem(0, new QStandardItem(QString::fromLocal8Bit("号码")));
    model_unloading->setHorizontalHeaderItem(1, new QStandardItem(QString::fromLocal8Bit("时间")));
    model_unloading->setHorizontalHeaderItem(2, new QStandardItem(QString::fromLocal8Bit("毛重")));
    model_unloading->setHorizontalHeaderItem(3, new QStandardItem(QString::fromLocal8Bit("车重")));
    model_unloading->setHorizontalHeaderItem(4, new QStandardItem(QString::fromLocal8Bit("净重")));
    model_unloading->setHorizontalHeaderItem(5, new QStandardItem(QString::fromLocal8Bit("单价")));
    model_unloading->setHorizontalHeaderItem(6, new QStandardItem(QString::fromLocal8Bit("价格")));
    model_unloading->setHorizontalHeaderItem(7, new QStandardItem(QString::fromLocal8Bit("姓名")));
    model_unloading->setHorizontalHeaderItem(8, new QStandardItem(QString::fromLocal8Bit("种类")));

    ui->tableView_unloading->setModel(model_unloading);
    ui->tableView_unloading->horizontalHeader()->setStyleSheet("QHeaderView::section {"
                                                           "color: black;padding-left: 4px;border: 1px solid #6c6c6c;}");


    ui->tableView_unloading->verticalHeader()->setStyleSheet("QHeaderView::section {"
                                                         "color: black;padding-left: 4px;border: 1px solid #6c6c6c;}");
    ui->tableView_unloading->setColumnWidth(0, 40);
    ui->tableView_unloading->setColumnWidth(1, 160);
    ui->tableView_unloading->setColumnWidth(2, 55);
    ui->tableView_unloading->setColumnWidth(3, 55);
    ui->tableView_unloading->setColumnWidth(4, 55);
    ui->tableView_unloading->setColumnWidth(5, 50);
    ui->tableView_unloading->setColumnWidth(6, 60);
    ui->tableView_unloading->setColumnWidth(7, 55);
    ui->tableView_unloading->setColumnWidth(8, 50);
    ui->tableView_unloading->setSelectionBehavior(QAbstractItemView::SelectRows);  // 设置选中模式为选中行
    ui->tableView_unloading->setSelectionMode( QAbstractItemView::SingleSelection);  // 设置选中单个
    ui->tableView_unloading->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 设置不可编辑
}

void WasteRecycle::keyPressEvent(QKeyEvent *e)
{
    static bool bFlag = true;
    switch (e->key()) {
    case Qt::Key_Up:
        qDebug() << "key up";
        this->focusNextPrevChild(false);
        break;
    case Qt::Key_Down:
        qDebug() << "key down";
         this->focusNextPrevChild(true);
        break;
    case Qt::Key_Left:
        qDebug() << "key left";
        break;
     case Qt::Key_Right:
         qDebug() << "key right";
         break;
    case Qt::Key_Enter:
        qDebug() << "key enter";
        /*if(ui->btn_Level1->hasFocus()) {
            on_btn_Level1_clicked();
        } else if (ui->btn_Level2->hasFocus()) {
            on_btn_Level2_clicked();
        } else if (ui->btn_Level3->hasFocus()) {
            on_btn_Level3_clicked();
        } else if (ui->btn_Level4->hasFocus()) {
            on_btn_Level4_clicked();
        } else */
        if (ui->btn_Next->hasFocus()) {
            on_btn_Next_clicked();
        }
        break;
    case Qt::Key_Return:
        qDebug() << "key return";
       /* if(ui->btn_Level1->hasFocus()) {
            on_btn_Level1_clicked();
        } else if (ui->btn_Level2->hasFocus()) {
            on_btn_Level2_clicked();
        } else if (ui->btn_Level3->hasFocus()) {
            on_btn_Level3_clicked();
        } else if (ui->btn_Level4->hasFocus()) {
            on_btn_Level4_clicked();
        } else */
        if (ui->btn_Next->hasFocus()) {
            on_btn_Next_clicked();
        }
        break;
    case Qt::Key_Space:
        qDebug() << "key space";
        float data;
        data = ui->lb_display->text().toFloat();
        if(ui->le_RoughWeigh->text().trimmed() == "") {
            ui->le_RoughWeigh->setText(QString("%1").arg(data));
        } else if(ui->le_VehicleWeigh->text().trimmed() == "") {
            ui->le_VehicleWeigh->setText(QString("%1").arg(data));
        }
        break;
    case Qt::Key_F1:
        qDebug() << "key F1";
        // 当停止时，则开始识别
//        if (ui->tabWidget->currentIndex() == 2) {
//            on_btn_capture_clicked();
//        } else if (pause_) {
//            on_btn_startAnalyze_clicked();
//        }
        if(!bModify && bFlag) {
            bFlag = false;
            on_btn_rWrite_clicked();
            Sleep(10);
            bFlag = true;
        }
        break;
    default:
         qDebug() << "default";
         // this->setFocus();
         QWidget::keyPressEvent (e);
        break;
    }
}

void WasteRecycle::enterEvent(QEvent *e)
{
    QKeyEvent* theKey = static_cast<QKeyEvent*>(e);
    if (theKey->key() == Qt::Key_Enter || theKey->key() == Qt::Key_Return) {
        qDebug() << "enter event happen!";
    } else {
        QWidget::enterEvent(e);
    }
}

bool WasteRecycle::eventFilter(QObject *obj, QEvent *e)
{
    /*if (obj == ui->btn_Level1) {
        if (QEvent::Enter == e->type()) {
            QString ePrice = ui->le_Level1ExtraPrice->text();
            float level = ePrice != "" ? (fLevel1+ePrice.toInt()/100.0) : fLevel1;
            ui->btn_Level1->setText(QString("%1").arg(level));
        } else if (QEvent::Leave == e->type()) {
            ui->btn_Level1->setText(QString::fromLocal8Bit("黄板纸"));
        }
    } else if (obj == ui->btn_Level2) {
        if (QEvent::Enter == e->type()) {
            ui->btn_Level2->setText(ui->lb_percent->text());
        } else if (QEvent::Leave == e->type()) {
            ui->btn_Level2->setText(QString::fromLocal8Bit("超市纸"));
        }
    } else if (obj == ui->btn_Level3) {
        if (QEvent::Enter == e->type()) {
            ui->btn_Level3->setText(QString("%1").arg(fLevel3));
        } else if (QEvent::Leave == e->type()) {
            ui->btn_Level3->setText(QString::fromLocal8Bit("统货纸"));
        }
    } else if (obj == ui->btn_Level4) {
        if (QEvent::Enter == e->type()) {
            QString ePrice = ui->le_Level4ExtraPrice->text();
            float level = ePrice != "" ? (fLevel4+ePrice.toInt()/100.0) : fLevel4;
            ui->btn_Level4->setText(QString("%1").arg(level));
        } else if (QEvent::Leave == e->type()) {
            ui->btn_Level4->setText(QString::fromLocal8Bit("民用纸"));
        }
    } else */
    if (obj == ui->btn_finalCal) {
        if (QEvent::Enter == e->type()) {
            float huangban = ui->le_huangban->text().toFloat();
            float chaoshi = ui->le_chaoshi->text().toFloat();
            float tonghuo = ui->le_tonghuo->text().toFloat();

            float hbPrice = priceWin->paperPrices_[QString::fromLocal8Bit("黄板")] * (1-ui->spinBox->value()/100.0) - ui->doubleSpinBox->value(); // + ui->le_fineAdjustment->text().toFloat()/100.0;
            hbPrice = ((int)(hbPrice*100))/100.0;
            float csPrice = priceWin->paperPrices_[QString::fromLocal8Bit("超市")] * (1-ui->spinBox->value()/100.0) - ui->doubleSpinBox->value(); // + ui->le_fineAdjustment->text().toFloat()/100.0;
            csPrice = ((int)(csPrice*100))/100.0;
            float thPrice = priceWin->paperPrices_[QString::fromLocal8Bit("统货")] * (1-ui->spinBox->value()/100.0) - ui->doubleSpinBox->value(); // + ui->le_fineAdjustment->text().toFloat()/100.0;
            thPrice = ((int)(thPrice*100))/100.0;

            float fPrice = 0.0;

            if((huangban+chaoshi+tonghuo)>0) {
                fPrice = (hbPrice*huangban + csPrice*chaoshi + thPrice*tonghuo) / (huangban + chaoshi + tonghuo);  // + ui->le_fineAdjustment->text().toFloat()/100.0;
                fPrice = ((int)(fPrice*100))/100.0;
                fPrice += ui->le_fineAdjustment->text().toFloat()/100.0;
            }

            float profit = 2*(ui->le_RoughWeigh->text().toFloat() - ui->le_VehicleWeigh->text().toFloat())*(1-ui->spinBox->value()/100.0)*(ui->doubleSpinBox->value()-ui->le_fineAdjustment->text().toFloat()/100.0);
            ui->btn_finalCal->setText(QString("%1 | %2").arg(fPrice).arg((int)profit));
        } else if (QEvent::Leave == e->type()) {
            ui->btn_finalCal->setText(QString::fromLocal8Bit("结算"));
        }
    }
    if (obj == ui->btn_huangban) {
        if (QEvent::Enter == e->type()) {
            // float fPrice = priceWin->LineEditMap_[ui->btn_huangban->text()]->value_ + ui->le_fineAdjustment->text().toFloat()/100.0;
            float fPrice = priceWin->paperPrices_[QString::fromLocal8Bit("黄板")] * (1-ui->spinBox->value()/100.0) - ui->doubleSpinBox->value(); // + ui->le_fineAdjustment->text().toFloat()/100.0;
            fPrice = ((int)(fPrice*100))/100.0;
            fPrice += ui->le_fineAdjustment->text().toFloat()/100.0;
            ui->btn_huangban->setText(QString("%1").arg(fPrice));
        } else if (QEvent::Leave == e->type()) {
            ui->btn_huangban->setText(QString::fromLocal8Bit("黄板"));
        }
    } else if (obj == ui->btn_tonghuo) {
        if (QEvent::Enter == e->type()) {
            // float fPrice = priceWin->LineEditMap_[ui->btn_tonghuo->text()]->value_ + ui->le_fineAdjustment->text().toFloat()/100.0;
            // float fPrice = priceWin->paperPrices_[QString::fromLocal8Bit("统货")] + ui->le_fineAdjustment->text().toFloat()/100.0;
            float fPrice = priceWin->paperPrices_[QString::fromLocal8Bit("统货")] * (1-ui->spinBox->value()/100.0) - ui->doubleSpinBox->value();  // + ui->le_fineAdjustment->text().toFloat()/100.0;
            fPrice = ((int)(fPrice*100))/100.0;
            fPrice += ui->le_fineAdjustment->text().toFloat()/100.0;
            ui->btn_tonghuo->setText(QString("%1").arg(fPrice));
        } else if (QEvent::Leave == e->type()) {
            ui->btn_tonghuo->setText(QString::fromLocal8Bit("统货"));
        }
    } /*else if (obj == ui->btn_jumin) {
        if (QEvent::Enter == e->type()) {
            float fPrice = priceWin->LineEditMap_[ui->btn_jumin->text()]->value_ + ui->le_fineAdjustment->text().toFloat()/100.0;
            ui->btn_jumin->setText(QString("%1").arg(fPrice));
        } else if (QEvent::Leave == e->type()) {
            ui->btn_jumin->setText(QString::fromLocal8Bit("民用"));
        }
    } */else if (obj == ui->btn_chao1) {
        if (QEvent::Enter == e->type()) {
            // float fPrice = priceWin->LineEditMap_[QString::fromLocal8Bit("超1")]->value_ + ui->le_fineAdjustment->text().toFloat()/100.0;
            // float fPrice = priceWin->paperPrices_[QString::fromLocal8Bit("超市")] + ui->le_fineAdjustment->text().toFloat()/100.0;
            float fPrice = priceWin->paperPrices_[QString::fromLocal8Bit("超市")] * (1-ui->spinBox->value()/100.0) - ui->doubleSpinBox->value();  // + ui->le_fineAdjustment->text().toFloat()/100.0;
            fPrice = ((int)(fPrice*100))/100.0;
            fPrice += ui->le_fineAdjustment->text().toFloat()/100.0;
            ui->btn_chao1->setText(QString("%1").arg(fPrice));
        } else if (QEvent::Leave == e->type()) {
            ui->btn_chao1->setText(QString::fromLocal8Bit("超市"));
        }
    } /*else if (obj == ui->btn_chao2) {
        if (QEvent::Enter == e->type()) {
            float fPrice = priceWin->LineEditMap_[ui->btn_chao2->text()]->value_ + ui->le_fineAdjustment->text().toFloat()/100.0;
            ui->btn_chao2->setText(QString("%1").arg(fPrice));
        } else if (QEvent::Leave == e->type()) {
            ui->btn_chao2->setText(QString::fromLocal8Bit("超2"));
        }
    } else if (obj == ui->btn_chao3) {
        if (QEvent::Enter == e->type()) {
            float fPrice = priceWin->LineEditMap_[ui->btn_chao3->text()]->value_ + ui->le_fineAdjustment->text().toFloat()/100.0;
            ui->btn_chao3->setText(QString("%1").arg(fPrice));
        } else if (QEvent::Leave == e->type()) {
            ui->btn_chao3->setText(QString::fromLocal8Bit("超3"));
        }
    } */else if (obj == ui->btn_shuben) {
        if (QEvent::Enter == e->type()) {
            float fPrice = priceWin->LineEditMap_[ui->btn_shuben->text()]->value_ + ui->le_fineAdjustment->text().toFloat()/100.0;
            ui->btn_shuben->setText(QString("%1").arg(fPrice));
        } else if (QEvent::Leave == e->type()) {
            ui->btn_shuben->setText(QString::fromLocal8Bit("书本"));
        }
    } else if (obj == ui->btn_baozhi) {
        if (QEvent::Enter == e->type()) {
            float fPrice = priceWin->LineEditMap_[ui->btn_baozhi->text()]->value_ + ui->le_fineAdjustment->text().toFloat()/100.0;
            ui->btn_baozhi->setText(QString("%1").arg(fPrice));
        } else if (QEvent::Leave == e->type()) {
            ui->btn_baozhi->setText(QString::fromLocal8Bit("报纸"));
        }
    } else if (obj == ui->btn_tie) {
        if (QEvent::Enter == e->type()) {
            float fPrice = priceWin->LineEditMap_[ui->btn_tie->text()]->value_ + ui->le_fineAdjustment->text().toFloat()/20.0;
            ui->btn_tie->setText(QString("%1").arg(fPrice));
        } else if (QEvent::Leave == e->type()) {
            ui->btn_tie->setText(QString::fromLocal8Bit("铁"));
        }
    } else if (obj == ui->btn_buxiugang) {
        if (QEvent::Enter == e->type()) {
            float fPrice = priceWin->LineEditMap_[ui->btn_buxiugang->text()]->value_ + ui->le_fineAdjustment->text().toFloat()/10.0;
            ui->btn_buxiugang->setText(QString("%1").arg(fPrice));
        } else if (QEvent::Leave == e->type()) {
            ui->btn_buxiugang->setText(QString::fromLocal8Bit("不锈钢"));
        }
    } else if (obj == ui->btn_lv) {
        if (QEvent::Enter == e->type()) {
            float fPrice = priceWin->LineEditMap_[ui->btn_lv->text()]->value_ + ui->le_fineAdjustment->text().toFloat()/10.0;
            ui->btn_lv->setText(QString("%1").arg(fPrice));
        } else if (QEvent::Leave == e->type()) {
            ui->btn_lv->setText(QString::fromLocal8Bit("铝"));
        }
    } else if (obj == ui->btn_tong) {
        if (QEvent::Enter == e->type()) {
            float fPrice = priceWin->LineEditMap_[ui->btn_tong->text()]->value_ + ui->le_fineAdjustment->text().toFloat()/10.0;
            ui->btn_tong->setText(QString("%1").arg(fPrice));
        } else if (QEvent::Leave == e->type()) {
            ui->btn_tong->setText(QString::fromLocal8Bit("铜"));
        }
    } else if (obj == ui->btn_rClear) {
        if (QEvent::Enter == e->type()) {
            ui->btn_rClear->setText(QString::fromLocal8Bit("清除"));
        } else if (QEvent::Leave == e->type()) {
            ui->btn_rClear->setText(QString::fromLocal8Bit("公斤"));
        }
    } else if (obj == ui->btn_vClear) {
        if (QEvent::Enter == e->type()) {
            ui->btn_vClear->setText(QString::fromLocal8Bit("清除"));
        } else if (QEvent::Leave == e->type()) {
            ui->btn_vClear->setText(QString::fromLocal8Bit("公斤"));
        }
    } else if (obj == ui->lb_rwScaledImage && !ui->le_RoughWeigh->text().isEmpty()) {
        static QLabel* lb = new QLabel();
        QPoint curPos = QCursor::pos();
        lb->move(curPos.x()+ui->lb_rwScaledImage->width(), 0);
        if (QEvent::Enter == e->type() && ui->lb_rwScaledImage->pixmap()) {
            // QDateTime date = QDateTime::currentDateTime();
            QString date = ui->dateEdit->text().replace("-", "_");
            // QString today = date.toString("yyyy_MM_dd");
            QString dirName = date + "_rwPicture";

            QString index = ui->lb_CurrNum->text();
            QString rw = ui->le_RoughWeigh->text();

            QString filename = QDir::currentPath() +  "/" + dirName + "/" + index + "_" + rw + ".jpeg";
            std::cout << "filename1:" << filename.toStdString() << std::endl;
            QImage image(filename);
            lb->setPixmap(QPixmap::fromImage(image).scaled(ui->w_playWindow->width(), ui->w_playWindow->height()));
            lb->setWindowFilePath(filename);
            lb->show();
        } else if (QEvent::Leave == e->type()) {
            lb->close();
        }
    } else if (obj == ui->lb_vwScaledImage && !ui->le_VehicleWeigh->text().isEmpty()) {
        static QLabel* lb = new QLabel();
        QPoint curPos = QCursor::pos();
        lb->move(curPos.x()+ui->lb_vwScaledImage->width(), 0);
        if (QEvent::Enter == e->type() && ui->lb_vwScaledImage->pixmap()) {
            // QDateTime date = QDateTime::currentDateTime();
            // QString today = date.toString("yyyy_MM_dd");
            QString date = ui->dateEdit->text().replace("-", "_");
            QString dirName = date + "_vwPicture";
            QString index = ui->lb_CurrNum->text();
            QString vw = ui->le_VehicleWeigh->text();
            QString filename = QDir::currentPath() +  "/" + dirName + "/" + index + "_" + vw + ".jpeg";
            std::cout << "filename2:" << filename.toStdString() << std::endl;
            QImage image(filename);
            lb->setPixmap(QPixmap::fromImage(image).scaled(ui->w_playWindow->width(), ui->w_playWindow->height()));
            lb->setWindowFilePath(filename);
            lb->show();
        } else if (QEvent::Leave == e->type()) {
            lb->close();
        }
    } else if (obj == ui->lb_photo) {
        static QLabel* lb = new QLabel();
        QPoint curPos = QCursor::pos();
        lb->move(curPos.x()+ui->lb_photo->width(), 0);
        if (QEvent::Enter == e->type() && ui->lb_photo->pixmap()) {
            QString filename = QDir::currentPath() +  "/members/" + ui->le_name->text() + ".jpeg";
            std::cout << "filename3:" << filename.toStdString() << std::endl;
            QImage image(filename);
            int param = 1;
            while((image.height() / param) > ui->w_playWindow->height()) {
                ++param;
            }
            // lb->setParent(ui->lb_photo);
            lb->setPixmap(QPixmap::fromImage(image).scaled(image.width()/param, image.height()/param));
            lb->setWindowFilePath(filename);
            lb->show();
        } else if (QEvent::Leave == e->type()) {
            lb->close();
        }
    }

    return false;
}

void WasteRecycle::showPrice(float level)
{
    if (ui->lb_Price->text().trimmed() == "") {
        qDebug() << "go unloading";
        nextVehicle();
        return;
    }
    QString msg = "<font size='25' color='black'>" + ui->lb_Price->text() + QString::fromLocal8Bit(" 元") + "</font>";
    int ret = QMessageBox::information(this,
                                                              QString::fromLocal8Bit("价格"),
                                                              msg,
                                                              QString::fromLocal8Bit("确定"),
                                                              QString::fromLocal8Bit("取消"));
    switch (ret) {
    case 0:
        if (bModify) {
            modifyData();
        } else {
            QString dat = ui->dateEdit->text();
            QDateTime date = QDateTime::currentDateTime();
            QString today = date.toString("yyyy-MM-dd");
            if (dat != today) {
                ui->dateEdit->setDate(QDate::fromString(today, "yyyy-MM-dd"));
                on_btn_search_clicked();
            }
        }
        storeData(level);
        nextVehicle();
        // 当单子已结算，则当做磅秤已回到过0，可更换当前识别用户
        hasReturnZero = true;
        break;
    case 1:
        setTextEnabled(true);
        break;
    default:
        break;
    }
}

void WasteRecycle::clearData()
{
//    ui->le_Level1ExtraPrice->clear();
//    ui->le_Level4ExtraPrice->clear();
    ui->le_VehicleWeigh->clear();
    ui->le_RoughWeigh->clear();

    ui->lb_Price->clear();

    ui->lb_unitPrice->clear();
    ui->lb_rwScaledImage->clear();
    ui->lb_vwScaledImage->clear();
    ui->lb_photo->clear();
    ui->le_name->clear();
    ui->le_phone->clear();
    ui->lb_similarity->clear();
}

void WasteRecycle::nextVehicle()
{
    // 当未填毛重时，忽略该次点击操作
    /*if("" == ui->le_RoughWeigh->text()) {
        qWarning() << "le_RoughWeigh is empty";
        return;
    }*/
    // 当前索引值递增, 并显示下一辆车的索引值
    if (ui->lb_CurrNum->text().toInt() == toBeUseIndex) {
        ++toBeUseIndex;
    }
    clearData();
    ui->lb_CurrNum->setText(QString::fromStdString(std::to_string(toBeUseIndex)));
    setTextEnabled(true);
    ui->le_RoughWeigh->setFocus();

    model->sort(0);
    model_unloading->sort(0);
//    float ps = (fLevel2-fLevel3)/(fLevel1-fLevel3)  * 100;
//    int pos = ps;
    // bPriceInit = true;
    ui->vslider_percent->setValue(0);
    ui->le_fineAdjustment->setText(QString("%1").arg(0));
    ui->le_chaoshi->clear();
    ui->le_huangban->clear();
    ui->le_tonghuo->clear();
    ui->doubleSpinBox->setValue(0);
    ui->spinBox->setValue(0);
    ui->lb_NetWeight->clear();
//    if(!stop_) {
//        pause_ = false;
//    }
    qDebug() << "next vehicle";
}

void WasteRecycle::deleteData()
{
    qDebug() << "table name:" << rightMenu->objectName();
    if (rightMenu->objectName() == "tableView_unloading") {
        bModifyUnloading =  true;
    } else if (rightMenu->objectName() == "tableView") {
        bModify = true;
    }
    qDebug() << "deleteData IN";
    QString date = ui->dateEdit->text().replace("-", "_");
    int row = ui->tableView->currentIndex().row();
    int urow = ui->tableView_unloading->currentIndex().row();

    if (bModifyUnloading == false && model_unloading->index(urow, 0).data().toString() == ui->lb_CurrNum->text()) {
        oper->sqlDeleteById(model_unloading->index(urow, 0).data().toString(), date);
        model_unloading->removeRow(urow);
    } else if (bModifyUnloading == true) {
        oper->sqlDeleteById(model_unloading->index(urow, 0).data().toString(), date);
        model_unloading->removeRow(urow);
        bModifyUnloading = false;
        return;
    }

    if (bModify == false && model->index(row, 0).data().toString() == ui->lb_CurrNum->text()) {
        oper->sqlDeleteById(model->index(row, 0).data().toString(), date);
        model->removeRow(row);
    } else if (bModify == true) {
        oper->sqlDeleteById(model->index(row, 0).data().toString(), date);
        model->removeRow(row);
        bModify = false;
    }

    qDebug() << "deleteData IN";
}

void WasteRecycle::updateTableCharts()
{
    // 1. 获取所有record表
    QStringList tables = oper->sqlGetAllTableName();
    if (!tables.contains("charts")) {
        QString createChartsTable = "create table charts (time varchar(100), amouts varchar(10), totalWeight varchar(10), totalPrice varchar(10), averagePrice varchar(10));";
        oper->createTable(createChartsTable);
    } else {
        qDebug() << "charts is exist!";
    }

    // 2. 遍历所有表的日期是否在charts表中都已登记，没有则进行登记
    QStringList records = tables.filter("record_");

    for (auto e : records) {
        QString date = e;
        date = date.remove("record_");
        QDateTime datetime = QDateTime::currentDateTime();
        QString today = datetime.toString("yyyy_MM_dd");
        if(date == today) {
            continue;
        }

        // a. 对每一天的数据进行统计
        if (!oper->searchTableWetherExist("charts", "time", date)) {
            qDebug() << "Insert date:" << date;
            std::list<QString> records;
            records = oper->sqlQueryByDate(date);

            float totalWeight = 0;
            float totalPrice = 0;
            float averagePrice = 0;
            int amount = 0;

            for(auto record : records) {
                QStringList items = record.split("  ");
                if (items.at(8) != QString::fromLocal8Bit("纸") && items.at(8) != QString::fromLocal8Bit("板纸")) {
                    continue;
                }
                totalWeight += items.at(4).toFloat();
                totalPrice += items.at(6).toFloat();
                ++amount;
                averagePrice = (totalWeight==0 ? 0 :  (totalPrice / totalWeight) * 2000);
            }

            qDebug() << "tableName:" << e << " totalWeight:" <<totalWeight << " totalPrice:" << totalPrice << " amount:" << amount << " averagePrice:" << averagePrice;
            QString insertChartsTable =  "insert into charts values('" + date + "'" + ", '" + QString("%1").arg(amount) + "'"
                    + ", '" +  QString("%1").arg(totalWeight) + "'" + ", '" + QString("%1").arg(totalPrice) + "'"+ ", '" +  QString("%1").arg(averagePrice) + "');";
            oper->insertTable(insertChartsTable);
        } else {
            qDebug() << "date:" << date << " is Exist in table charts";
        }
    }
}

void WasteRecycle::initActionsConnections()
{
    connect(m_serial, &QSerialPort::readyRead, this, &WasteRecycle::readWeighBridgeData);
    this->openSerialPort();
    this->readWeighBridgeData();
}

void WasteRecycle::readWeighBridgeData()
{
    QByteArray data = m_serial->readAll();
    this->putWeighBridgeData(data);
}

void WasteRecycle::putWeighBridgeData(QByteArray &wbd)
{
    weighBridgeData.append(wbd);
    if(weighBridgeData.contains("\n")) {
        weighBridgeData.clear();
    }

    if(weighBridgeData.length()>12) {
        QString qqba = QString(weighBridgeData);
        if (weighBridgeData.contains('cl') ) {
            weighBridgeData.clear();
            qqba = qqba.mid(qqba.indexOf("l")+1);
        }
        qqba = qqba.mid(qqba.indexOf("n")+1, 8);
        if(fWeight != qqba.toFloat()) {
            m_pTimer->stop();
            fWeight = qqba.toFloat();
            if (fWeight < 1) {
                hasReturnZero = true;
            }
            if ((fWeight - theTimingWeigh > 0.5|| theTimingWeigh - fWeight > 0.5)) {
                weighTimer->stop();
                theTimingWeigh = fWeight;
                // 开始计时3秒，3秒到，则把该稳定重量存入可用重量的map中
                weighTimer->start(1000*3);
                qDebug() << ">>>>>>> [putWeighBridgeData] weighTimer start timing!";
            }
            ui->lb_display->setText(QString("%1").arg(fWeight));

            // 设置2分钟超时
            m_pTimer->start(1000*60*2);
        }
        // weighBridgeData.clear();
    }
}

float WasteRecycle::priceCalculate(float level)
{
    qDebug() << "priceCalculate IN&&OUT";
    QString strRw = ui->le_RoughWeigh->text();
    QString strVw = ui->le_VehicleWeigh->text();

    float fRw = strRw.toFloat();
    float fVw = strVw.toFloat();
    return (fRw-fVw)*level*2;
}

void WasteRecycle::updatePrice()
{
//    fLevel1 = priceSetWin->getAPrice();
//    fLevel2 = priceSetWin->getBPrice();
//    fLevel3 = priceSetWin->getCPrice();
//    fLevel4 = priceSetWin->getDPrice();
}

void WasteRecycle::finalPrice(float level)
{
    qDebug() << "finalPrice IN";
    ui->lb_Price->clear();
    if (ui->le_VehicleWeigh->text().trimmed() == "") {
        qDebug() << "level: " << level << " VehicleWeigh is empty.";
        return;
    }
    ui->lb_Price->setText(QString("%1").arg(priceCalculate(level)));
    /*QString qstrCurrIndex = ui->lb_CurrNum->text();
    int iCurrIndex = qstrCurrIndex.toInt();

    // 如果存在于正在卸货的map中，则删除map中的以及combobox下拉列表中的对应项
    for(auto e: unloadingClientMap) {
        if (iCurrIndex == e.first){
            qDebug() << "erase num:" << iCurrIndex;
            unloadingClientMap.erase(iCurrIndex);
            int ret = ui->cbb_NumSwitch->findText(qstrCurrIndex);
            if (-1 != ret) {
                qDebug() << "be found, index:" << ret;
                ui->cbb_NumSwitch->removeItem(ret);
            }
            break;
        }
    }*/ //========2018-10-27======
    qDebug() << "finalPrice OUT";
}

// 写数据库（排队号码, 时间， 毛重， 车重， 单价， 总价）
void WasteRecycle::writeData(float level)
{
    qDebug() << "writeData IN";
    RecordInfo info;
    info.m_index = ui->lb_CurrNum->text();
    info.m_rWeight = ui->le_RoughWeigh->text();
    info.m_vWeight = ui->le_VehicleWeigh->text().trimmed() == "" ? QString::fromLocal8Bit("卸货中") : ui->le_VehicleWeigh->text();
    info.m_nWeight = ui->lb_NetWeight->text();
    info.m_price = ui->lb_Price->text();
    info.m_unitPrice =  towDecimalPlaces(QString("%1").arg(level));
    info.m_kind = kind_;
    // info.m_name = ui->le_name->text();
    float profit = 2*(ui->le_RoughWeigh->text().toFloat() - ui->le_VehicleWeigh->text().toFloat())*(1-ui->spinBox->value()/100.0)*(ui->doubleSpinBox->value()-ui->le_fineAdjustment->text().toFloat()/100.0);
    info.m_name = QString("%1").arg((int)profit);

    QDateTime qdtTime =QDateTime::currentDateTime();
    info.m_time = qdtTime.toString("yyyy-MM-dd hh:mm:ss");

    QString qstrDate = ui->dateEdit->text().replace("-", "_");
    QDateTime date = QDateTime::fromString(qstrDate, "yyyy_MM_dd");
    int interval = date.daysTo(qdtTime);
    if (interval > 0) {
        qDebug() << "date is error, the date being writing is" << qstrDate;
        return;
    }
    oper->sqlInsert(info, qstrDate);
    qDebug() << "writeData OUT";
}

/* void WasteRecycle::updateListWidget(float level)
{
    // int num = ui->lw_history->count();

//    QDateTime qtime = QDateTime::currentDateTime();
//    QString time = qtime.toString("yyyy-MM-dd hh:mm:ss");
//    QString record = ui->lb_CurrNum->text() + UtilityTools::holdPlaces(5) + time + UtilityTools::holdPlaces(5) +
//            ui->le_RoughWeigh->text() + UtilityTools::holdPlaces(15) + ui->le_VehicleWeigh->text() + UtilityTools::holdPlaces(15) +
//            ui->lb_NetWeight->text() + UtilityTools::holdPlaces(15) + towDecimalPlaces(QString("%1").arg(level)) + UtilityTools::holdPlaces(15) +
//            ui->lb_Price->text();
//    ui->lw_history->addItem(record);
} */

void WasteRecycle::updateUnloadingTableView(float level)
{
    qDebug() << "updateUnloadingTableView IN";
    QDateTime qtime = QDateTime::currentDateTime();
    QString time = qtime.toString("yyyy-MM-dd hh:mm:ss");
    int row = model_unloading->rowCount();
    model_unloading->setItem(row, 0, new QStandardItem(ui->lb_CurrNum->text()));
    model_unloading->setItem(row, 1, new QStandardItem(time));
    model_unloading->setItem(row, 2, new QStandardItem(ui->le_RoughWeigh->text()));
    model_unloading->setItem(row, 3, new QStandardItem(ui->le_VehicleWeigh->text().trimmed() == "" ? QString::fromLocal8Bit("卸货中") : ui->le_VehicleWeigh->text()));
    model_unloading->setItem(row, 4, new QStandardItem(ui->lb_NetWeight->text()));
    model_unloading->setItem(row, 5, new QStandardItem(towDecimalPlaces(QString("%1").arg(level))));
    model_unloading->setItem(row, 6, new QStandardItem(ui->lb_Price->text()));
    model_unloading->setItem(row, 7, new QStandardItem(ui->le_name->text()));
    model_unloading->setItem(row, 8, new QStandardItem(kind_));
    ui->tableView_unloading->selectRow(row);
    qDebug() << "updateUnloadingTableView OUT";
}

void WasteRecycle::updateTableView(float level)
{
    qDebug() << "updateTableView IN";
    QDateTime qtime = QDateTime::currentDateTime();
    QString time = qtime.toString("yyyy-MM-dd hh:mm:ss");
    int row = model->rowCount();
    model->setItem(row, 0, new QStandardItem(ui->lb_CurrNum->text()));
    model->setItem(row, 1, new QStandardItem(time));
    model->setItem(row, 2, new QStandardItem(ui->le_RoughWeigh->text()));
    model->setItem(row, 3, new QStandardItem(ui->le_VehicleWeigh->text().trimmed() == "" ? QString::fromLocal8Bit("卸货中") : ui->le_VehicleWeigh->text()));
    model->setItem(row, 4, new QStandardItem(ui->lb_NetWeight->text()));
    model->setItem(row, 5, new QStandardItem(towDecimalPlaces(QString("%1").arg(level))));
    model->setItem(row, 6, new QStandardItem(ui->lb_Price->text()));
    float profit = 0;
    if(kind_==QString::fromLocal8Bit("板纸")) {
        profit = 2*(ui->le_RoughWeigh->text().toFloat() - ui->le_VehicleWeigh->text().toFloat())*(1-ui->spinBox->value()/100.0)*(ui->doubleSpinBox->value()-ui->le_fineAdjustment->text().toFloat()/100.0);
    }
    model->setItem(row, 7, new QStandardItem(QString("%1").arg((int)profit)));
    model->setItem(row, 8, new QStandardItem(kind_));
    ui->tableView->selectRow(row);
    qDebug() << "updateTableView OUT";
}

bool WasteRecycle::check()
{
    if (ui->le_RoughWeigh->text() == ""/* || ui->le_VehicleWeigh->text() == ""*/) {
        qWarning() << "le_RoughWeigh or le_VehicleWeigh is empty.";
        QMessageBox::warning(this, QString::fromLocal8Bit("提醒"), QString::fromLocal8Bit("毛重或者车重未填写!"), QString::fromLocal8Bit("关闭"));
        return false;
    }
    if (ui->le_RoughWeigh->text().toFloat() <= ui->le_VehicleWeigh->text().toFloat()) {
        qWarning() << "le_RoughWeigh or le_VehicleWeigh data is wrong.";
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("毛重或者车重数据错误!"), QString::fromLocal8Bit("关闭"));
        return false;
    }
    return true;
}

void WasteRecycle::setTextEnabled(bool bValue)
{
    ui->le_RoughWeigh->setEnabled(bValue);
    ui->le_VehicleWeigh->setEnabled(bValue);
//    ui->le_Level1ExtraPrice->setEnabled(bValue);
//    ui->le_Level4ExtraPrice->setEnabled(bValue);
}

// 更新数据统计中的数据栏
void WasteRecycle::updateStatistics()
{
//    int amount = ui->lw_history->count();
//    ui->lb_Amount->setText(QString("%1").arg(amount));
//    float totalWeight = 0;
//    float totalPrice = 0;
//    float averagePrice = 0;

//    for (int i = 0; i < amount; ++i) {
//        QString record = ui->lw_history->item(i)->text();
//        totalWeight += QString(record.split(" ", QString::SkipEmptyParts).at(5)).toFloat();
//        totalPrice += QString(record.split(" ", QString::SkipEmptyParts).at(7)).toFloat();
//    }

//    averagePrice = (totalWeight==0 ? 0 :  (totalPrice / totalWeight) * 2000);

//    ui->lb_TotalWeight->setText(QString("%1").arg(totalWeight));
//    ui->lb_TotalPrices->setText(QString("%1").arg(totalPrice));
//    ui->lb_AveragePrice->setText(QString("%1").arg(averagePrice));
}

void WasteRecycle::statistics()
{
    qDebug() << "statistics IN";
    float totalWeight = 0;
    float totalPrice = 0;
    float averagePrice = 0;
    float totalProfit = 0;
    int amount = 0;
    int rows = model->rowCount();
    for (int i =  0; i < rows ; ++i) {
        if (model->index(i, 8).data().toString() != QString::fromLocal8Bit("纸") &&
                model->index(i, 8).data().toString() != QString::fromLocal8Bit("板纸")) {
            continue;
        }
        totalWeight += model->index(i, 4).data().toFloat();
        totalPrice += model->index(i, 6).data().toFloat();
        totalProfit += model->index(i, 7).data().toFloat();
        ++amount;
    }

    averagePrice = (totalWeight==0 ? 0 :  (totalPrice / totalWeight) * 2000);
    ui->lb_Amount->setText(QString("%1").arg(amount));
    ui->lb_TotalWeight->setText(QString("%1").arg(totalWeight));
    ui->lb_TotalPrices->setText(QString("%1").arg(totalPrice));
    ui->lb_AveragePrice->setText(QString("%1").arg(averagePrice));
    ui->lb_TotalProfit->setText(QString("%1").arg(totalProfit));
    qDebug() << "statistics OUT";
}

void WasteRecycle::storeData(float level) {
    qDebug() << "storeData IN";
    if(ui->lb_Price->text().trimmed() == "") {
        updateUnloadingTableView(level);
    } else {
        if (bModifyUnloading) {
            QString date = ui->dateEdit->text().replace("-", "_");
            int urow = ui->tableView_unloading->currentIndex().row();
            oper->sqlDeleteById(model_unloading->index(urow, 0).data().toString(), date);
            model_unloading->removeRow(urow);
            bModifyUnloading = false;
        }
        updateTableView(level);
    }

    writeData(level);
    kind_ = QString::fromLocal8Bit("未分类");
    oper->sqlDeleteUnloadingByIdx(ui->lb_CurrNum->text());
    qDebug() << "storeData OUT";
}

// 检查车重数据是否合法
bool WasteRecycle::checkVehicleWeighIsValid() {
    QString vw = ui->le_VehicleWeigh->text().trimmed();
    bool isAllDigit = true;
    for (auto e : vw.toStdString()) {
        if(!isdigit(e) && e != '.') {
            isAllDigit = false;
            break;
        }
    }

    // 车重数据为空或者数据不全为数字时
    if(vw == "" || !isAllDigit) {
        // 如果是从表格恢复的数据则不做任何修改
        /*if (bModify || bModifyUnloading) {
            return;
        }*/
        qDebug() << "vehicle weigh is not alldigital or is empty";
        return false;
    }
    return true;
}

void WasteRecycle::on_btn_huangban_clicked()
{
    qDebug() << ">>>>>>> on_btn_huangban_clicked IN";
    // kind_ = QString::fromLocal8Bit("板纸");
    // 判断车重是否全为数字
    if (!checkVehicleWeighIsValid()) {
        on_btn_Next_clicked();
        return;
    }

    float rw = ui->le_RoughWeigh->text().toFloat();
    float vw = ui->le_VehicleWeigh->text().toFloat();
    float weight = (rw-vw)*2;
    if(weight > 0) {
        if(!ui->le_chaoshi->text().isEmpty()) {
            weight -= ui->le_chaoshi->text().toFloat();
        }
        if(!ui->le_tonghuo->text().isEmpty()) {
            weight -= ui->le_tonghuo->text().toFloat();
        }

        ui->le_huangban->setText(QString("%1").arg(weight));
    }

    // 最终价格为黄板纸价格+微调价格
//    float fPrice = priceWin->LineEditMap_[QString::fromLocal8Bit("黄板")]->value_ + ui->le_fineAdjustment->text().toFloat()/100.0;

//    finalPrice(fPrice);
//    ui->lb_unitPrice->setText(QString("%1").arg(fPrice));
//    setTextEnabled(false);
//    showPrice(fPrice);
}

void WasteRecycle::on_btn_tonghuo_clicked()
{
    qDebug() << ">>>>>>> on_btn_tonghuo_clicked IN";
    // kind_ = QString::fromLocal8Bit("板纸");
    // 判断车重是否全为数字
    if (!checkVehicleWeighIsValid()) {
        on_btn_Next_clicked();
        return;
    }

    float rw = ui->le_RoughWeigh->text().toFloat();
    float vw = ui->le_VehicleWeigh->text().toFloat();
    float weight = 2*(rw-vw);
    if(weight > 0) {
        if(!ui->le_huangban->text().isEmpty()) {
            weight -= ui->le_huangban->text().toFloat();
        }
        if(!ui->le_chaoshi->text().isEmpty()) {
            weight -= ui->le_chaoshi->text().toFloat();
        }

        ui->le_tonghuo->setText(QString("%1").arg(weight));
    }

    // 最终价格为黄板纸价格+微调价格
//    float fPrice = priceWin->LineEditMap_[QString::fromLocal8Bit("统货")]->value_ + ui->le_fineAdjustment->text().toFloat()/100.0;

//    finalPrice(fPrice);
//    ui->lb_unitPrice->setText(QString("%1").arg(fPrice));
//    setTextEnabled(false);
//    showPrice(fPrice);
}

void WasteRecycle::on_btn_jumin_clicked()
{
    qDebug() << ">>>>>>> on_btn_jumin_clicked IN";
    kind_ = QString::fromLocal8Bit("板纸");
    // 判断车重是否全为数字
    if (!checkVehicleWeighIsValid()) {
        on_btn_Next_clicked();
        return;
    }

    // 最终价格为黄板纸价格+微调价格
    float fPrice = priceWin->LineEditMap_[QString::fromLocal8Bit("民用")]->value_ + ui->le_fineAdjustment->text().toFloat()/100.0;

    finalPrice(fPrice);
    ui->lb_unitPrice->setText(QString("%1").arg(fPrice));
    setTextEnabled(false);
    showPrice(fPrice);
}

void WasteRecycle::on_btn_chao1_clicked()
{
    qDebug() << ">>>>>>> on_btn_chao1_clicked IN";
    // kind_ = QString::fromLocal8Bit("板纸");
    // 判断车重是否全为数字
    if (!checkVehicleWeighIsValid()) {
        on_btn_Next_clicked();
        return;
    }

    float rw = ui->le_RoughWeigh->text().toFloat();
    float vw = ui->le_VehicleWeigh->text().toFloat();
    float weight = 2*(rw-vw);
    if(weight > 0) {
        if(!ui->le_huangban->text().isEmpty()) {
            weight -= ui->le_huangban->text().toFloat();
        }
        if(!ui->le_tonghuo->text().isEmpty()) {
            weight -= ui->le_tonghuo->text().toFloat();
        }

        ui->le_chaoshi->setText(QString("%1").arg(weight));
    }

    // 最终价格为黄板纸价格+微调价格
//    float fPrice = priceWin->LineEditMap_[QString::fromLocal8Bit("超1")]->value_ + ui->le_fineAdjustment->text().toFloat()/100.0;

//    finalPrice(fPrice);
//    ui->lb_unitPrice->setText(QString("%1").arg(fPrice));
//    setTextEnabled(false);
//    showPrice(fPrice);
}

void WasteRecycle::on_btn_chao2_clicked()
{
    qDebug() << ">>>>>>> on_btn_chao2_clicked IN";
    kind_ = QString::fromLocal8Bit("板纸");
    // 判断车重是否全为数字
    if (!checkVehicleWeighIsValid()) {
        on_btn_Next_clicked();
        return;
    }

    // 最终价格为黄板纸价格+微调价格
    float fPrice = priceWin->LineEditMap_[QString::fromLocal8Bit("超2")]->value_ + ui->le_fineAdjustment->text().toFloat()/100.0;

    finalPrice(fPrice);
    ui->lb_unitPrice->setText(QString("%1").arg(fPrice));
    setTextEnabled(false);
    showPrice(fPrice);
}

void WasteRecycle::on_btn_chao3_clicked()
{
    qDebug() << ">>>>>>> on_btn_chao3_clicked IN";
    kind_ = QString::fromLocal8Bit("板纸");
    // 判断车重是否全为数字
    if (!checkVehicleWeighIsValid()) {
        on_btn_Next_clicked();
        return;
    }

    // 最终价格为黄板纸价格+微调价格
    float fPrice = priceWin->LineEditMap_[QString::fromLocal8Bit("超3")]->value_ + ui->le_fineAdjustment->text().toFloat()/100.0;

    finalPrice(fPrice);
    ui->lb_unitPrice->setText(QString("%1").arg(fPrice));
    setTextEnabled(false);
    showPrice(fPrice);
}

void WasteRecycle::on_btn_shuben_clicked()
{
    qDebug() << ">>>>>>> on_btn_shuben_clicked IN";
    kind_ = QString::fromLocal8Bit("书本");
    // 判断车重是否全为数字
    if (!checkVehicleWeighIsValid()) {
        on_btn_Next_clicked();
        return;
    }

    // 最终价格为黄板纸价格+微调价格
    float fPrice = priceWin->LineEditMap_[QString::fromLocal8Bit("书本")]->value_ + ui->le_fineAdjustment->text().toFloat()/100.0;

    finalPrice(fPrice);
    ui->lb_unitPrice->setText(QString("%1").arg(fPrice));
    setTextEnabled(false);
    showPrice(fPrice);
}

void WasteRecycle::on_btn_baozhi_clicked()
{
    qDebug() << ">>>>>>> on_btn_baozhi_clicked IN";
    kind_ = QString::fromLocal8Bit("报纸");
    // 判断车重是否全为数字
    if (!checkVehicleWeighIsValid()) {
        on_btn_Next_clicked();
        return;
    }

    // 最终价格为黄板纸价格+微调价格
    float fPrice = priceWin->LineEditMap_[QString::fromLocal8Bit("报纸")]->value_ + ui->le_fineAdjustment->text().toFloat()/100.0;

    finalPrice(fPrice);
    ui->lb_unitPrice->setText(QString("%1").arg(fPrice));
    setTextEnabled(false);
    showPrice(fPrice);
}

void WasteRecycle::on_btn_tie_clicked()
{
    qDebug() << ">>>>>>> on_btn_tie_clicked IN";
    kind_ = QString::fromLocal8Bit("铁");
    // 判断车重是否全为数字
    if (!checkVehicleWeighIsValid()) {
        on_btn_Next_clicked();
        return;
    }

    // 最终价格为黄板纸价格+微调价格
    float fPrice = priceWin->LineEditMap_[QString::fromLocal8Bit("铁")]->value_ + ui->le_fineAdjustment->text().toFloat()/20.0;

    finalPrice(fPrice);
    ui->lb_unitPrice->setText(QString("%1").arg(fPrice));
    setTextEnabled(false);
    showPrice(fPrice);
}

void WasteRecycle::on_btn_buxiugang_clicked()
{
    qDebug() << ">>>>>>> on_btn_buxiugang_clicked IN";
    kind_ = QString::fromLocal8Bit("不锈钢");
    // 判断车重是否全为数字
    if (!checkVehicleWeighIsValid()) {
        on_btn_Next_clicked();
        return;
    }

    // 最终价格为黄板纸价格+微调价格
    float fPrice = priceWin->LineEditMap_[QString::fromLocal8Bit("不锈钢")]->value_ + ui->le_fineAdjustment->text().toFloat()/10.0;

    finalPrice(fPrice);
    ui->lb_unitPrice->setText(QString("%1").arg(fPrice));
    setTextEnabled(false);
    showPrice(fPrice);
}

void WasteRecycle::on_btn_lv_clicked()
{
    qDebug() << ">>>>>>> on_btn_lv_clicked IN";
    kind_ = QString::fromLocal8Bit("铝");
    // 判断车重是否全为数字
    if (!checkVehicleWeighIsValid()) {
        on_btn_Next_clicked();
        return;
    }

    // 最终价格为黄板纸价格+微调价格
    float fPrice = priceWin->LineEditMap_[QString::fromLocal8Bit("铝")]->value_ + ui->le_fineAdjustment->text().toFloat()/10.0;

    finalPrice(fPrice);
    ui->lb_unitPrice->setText(QString("%1").arg(fPrice));
    setTextEnabled(false);
    showPrice(fPrice);
}

void WasteRecycle::on_btn_tong_clicked()
{
    qDebug() << ">>>>>>> on_btn_tong_clicked IN";
    // 判断车重是否全为数字
    kind_ = QString::fromLocal8Bit("铜");
    if (!checkVehicleWeighIsValid()) {
        on_btn_Next_clicked();
        return;
    }

    // 最终价格为黄板纸价格+微调价格
    float fPrice = priceWin->LineEditMap_[QString::fromLocal8Bit("铜")]->value_ + ui->le_fineAdjustment->text().toFloat()/10.0;

    finalPrice(fPrice);
    ui->lb_unitPrice->setText(QString("%1").arg(fPrice));
    setTextEnabled(false);
    showPrice(fPrice);
}


/*void WasteRecycle::on_btn_Level1_clicked()
{
    qDebug() << "on_btn_Level1_clicked IN";
    if(!check()) {
        return;
    }

    updatePrice();

    // 判断车重是否全为数字
    if (!checkVehicleWeighIsValid()) {
        on_btn_Next_clicked();
        return;
    }

    QString ePrice = ui->le_Level1ExtraPrice->text();
    float level = ePrice != "" ? (fLevel1+ePrice.toInt()/100.0) : fLevel1;
    finalPrice(level);
    ui->lb_unitPrice->setText(QString("%1").arg(level));
    setTextEnabled(false);
    showPrice(level);
    qDebug() << "on_btn_Level1_clicked OUT";
}

void WasteRecycle::on_btn_Level2_clicked()
{
    qDebug() << "on_btn_Level2_clicked IN";
    if(!check()) {
        return;
    }
    // on_btn_modify_clicked();
    updatePrice();

    // 判断车重是否全为数字
    if (!checkVehicleWeighIsValid()) {
        on_btn_Next_clicked();
        return;
    }

    // float level = (fLevel1*(ui->vslider_percent->value()) + fLevel3*(100 - ui->vslider_percent->value())) / 100;
    float level = ui->lb_percent->text().toFloat();
    finalPrice(level);
    ui->lb_unitPrice->setText(ui->lb_percent->text());
    setTextEnabled(false);
    showPrice(level);
    qDebug() << "on_btn_Level2_clicked OUT";
}

void WasteRecycle::on_btn_Level3_clicked()
{
    qDebug() << "on_btn_Level3_clicked IN";
    if(!check()) {
        return;
    }
    // on_btn_modify_clicked();
    updatePrice();

    // 判断车重是否全为数字
    if (!checkVehicleWeighIsValid()) {
        on_btn_Next_clicked();
        return;
    }

    finalPrice(fLevel3);
    ui->lb_unitPrice->setText(QString("%1").arg(fLevel3));
    setTextEnabled(false);
    showPrice(fLevel3);
    qDebug() << "on_btn_Level3_clicked OUT";
}


void WasteRecycle::on_btn_Level4_clicked()
{
    qDebug() << "on_btn_Level4_clicked IN";
    if(!check()) {
        return;
    }

    updatePrice();

    // 判断车重是否全为数字
    if (!checkVehicleWeighIsValid()) {
        on_btn_Next_clicked();
        return;
    }

    QString ePrice = ui->le_Level4ExtraPrice->text();
    float level = ePrice != "" ? (fLevel4+ePrice.toInt()/100.0) : fLevel4;
    finalPrice(level);
    ui->lb_unitPrice->setText(QString("%1").arg(level));
    setTextEnabled(false);
    showPrice(level);
    qDebug() << "on_btn_Level4_clicked OUT";
}
*/
void WasteRecycle::on_btn_Next_clicked()
{
    qDebug() << "on_btn_Next_clicked IN";
    // QString qstrText = ui->lb_Price->text();
    if(ui->le_RoughWeigh->text().trimmed() == "") {
        qWarning() << "le_RoughWeigh is empty";
        ui->le_RoughWeigh->setFocus();
        return;
    }

    bModify = false;
    bModifyUnloading = false;

    if (ui->lb_Price->text().trimmed() == "") {
        deleteData();
        storeData(0);
    }

    nextVehicle();
    qDebug() << "on_btn_Next_clicked OUT";
}


void WasteRecycle::on_cbb_NumSwitch_activated(const QString &arg1)
{
    qDebug() << "on_cbb_NumSwitch_activated IN [arg1=" << arg1 << "]";


    qDebug() << "on_cbb_NumSwitch_activated OUT [arg1=" << arg1 << "]";
}


void WasteRecycle::on_btn_set_clicked()
{
    qDebug() << "on_btn_set_clicked IN";
    priceSetWin->updatePrices();
    priceSetWin->exec();
    qDebug() << "on_btn_set_clicked OUT";
}

void WasteRecycle::on_le_VehicleWeigh_textChanged(const QString &arg1)
{
    if(ui->le_VehicleWeigh->text().isEmpty()) {
        return;
    }
    QString strRw = ui->le_RoughWeigh->text();
    QString strVw = ui->le_VehicleWeigh->text();

    bool isAllDigit = true;
    for (auto e : strVw.toStdString()) {
        if(!isdigit(e) && e != '.') {
            isAllDigit = false;
            break;
        }
    }

    qDebug() << "isAllDigit:" << isAllDigit;

    float fRw = strRw.toFloat();
    float fVw = strVw.toFloat();

    if(fRw < fVw) {
        ui->lb_NetWeight->setText("");
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("毛重或者车重数据错误!"), QString::fromLocal8Bit("关闭"));
        qWarning() << "le_RoughWeigh or le_VehicleWeigh data is wrong.";
        return;
    } else {
        float weight = 2*(fRw-fVw);
        float value = 0;
        if (weight > priceWin->LineEditMap_[QString::fromLocal8Bit("LP条件")]->value_) {
            value = priceWin->LineEditMap_[QString::fromLocal8Bit("量贩盈利率")]->value_;
            ui->doubleSpinBox->setValue(value);

        } else if(weight > priceWin->LineEditMap_[QString::fromLocal8Bit("NP条件")]->value_) {
            value = priceWin->LineEditMap_[QString::fromLocal8Bit("普贩盈利率")]->value_;
            ui->doubleSpinBox->setValue(value);
        } else if (weight > priceWin->LineEditMap_[QString::fromLocal8Bit("LR条件")]->value_) {
            value = priceWin->LineEditMap_[QString::fromLocal8Bit("量居盈利率")]->value_;
            ui->doubleSpinBox->setValue(value);
        } else {
            value = priceWin->LineEditMap_[QString::fromLocal8Bit("普居盈利率")]->value_;
            ui->doubleSpinBox->setValue(value);
        }
    }

    if (isAllDigit && strVw.trimmed() != "") {
        ui->lb_NetWeight->setText(QString("%1").arg((fRw - fVw)*2));
    } else {
        ui->lb_NetWeight->setText("");
    }
}

void WasteRecycle::on_le_RoughWeigh_textChanged(const QString &arg1)
{
    qDebug() << "on_le_RoughWeigh_textChanged IN";
    QString strRw = ui->le_RoughWeigh->text();
    QString strVw = ui->le_VehicleWeigh->text();

    if (strVw.isEmpty()) {
        return;
    }

    // 若不为数字，则忽略不计算
    for (auto e : strRw.toStdString()) {
        if(!isdigit(e) && e != '.') {
            ui->lb_NetWeight->setText("");
            return;
        }
    }

    float fRw = strRw.toFloat();
    float fVw = strVw.toFloat();

    if(!ui->le_VehicleWeigh->text().isEmpty()) {
        float weight = 2*(fRw-fVw);
        float value = 0;
        if (weight > priceWin->LineEditMap_[QString::fromLocal8Bit("LP条件")]->value_) {
            value = priceWin->LineEditMap_[QString::fromLocal8Bit("量贩盈利率")]->value_;
            ui->doubleSpinBox->setValue(value);
        } else if(weight > priceWin->LineEditMap_[QString::fromLocal8Bit("NP条件")]->value_) {
            value = priceWin->LineEditMap_[QString::fromLocal8Bit("普贩盈利率")]->value_;
            ui->doubleSpinBox->setValue(value);
        } else if (weight > priceWin->LineEditMap_[QString::fromLocal8Bit("LR条件")]->value_) {
            value = priceWin->LineEditMap_[QString::fromLocal8Bit("量居盈利率")]->value_;
            ui->doubleSpinBox->setValue(value);
        } else if (weight > priceWin->LineEditMap_[QString::fromLocal8Bit("NR条件")]->value_ ) {
            value = priceWin->LineEditMap_[QString::fromLocal8Bit("普居盈利率")]->value_;
            ui->doubleSpinBox->setValue(value);
        }
    }
    ui->lb_NetWeight->setText(QString("%1").arg((fRw - fVw)*2));

    qDebug() << "on_le_RoughWeigh_textChanged OUT";
}

/* void WasteRecycle::on_btn_modify_clicked()
{
    qDebug() << "on_btn_modify_clicked IN";
    setTextEnabled(true);
    bModify = true;
//    QString date = ui->dateEdit->text().replace("-", "_");
//    qDebug() << "date:" << date;
//    oper->sqlDeleteById(ui->lb_CurrNum->text(), date);
//    int row = ui->tableView->currentIndex().row();
//    qDebug() << "row:" << row << " data:" << model->index(row, 0).data().toString() << "   currNum:" << ui->lb_CurrNum;
//    if (model->index(row, 0).data().toString() == ui->lb_CurrNum->text()) {
//        model->removeRow(row);
//    }
//    ui->lb_Price->clear();

    qDebug() << "on_btn_modify_clicked OUT";
} */

void WasteRecycle::on_vslider_percent_valueChanged(int value)
{
//    if (bPriceInit) {
//        bPriceInit = false;
//        return;
//    }
    // qDebug() << "on_vslider_percent_valueChanged IN";
    // updatePrice();
    int pos = ui->vslider_percent->value();
    // float level = pos > 50 ? (fLevel2 + (fLevel1 - fLevel2)*(pos - 50)/50) : (fLevel2 - (fLevel2 - fLevel3)*(50 - pos)/50);
    // float level = (pos/100.0) * (fLevel1 - fLevel3) + fLevel3;
    // QString uPrice = QString("%1").arg(level);
    // ui->lb_percent->setText(towDecimalPlaces(uPrice));
    // ui->le_RoughWeigh->setFocus();
    ui->le_fineAdjustment->setText(QString("%1").arg(pos));
    // qDebug() << "on_vslider_percent_valueChanged OUT";
}

void WasteRecycle::on_le_RoughWeigh_editingFinished()
{
//    QString idx = ui->lb_CurrNum->text();
//    QString rW = ui->le_RoughWeigh->text();
//    if (rW != "" && idx.toInt() == toBeUseIndex) {
//        qDebug() << "rWeight = " << ui->le_RoughWeigh->text();
//        oper->sqlInsertUnloading(idx, rW);
//    }
}

void WasteRecycle::on_btn_Statistics_clicked()
{
    qDebug() << "on_btn_Statistics_clicked IN";
    statistics();
    if (ui->lb_Amount->isHidden()) {
        ui->lb_Amount->setVisible(true);
        ui->lb_Amount0->setVisible(true);
        ui->lb_AveragePrice->setVisible(true);
        ui->lb_AveragePrice0->setVisible(true);
        ui->lb_TotalPrices->setVisible(true);
        ui->lb_TotalPrices0->setVisible(true);
        ui->lb_TotalWeight->setVisible(true);
        ui->lb_TotalWeight0->setVisible(true);
        ui->lb_TotalProfit->setVisible(true);
        ui->lb_TotalProfit0->setVisible(true);
    } else {
        ui->lb_Amount->setHidden(true);
        ui->lb_Amount0->setHidden(true);
        ui->lb_AveragePrice->setHidden(true);
        ui->lb_AveragePrice0->setHidden(true);
        ui->lb_TotalPrices->setHidden(true);
        ui->lb_TotalPrices0->setHidden(true);
        ui->lb_TotalWeight->setHidden(true);
        ui->lb_TotalWeight0->setHidden(true);
        ui->lb_TotalProfit->setHidden(true);
        ui->lb_TotalProfit0->setHidden(true);
    }
    qDebug() << "on_btn_Statistics_clicked OUT";
}

void WasteRecycle::dataRecoveryFromUnloadingTableView(const QModelIndex &index)
{
    ui->tableView_unloading->selectRow(index.row());
    QString currIndex = model_unloading->index(index.row(), 0).data().toString();
    QString rw = model_unloading->index(index.row(), 2).data().toString();
    QString vw = model_unloading->index(index.row(), 3).data().toString();
    QString nw = model_unloading->index(index.row(), 4).data().toString();
    QString uprice = model_unloading->index(index.row(), 5).data().toString();
    QString price = model_unloading->index(index.row(), 6).data().toString();
    QString time = model_unloading->index(index.row(), 1).data().toString();
    QString usr = model_unloading->index(index.row(), 7).data().toString();

    ui->lb_CurrNum->setText(currIndex);
    ui->le_RoughWeigh->setText(rw);
    ui->le_VehicleWeigh->setText(vw);
    ui->lb_NetWeight->setText(nw);
    ui->lb_unitPrice->setText(uprice);
    ui->lb_Price->setText(price);

    // 判断车重是否全为数字
    bool isAllDigit = true;
    for (auto e : vw.toStdString()) {
        if(!isdigit(e) && e != '.') {
            isAllDigit = false;
            break;
        }
    }

    if (!isAllDigit) {
        setTextEnabled(true);
        ui->le_VehicleWeigh->setFocus();
        ui->le_VehicleWeigh->selectAll();
    } else {
        setTextEnabled(false);
    }

    QStringList tmp = time.split(" ");
    QString date = tmp.at(0);
    date = date.replace('-', '_');

    QString rwFilename = QDir::currentPath() +  "/" + date + "_rwPicture" + "/" + currIndex + "_" + rw + ".jpeg";
    QFile rfile(rwFilename);
    if (rfile.exists()) {
        QImage rwImage = QImage(rwFilename);
        ui->lb_rwScaledImage->setPixmap(QPixmap::fromImage(rwImage).scaled(ui->lb_rwScaledImage->width(), ui->lb_rwScaledImage->height()));
    }

    if(vw.trimmed() == "") {
        vw = "0";
        ui->le_VehicleWeigh->setText("0");
    }
    QString vwFilename = QDir::currentPath() +  "/" + date + "_vwPicture" + "/" + currIndex + "_" + vw + ".jpeg";
    QFile vfile(vwFilename);
    if (vfile.exists()) {
        QImage vwImage = QImage(vwFilename);
        ui->lb_vwScaledImage->setPixmap(QPixmap::fromImage(vwImage).scaled(ui->lb_vwScaledImage->width(), ui->lb_vwScaledImage->height()));
    }

    if(!usr.isEmpty()) {
        QString usrFilename = QDir::currentPath() +  "/members/" + usr + ".jpeg";
        QImage usrImage = QImage(usrFilename);
        ui->le_name->setText(usr);
        ui->lb_photo->setPixmap(QPixmap::fromImage(usrImage).scaled(ui->lb_photo->width(), ui->lb_photo->height()));
    }
}

void WasteRecycle::dataRecoveryFromTableView(const QModelIndex &index)
{
    ui->tableView->selectRow(index.row());
    QString currIndex = model->index(index.row(), 0).data().toString();
    QString rw = model->index(index.row(), 2).data().toString();
    QString vw = model->index(index.row(), 3).data().toString();
    QString nw = model->index(index.row(), 4).data().toString();
    QString uprice = model->index(index.row(), 5).data().toString();
    QString price = model->index(index.row(), 6).data().toString();
    QString time = model->index(index.row(), 1).data().toString();
    QString usr = model->index(index.row(), 7).data().toString();

    ui->lb_CurrNum->setText(currIndex);
    ui->le_RoughWeigh->setText(rw);
    ui->le_VehicleWeigh->setText(vw);
    ui->lb_NetWeight->setText(nw);
    ui->lb_unitPrice->setText(uprice);
    ui->lb_Price->setText(price);

    bool isAllDigit = true;
    for (auto e : vw.toStdString()) {
        if(!isdigit(e) && e != '.') {
            isAllDigit = false;
            break;
        }
    }

    if (!isAllDigit) {
        setTextEnabled(true);
        ui->le_VehicleWeigh->setFocus();
        ui->le_VehicleWeigh->selectAll();
    } else {
        setTextEnabled(false);
    }

    QStringList tmp = time.split(" ");
    QString date = tmp.at(0);
    date = date.replace('-', '_');

    QString rwFilename = QDir::currentPath() +  "/" + date + "_rwPicture" + "/" + currIndex + "_" + rw + ".jpeg";
    QImage rwImage = QImage(rwFilename);
    ui->lb_rwScaledImage->setPixmap(QPixmap::fromImage(rwImage).scaled(ui->lb_rwScaledImage->width(), ui->lb_rwScaledImage->height()));

    if(vw.trimmed() == "") {
        vw = "0";
        ui->le_VehicleWeigh->setText("0");
    }
    QString vwFilename = QDir::currentPath() +  "/" + date + "_vwPicture" + "/" + currIndex + "_" + vw + ".jpeg";
    QImage vwImage = QImage(vwFilename);
    ui->lb_vwScaledImage->setPixmap(QPixmap::fromImage(vwImage).scaled(ui->lb_vwScaledImage->width(), ui->lb_vwScaledImage->height()));
    if(!usr.isEmpty()) {
        QString usrFilename = QDir::currentPath() +  "/members/" + usr + ".jpeg";
        QImage usrImage = QImage(usrFilename);
        ui->le_name->setText(usr);
        ui->lb_photo->setPixmap(QPixmap::fromImage(usrImage).scaled(ui->lb_photo->width(), ui->lb_photo->height()));
    }
}

void WasteRecycle::saveCurrentData()
{
    if (ui->lb_Price->text().trimmed() == "" && ui->lb_CurrNum->text().toInt() == toBeUseIndex && ui->le_RoughWeigh->text() != "") {
        writeData(0);
        // updateTableView(0);
        updateUnloadingTableView(0);
        kind_ = QString::fromLocal8Bit("未分类");
    }
}

void WasteRecycle::modifyData()
{
    QString date = ui->dateEdit->text().replace("-", "_");
    qDebug() << "date:" << date;
    oper->sqlDeleteById(ui->lb_CurrNum->text(), date);
    int row = ui->tableView->currentIndex().row();
    qDebug() << "row:" << row << " data:" << model->index(row, 0).data().toString() << "   currNum:" << ui->lb_CurrNum;
    // 遍历tableview， 当currNum和当前要存入的num相同时， 删除tableview中数据，在存入
    int count = model->rowCount();
    for (int i = 0; i < count; ++i) {
        if (model->index(row, 0).data().toString() == ui->lb_CurrNum->text()) {
            model->removeRow(row);
            bModify = false;
        }
    }
}

void WasteRecycle::on_tableView_doubleClicked(const QModelIndex &index)
{
    qDebug() << "on_tableView_doubleClicked IN";

//    QString msg = "<font size='25' color='black'>" + QString("修改或者删除数据？") + "</font>";
//    int ret = QMessageBox::information(this,
//                                                            "警告",
//                                                            msg,
//                                                            "修改",
//                                                            "删除",
//                                                            "取消");
    if (bModifyUnloading) {
        bModifyUnloading = false;
    }
    bModify = true;
//    switch (ret) {
//    case 0:
//        saveCurrentData();
//        dataRecoveryFromTableView(index);
//        setTextEnabled(true);
//        ui->tabWidget->setCurrentWidget(ui->tab);
//        break;
//    case 1:
//        deleteData();
//        break;
//    case 2:
//        qDebug() << "cancel";
//        bModify = false;
//        break;
//    default:
//        break;
//    }
    saveCurrentData();
    dataRecoveryFromTableView(index);
    setTextEnabled(true);
    ui->tabWidget->setCurrentWidget(ui->tab);

    qDebug() << "on_tableView_doubleClicked OUT";
}

void WasteRecycle::on_le_RoughWeigh_returnPressed()
{
    ui->le_VehicleWeigh->setFocus();
}

void WasteRecycle::on_le_VehicleWeigh_returnPressed()
{
    ui->btn_Next->setFocus();
}

void WasteRecycle::on_btn_delete_clicked()
{
    qDebug() << "on_btn_delete_clicked IN";
    deleteData();
    if(ui->le_VehicleWeigh->text().trimmed() == QString::fromLocal8Bit("卸货中")) {
        oper->sqlDeleteUnloadingByIdx(ui->lb_CurrNum->text());
    }
    clearData();
    ui->lb_CurrNum->setText(QString("%1").arg(toBeUseIndex));
    setTextEnabled(true);
    ui->le_RoughWeigh->setFocus();

    qDebug() << "on_btn_delete_clicked OUT";
}

void WasteRecycle::on_btn_search_clicked()
{
    qDebug() << "[WasteRecycle] on_btn_search_clicked IN";
    model->clear();
    initTableView();
    std::list<QString> records;
    QString dat = ui->dateEdit->text().replace("-", "_");
    records = oper->sqlQueryByDate(dat);
    qDebug() << "records size:" <<  records.size();
    int i = 0;
    int k = 0;

    QDateTime date = QDateTime::currentDateTime();
    QString today = date.toString("yyyy_MM_dd");

    for (auto itor = records.begin(); i < records.size(); ++i, ++itor) {
        QStringList lines = itor->split("  ");
        if (lines.at(6).trimmed() == "" && dat == today) {
            ++k;
            continue;
        }
        for(int j = 0; j < lines.length(); ++j) {
            // qDebug() << "i=" << i << "  j=" << j << "  data=" << lines.at(j).trimmed();
            model->setItem(i-k, j, new QStandardItem(lines.at(j).trimmed()));
        }
    }
    qDebug() << "[WasteRecycle] on_btn_search_clicked OUT";
}

//template<typename ... Args>
//void parType(std::pair<QString, int> par, Args ... args) {
//    qDebug() << "pair first:" << par.first;
//    parType(args...);
//}

//template<typename T, typename ... Args>
//void parType(T par, Args ... args) {
//    qDebug() << "type:" << typeid(par).name();
//    parType(args...);
//}

//template<typename T, typename ... Args>
//void parType(T par) {
//    qDebug() << "last param:" << VNAME(par) << " type:" << typeid(par).name();
//}

void WasteRecycle::on_btn_totalWeightChart_clicked()
{
    // parType(std::pair<QString, int>("name", 1),2,3, QString("hello"));

    // 更新charts数据表
    updateTableCharts();

    // 查询charts表中的所有记录
    std::list<QString> elements;

    int halfYear = 183;
    QString limitTime = QDate::currentDate().addDays(-halfYear).toString("yyyy_MM_dd");
    QString queryChartsTable = "select * from charts where time >= '"+ limitTime + "' order by time asc;";
    elements = oper->queryTableRecords(queryChartsTable);

    std::map<QDateTime, float> totalWeightMap;
    for (auto element : elements) {
        QStringList line = element.split("  ");
        QDateTime date =  QDateTime::fromString(line.at(0), "yyyy_MM_dd");
        // qDebug() << date.toString();
        totalWeightMap[date] = line.at(2).toFloat();
    }

    // 图表绘制
    drawChart(totalWeightMap);
}

void WasteRecycle::on_btn_averageChart_clicked()
{
    // 更新charts数据表
    updateTableCharts();

    // 查询charts表中的所有记录
    std::list<QString> elements;

    int halfYear = 183;
    QString limitTime = QDate::currentDate().addDays(-halfYear).toString("yyyy_MM_dd");
    QString queryChartsTable = "select * from charts where time >= '"+ limitTime + "' order by time asc;";
    elements = oper->queryTableRecords(queryChartsTable);
    std::map<QDateTime, float> averagePriceMap;
    for (auto element : elements) {
        QStringList line = element.split("  ");
        QDateTime date =  QDateTime::fromString(line.at(0), "yyyy_MM_dd");
        // qDebug() << date.toString();
        averagePriceMap[date] = line.at(4).toFloat();
    }

    // 图表绘制
    drawChart(averagePriceMap);
}

void WasteRecycle::priceChanged()
{
//    updatePrice();
//    ui->lb_percent->setText(QString("%1").arg(fLevel2));

//    float ps = (fLevel2-fLevel3)/(fLevel1-fLevel3)  * 100;
//    int pos = ps;
//    bPriceInit = true;
//    ui->vslider_percent->setValue(pos);
}

void WasteRecycle::openSerialPort()
{
    QString port = "COM4";
    // if(ui->le_com->text() != "") {
        // port = "COM" + ui->le_com->text();
    // } else {
        // port = "COM4";
    // }
    m_serial->setPortName(port);
    m_serial->setBaudRate(1200);
    if(m_serial->open(QIODevice::ReadWrite) == false) {
        qDebug() << "serial open error:" << m_serial->errorString();
    }
}

void WasteRecycle::weighTimerHandleTimeout()
{
    qDebug() << ">>>>>>> [weighTimerHandleTimeout] weighTimer timeout";
    weighTimer->stop();
    if(count == 10) {
        count = 0;
    }
    {
        qDebug() << ">>>>>>> [weighTimerHandleTimeout] valid weigh:" << theTimingWeigh;
        std::unique_lock<std::mutex> lock(validWeightMapMutex);
        validWeightMap[count] = theTimingWeigh;
    }
    ++count;
}

void WasteRecycle::handleTimeout()
{
    m_pTimer->stop();
    // ui->lb_display->setText(QString::fromLocal8Bit("金龙纸业"));
    QPixmap myPix = QPixmap(":/images/mimeiti_da.png").scaled(600, 160);
    ui->lb_display->setPixmap(myPix);
    ui->lb_display->show();
}

void WasteRecycle::on_btn_rWrite_clicked()
{
    QString data = ui->lb_display->text();
    if(data != QString::fromLocal8Bit("金龙纸业") && data != "") {
        float tmp = data.toFloat();
        ui->le_RoughWeigh->setText(QString("%1").arg(tmp));
    }

    on_btn_roughWeightCapture_clicked();
    on_btn_Next_clicked();
    ui->tableView_unloading->setFocus();
}

void WasteRecycle::on_btn_vWrite_clicked()
{
    QString data = ui->lb_display->text();
    if(data != QString::fromLocal8Bit("金龙纸业") && data != "") {
        float tmp = data.toFloat();
        ui->le_VehicleWeigh->setText(QString("%1").arg(tmp));
    }

    on_btn_vechileWeightCapture_clicked();
}

void WasteRecycle::on_btn_rClear_clicked()
{
    ui->le_RoughWeigh->clear();
    ui->le_RoughWeigh->setFocus();
    ui->lb_rwScaledImage->clear();
    ui->lb_photo->clear();
    hasReturnZero = true;
}

void WasteRecycle::on_btn_vClear_clicked()
{
    ui->le_VehicleWeigh->clear();
    ui->le_VehicleWeigh->setFocus();
    ui->lb_vwScaledImage->clear();
}

void WasteRecycle::on_le_com_textChanged(const QString &arg1)
{
    m_serial->close();
    openSerialPort();
    QPixmap myPix = QPixmap(":/images/mimeiti_da.png").scaled(600, 160);
    ui->lb_display->setPixmap(myPix);
    ui->lb_display->show();
}

void WasteRecycle::on_tableView_unloading_doubleClicked(const QModelIndex &index)
{
    qDebug() << "on_tableView_unloading_doubleClicked IN";

//    QString msg = "<font size='25' color='black'>" +QString("修改或者删除数据？") + "</font>";
//    int ret = QMessageBox::information(this,
//                                                             "警告",
//                                                             msg,
//                                                             "修改",
//                                                             "删除",
//                                                             "取消");

    if (bModify) {
        bModify = false;
    }
    bModifyUnloading = true;
    /*switch (ret) {
    case 0:
        saveCurrentData();
        // dataRecoveryFromTableView(index);
        dataRecoveryFromUnloadingTableView(index);
        setTextEnabled(true);
        break;
    case 1:
        deleteData();
        break;
    case 2:
        qDebug() << "cancel";
        bModifyUnloading = false;
        break;
    default:
        break;
    }*/
    saveCurrentData();
    // dataRecoveryFromTableView(index);
    dataRecoveryFromUnloadingTableView(index);
    setTextEnabled(true);

    qDebug() << "on_tableView_unloading_doubleClicked OUT";
}

void WasteRecycle::on_btn_roughWeightCapture_clicked()
{
//    QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), "CapturePicture.jpeg", "*.jpeg");
//    if (!filename.endsWith(".jpeg")) {
//        filename.append(".jpeg");
//    }

//    if (ui->le_RoughWeigh->text().trimmed() == "") {
//        on_btn_rWrite_clicked();
//    }

    QDir dir;
    QDateTime date = QDateTime::currentDateTime();
    QString today = date.toString("yyyy_MM_dd");
    QString dirName = today + "_rwPicture";
    if(!dir.exists(dirName)) {
        dir.mkdir(dirName);
    }
    QString index = ui->lb_CurrNum->text();
    QString rw = ui->le_RoughWeigh->text();
    QString filename = QDir::currentPath() +  "/" + today + "_rwPicture" + "/" + index + "_" + rw + ".jpeg";
    qDebug() << "filename:" << filename;

    int iRet = OpenNetStream::getInstance()->capturePicture(sessionId_, filename.toUtf8());
    if (iRet != 0) {
        qDebug() << "save pic failed!";
        // this->showErrInfo(tr("图片保存失败！"));
        return;
    }

    // filename = "C:\\Users\\ljhuan\\Downloads\\tlya.jpg";
    QImage img(filename);
    ui->lb_rwScaledImage->setPixmap(QPixmap::fromImage(img).scaled(ui->lb_rwScaledImage->width(), ui->lb_rwScaledImage->height()));
}

void WasteRecycle::on_btn_vechileWeightCapture_clicked()
{
//    QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), "CapturePicture.jpeg", "*.jpeg");
//    if (!filename.endsWith(".jpeg")) {
//        filename.append(".jpeg");
//    }

//    if (ui->le_VehicleWeigh->text().trimmed() == "") {
//        on_btn_vWrite_clicked();
//    }

    // 当车重不是全为数字或者为空时，修改车重
//    bool flag = false;
//    if (ui->le_VehicleWeigh->text().trimmed() != "") {
//        flag = isAllDigital(ui->le_VehicleWeigh->text());
//    }
//    if (!flag) {
//        on_btn_vWrite_clicked();
//    }

    QDir dir;
    QDateTime date = QDateTime::currentDateTime();
    QString today = date.toString("yyyy_MM_dd");
    QString dirName = today + "_vwPicture";
    if(!dir.exists(dirName)) {
        dir.mkdir(dirName);
    }

    QString index = ui->lb_CurrNum->text();
    QString vw = ui->le_VehicleWeigh->text();
    if(vw.trimmed() == "") {
        vw = "0";
        ui->le_VehicleWeigh->setText("0");
    }
    QString filename = QDir::currentPath() +  "/" + today + "_vwPicture" + "/" + index + "_" + vw + ".jpeg";

    qDebug() << "filename:" << filename;

    int iRet = OpenNetStream::getInstance()->capturePicture(sessionId_, filename.toUtf8());
    if (iRet != 0) {
        qDebug() << "save pic failed!";
        // this->showErrInfo(tr("图片保存失败！"));
        return;
    }
    QImage img(filename);
    ui->lb_vwScaledImage->setPixmap(QPixmap::fromImage(img).scaled(ui->lb_vwScaledImage->width(), ui->lb_vwScaledImage->height()));

    // 获取vwPicture目录下的所有图片
    // QStringList imageList;
    /* QString imageDir = QDir::currentPath() +  "/rwPicture";
    QDirIterator it(imageDir, QDir::Files|QDir::NoSymLinks,QDirIterator::Subdirectories);
    if(vwImg2 != nullptr) {
        delete vwImg2;
        vwImg2 = nullptr;
    }
    vwImg2 = new QImage(filename);

    while(it.hasNext()) {
        QString imageFile=it.next();
        qDebug() << "imageFile:" << imageFile;

        if(rwImg1 != nullptr) {
            delete rwImg1;
            rwImg1 = nullptr;
        }
        rwImg1 = new QImage(imageFile);

        startCompare();
        Sleep(1000);
        // imageList << imageFile;
    } */
}

void WasteRecycle::replyCompareFinished(QNetworkReply *reply)
{
    qDebug() << "[WasteRecycle] replyCompareFinished IN";
    QByteArray replyData = reply->readAll();
    reply->close();
    qDebug()<<"[WasteRecycle] reply data is:"<<QString(replyData);

    jsonCompareDataParser(replyData);

    qDebug() << "[WasteRecycle] replyCompareFinished OUT";
}

void WasteRecycle::replyTokenFinished(QNetworkReply *reply)
{
    qDebug() << "[WasteRecycle] replyTokenFinished IN";
    QByteArray replyData = reply->readAll();
    reply->close();
    // qDebug()<<"reply data is:"<<QString(replyData);

    Json::Reader reader;
    Json::Value value;
    if(reader.parse(replyData.data(), value)) {
        token_ = QString::fromStdString(value["access_token"].asString());
        qDebug() << "token:" << token_;
    }

    qDebug() << "[WasteRecycle] replyTokenFinished OUT";
}

void WasteRecycle::on_btn_openRoughWeightDir_clicked()
{
    QDateTime date = QDateTime::currentDateTime();
    QString today = date.toString("yyyy_MM_dd");
    QString path = QDir::currentPath() +  "/" + today + "_rwPicture";
    qDebug() << "[WasteRecycle] RoughWeightDir:" << path;
    QDesktopServices::openUrl(QUrl("file:"+path, QUrl::TolerantMode));
}

void WasteRecycle::on_btn_openVehicleWeightDir_clicked()
{
    QDateTime date = QDateTime::currentDateTime();
    QString today = date.toString("yyyy_MM_dd");
    QString path = QDir::currentPath() +  "/" + today + "_vwPicture";
    qDebug() << "[WasteRecycle] VehicleWeightDir:" << path;
    QDesktopServices::openUrl(QUrl("file:"+path, QUrl::TolerantMode));
}

void WasteRecycle::on_btn_reconnection_clicked()
{
    m_serial->clear();
    m_serial->clearError();
    m_serial->close();
    m_serial->reset();
    m_serial->setBaudRate(1200);
    m_serial->setPortName("COM4");
    if(m_serial->open(QIODevice::ReadWrite) == false) {
        qDebug() << "serial open error:" << m_serial->errorString();
    }
}

void WasteRecycle::clicked_rightMenu(const QPoint &pos)
{
    QTableView *tableView=qobject_cast<QTableView*>(sender());
    // qDebug() << "table name:" << tableView->objectName();
    rightMenu->setObjectName(tableView->objectName());
    rightMenu->exec(QCursor::pos());
    rightMenu->setObjectName("");
}

void WasteRecycle::on_btn_monthlyStatics_clicked()
{
    // 统计近一个月的总量，平均价，总价
    qDebug() << "[WasteRecycle] on_btn_monthlyStatics_clicked IN";
    QDate today = QDateTime::currentDateTime().date();
    QDate monthEarlyDay = today.addMonths(-1);

    QString sqlQueryCharts = "select * from charts where time < '"+ today.toString("yyyy_MM_dd")+"' and time >='" + monthEarlyDay.toString("yyyy_MM_dd")+"';";
    std::list<QString> elements = oper->queryTableCharts(sqlQueryCharts);

    float monthTotalWeight = 0.0, monthAveragePrice = 0.0, monthTotalPrice = 0.0;
    int days = 0;
    for (auto element : elements) {
        QStringList line = element.split("  ");
        monthTotalWeight += line.at(2).toFloat();
        monthTotalPrice += line.at(3).toFloat();
        monthAveragePrice += line.at(4).toFloat();
    }

    days = elements.size();
    // days = monthEarlyDay.daysTo(today);
    monthAveragePrice /= days;

    monthlyStatics* monthlyWin = new monthlyStatics(monthTotalWeight, monthAveragePrice, monthTotalPrice, days);
    monthlyWin->show();

    qDebug() << "[WasteRecycle] on_btn_monthlyStatics_clicked OUT";
}

void WasteRecycle::on_btn_capture_clicked()
{
    QDateTime qtime = QDateTime::currentDateTime();
    QString time = qtime.toString("yyyyMMddhhmmss");

    QString filename = QDir::currentPath() + time + ".jpeg";
    ui->lb_path->setText(filename);

    int iRet = OpenNetStream::getInstance()->capturePicture(sessionId_, filename.toUtf8());
    if (iRet != 0) {
        qDebug() << "save pic failed!";
        this->showErrInfo(QString::fromLocal8Bit("图片保存失败！"));
        return;
    }

    int width = ui->lb_show->width();
    int height = ui->lb_show->height();
    QPixmap myPix = QPixmap(filename).scaled(width, height);
    ui->lb_show->setPixmap(myPix);
    ui->lb_show->show();
}

void WasteRecycle::on_btn_register_clicked()
{   
    QString phone = ui->le_phone->text();
    QString group = "packmen";
    QString pic;

    if (ui->le_phone->text().isEmpty() || ui->le_name->text().isEmpty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("提醒"), QString::fromLocal8Bit("注册信息不完整"), QString::fromLocal8Bit("关闭"));
        return;
    }
    if(ui->lb_path->text().isEmpty()) {
        // pic = QDir::currentPath() +  "/members/" + ui->le_phone->text() + ".jpeg";
    } else {
        pic = ui->lb_path->text();
    }
    QString name = ui->le_name->text();
    std::string res = api_->user_add(phone.toStdString().c_str(), group.toStdString().c_str(), pic.toStdString().c_str(), 2, phone.toStdString().c_str());
    qDebug() << ">>>>>>> ------res is:" << QString::fromStdString(res);
    // ui->lb_show->setText(QString::fromStdString(res));
    Json::Reader reader;
    Json::Value value;
    if (reader.parse(res, value)) {
        int err = value["errno"].asInt();
        if (err != 0) {
            std::string msg = value["msg"].asString();
            QString qmsg = QString::fromLocal8Bit("错误码：") + QString("%1").arg(err) + "\n" + QString::fromLocal8Bit("错误信息：") + QString::fromStdString(msg);
            QMessageBox::information(this, QString::fromLocal8Bit("注册失败！"), qmsg, QString::fromLocal8Bit("关闭"));
            return;
        }
    }

    // 还需要对返回信息做解析判断
    // 如果成功则保存图片到members文件夹, 并且将注册信息保存到db 的 members表格中

    QDir dir;
    QString dirName = "members";
    if(!dir.exists(dirName)) {
        dir.mkdir(dirName);
    }

    QString newName = QDir::currentPath() + "/members/" + name + ".jpeg";
    dir.rename(pic, newName);

    QDateTime qtime = QDateTime::currentDateTime();
    QString time = qtime.toString("yyyy-MM-dd hh:mm:ss");

    // 测试下 phone相同时， 数据是否可以覆盖
    QString insertMembersTable =  "insert into members values('" + phone + "'" + ", '" + name + "'"
            + ", '" +  time + "');";

    phoneNameMap_[phone] = name;
    oper->insertTable(insertMembersTable);

    // 注册时新建以该用户ID为表名的用户表
//    QString createUserTable = "create table "+ phone + " (time varchar(100));";
//    oper->createTable(createUserTable);

    QMessageBox::information(this, QString::fromLocal8Bit("注册成功！"), QString::fromLocal8Bit("注册成功！"), QString::fromLocal8Bit("关闭"));
    ui->le_phone->clear();
    ui->le_name->clear();
    ui->lb_show->clear();
    ui->lb_path->clear();
    ui->le_name->setFocus();
    api_->load_db_face();
}

void WasteRecycle::remove(const QString imgPath) {
    QDir dir;
    dir.remove(imgPath);
    QString featurePath = imgPath;
    featurePath = featurePath.replace('.', "_feature.");
    dir.remove(featurePath);
}

bool WasteRecycle::searchUnloadingTableByName(QString &name)
{
    // 当在unloading表格中查找到该用户并且车重不为“卸载中”或者空并且车重毛重相差超过一公斤时，执行恢复操作，之后再点击车重填入按钮
    // 当在卸货表格中查找不到当前号码时，则填入毛重；当查到当前号码时，则填入车重
    int rowCount = model_unloading->rowCount();
    int row = 0;
    for (; row < rowCount; ++row) {
        QString vWeigh = model_unloading->index(row, 3).data().toString();
        QString usr = model_unloading->index(row, 7).data().toString();
        if(usr == name && (vWeigh == "" || vWeigh == QString::fromLocal8Bit("卸货中"))) {
            QModelIndex index = model_unloading->index(row, 7);
            QString data = ui->lb_display->text();
            float fVWeight = data.toFloat();
            float fRWeight = model_unloading->index(row, 2).data().toFloat();

            // 当待机或者毛重车重相差不足一公斤时，则认为小贩还未开始卸货，不进行结算
            if (data == "" || (fRWeight-fVWeight) < 1) {
                qDebug() << "fRWeight-fVWeight is less than 1 kilo";
            } else {
                on_tableView_unloading_doubleClicked(index);
            }

            return true;
        }
    }
    if (row == rowCount) {
        return false;
    }
}

void WasteRecycle::parseInfo(QString& info) {
    Json::Reader reader;
    Json::Value	value;
    Json::Value  result;
    // Json::Value  userInfos_value;
    std::string userID;
    std::string score;
    QString userInfos;
    QString userName;
    if(reader.parse(info.toUtf8().data(), value)) {
        int err = value["errno"].asInt();
        if (err != 0) {
            qDebug() << "err:" << err;
            return;
        }

        result = value["data"]["result"];
        for (int i = 0; i < result.size(); ++i) {
            userID = result[i]["user_id"].asString();
            score = result[i]["score"].asString();
            std::string groupID = result[i]["group_id"].asString();
            qDebug() << "userID:" << QString::fromStdString(userID);
            qDebug() << "groupID:" << QString::fromStdString(groupID);
            // userInfos = QString::fromUtf8(api_->get_user_info(userID.c_str(), groupID.c_str()));
            userName = phoneNameMap_[QString::fromStdString(userID)];
        }
    } else {
        qDebug() << "json file error";
    }

    ui->le_name->setText(userName);
    ui->le_phone->setText(QString::fromStdString(userID));
    QString scoreMsg = QString::fromLocal8Bit("相似度：") + QString::fromStdString(score);
    ui->lb_similarity->setText(scoreMsg);

//    if (!userInfos.isEmpty() && reader.parse(userInfos.toUtf8().data(), userInfos_value)) {
//        int err = value["errno"].asInt();
//        if (err != 0) {
//            qDebug() << "err:" << err;
//            return;
//        }

//        result = userInfos_value["data"]["result"];
//        for (int i = 0; i < result.size(); ++i) {
//            std::string userInfo = result[i]["user_info"].asString();
//            qDebug() << "userInfo:" << QString::fromStdString(userInfo);
//            ui->le_name->setText(QString::fromStdString(userInfo));
//            ui->le_phone->setText(QString::fromStdString(userID));
//            QString scoreMsg = QString::fromLocal8Bit("相似度：") + QString::fromStdString(score);
//            ui->lb_similarity->setText(scoreMsg);
//        }
//    } else {
//        qDebug() << "json file error";
//    }
}

void WasteRecycle::identify(const QString & imgPath) {
    qDebug() << ">>>>>>> identify IN";
    qDebug() << ">>>>>>> imgPath:" << imgPath;

    std::string res;
    {
        std::unique_lock<std::mutex> guard(apiMutex_);
        // std::this_thread::sleep_for(std::chrono::milliseconds(500));
        res = api_->identify(imgPath.toUtf8().data(), 2);
    }
    QString info = QString::fromStdString(res).replace("\n", "").replace("\t", "").replace("\\", "");
    qDebug() << ">>>>>>> ---200 identify res is:" << info;

    parseInfo(info);

    qDebug() << ">>>>>>> identify OUT";
}

void WasteRecycle::analyze(const QString imgPath) {
    qDebug() << ">>>>>>> [analyze] analyze IN";
    qDebug() << ">>>>>>> [analyze] imgPath:" << imgPath;

    Json::Reader reader;
    Json::Value	value;
    Json::Value  result;
    // Json::Value  userInfos_value;
    std::string userID;
    std::string score;
    static std::string currentUser;
    // std::string userInfo;
    // QString userInfos;
    QString userName;
    int s = 0, e = 0, s1 = 0, e1 = 0;
    s1 = clock();

    // 1. 处于修改状态时， 不进行人脸识别
    if (bModify || bModifyUnloading) {
        qDebug() << ">>>>>>> [analyze] in modify modern, no face analyze!";
        return;
    }
    // 2. 地磅未归零时， 不进行人脸识别
//    if (!hasReturnZero) {
//        qDebug() << ">>>>>>> [analyze] has't back to zero";
//        return;
//    }

    // 3. 未显示数字，与地磅连接有问题时，不进行人脸识别
    QString data = ui->lb_display->text();
    if(data == QString::fromLocal8Bit("金龙纸业") || data == "") {
        qDebug() << ">>>>>>> [analyze] display is wrong";
        return;
    }

    QString featurePath = imgPath;
    featurePath = featurePath.replace('.', "_feature.");

    std::string res;
    {
        std::unique_lock<std::mutex> guard(apiMutex_);
        // std::this_thread::sleep_for(std::chrono::milliseconds(500));
        s = clock();
        res = api_->identify(imgPath.toUtf8().data(), 2);
        e = clock();
    }

    qDebug() << ">>>>>>> [analyze] api_->identify: " << (e - s) << " ms";

    QString info = QString::fromStdString(res).replace("\n", "").replace("\t", "").replace("\\", "");
    qDebug() << ">>>>>>> [analyze] ---200 analyze res is:" << info;


    if(reader.parse(info.toUtf8().data(), value)) {
        int err = value["errno"].asInt();
        // 4. 识别错误时， 不再继续
        if (err != 0) {
            // 图片删除
            remove(imgPath);
            remove(featurePath);
            qDebug() << ">>>>>>> [analyze] err:" << err;

            // 继续取流进行人脸识别
            on_btn_startAnalyze_clicked();
            return;
        }

        result = value["data"]["result"];
        for (int i = 0; i < result.size(); ++i) {
            userID = result[i]["user_id"].asString();
            score = result[i]["score"].asString();
            // std::cout << "score:" << QString::fromStdString(score).toFloat() << " maxScore:" << maxScore << std::endl;
            float sc = QString::fromStdString(score).toFloat();
            float threshold = priceSetWin->threshold_.toFloat();

            // 5. 识别出的人脸置信度低于阈值时，不再继续
            if (sc < threshold ) {
                qDebug() << ">>>>>>> [analyze] score is too low, score:" << QString::fromStdString(score) << " threshold:" << priceSetWin->threshold_;
                // 分数比较低的图片删除
                remove(imgPath);
                remove(featurePath);
                // 继续取流进行人脸识别
                on_btn_startAnalyze_clicked();
                return;
            }

            // 6. 识别用户切换，但地磅未归零时，不再继续
            if (userID != currentUser/* && hasReturnZero==false*/) {
//                qDebug() << ">>>>>>> [analyze] has not yet return zero and not analyze!, current userID is:" << QString::fromStdString(userID);
//                return;
            } else {
                float data = ui->lb_display->text().toFloat();
                std::unique_lock<std::mutex> lock(validWeightMapMutex);
                int size = validWeightMap.size();
                int i = 0;
                for (; i < size ;++i) {
                    if((data - validWeightMap[i] < 1) || (validWeightMap[i] - data < 1)) {
                        break;
                    }
                }

                // 7. 地磅数据未稳定时，不再继续
                if (ui->lb_display->text() !="" && i == size) {
                    qDebug() << ">>>>>>> [analyze] weighbridge data is not stable!, current number is not validWeigh!";
                    // 继续取流进行人脸识别
                    on_btn_startAnalyze_clicked();
                    return;
                }
            }

            if (userID != currentUser) {
                currentUser = userID;
                qDebug() << ">>>>>>> [analyze] User change, currentUser:" <<  QString::fromStdString(currentUser);
            }
            std::string groupID = result[i]["group_id"].asString();
            qDebug() << ">>>>>>> userID:" << QString::fromStdString(userID);
            qDebug() << ">>>>>>> groupID:" << QString::fromStdString(groupID);
            // userInfos = QString::fromUtf8(api_->get_user_info(userID.c_str(), groupID.c_str()));
            userName = phoneNameMap_[QString::fromStdString(userID)];
        }
    } else {
        // 8. json 信息解析出错时，不再继续
        qDebug() << ">>>>>>> json file error";
        // 继续取流进行人脸识别
        on_btn_startAnalyze_clicked();
        return;
    }

    ui->le_name->setText(userName);
    ui->le_phone->setText(QString::fromStdString(userID));
    QString scoreMsg = QString::fromLocal8Bit("相似度：") + QString::fromStdString(score);
    ui->lb_similarity->setText(scoreMsg);

//    if (!userInfos.isEmpty() && reader.parse(userInfos.toUtf8().data(), userInfos_value)) {
//        // 9. 获取用户信息失败时，不再继续
//        int err = value["errno"].asInt();
//        if (err != 0) {
//            qDebug() << ">>>>>>> get_user_info err:" << err;
//            // 继续取流进行人脸识别
//            on_btn_startAnalyze_clicked();
//            return;
//        }

//        result = userInfos_value["data"]["result"];
//        for (int i = 0; i < result.size(); ++i) {
//            userInfo = result[i]["user_info"].asString();
//            qDebug() << ">>>>>>> userInfo:" << QString::fromStdString(userInfo);
//            ui->le_name->setText(QString::fromStdString(userInfo));
//            ui->le_phone->setText(QString::fromStdString(userID));
//            QString scoreMsg = QString::fromLocal8Bit("相似度：") + QString::fromStdString(score);
//            ui->lb_similarity->setText(scoreMsg);
//        }
//    } else {
//        qDebug() << ">>>>>>> json file error";
//        // 继续取流进行人脸识别
//        on_btn_startAnalyze_clicked();
//        return;
//    }

    // on_btn_startAnalyze_clicked();

    // 当在unloading表格中查找到该用户并且车重不为“卸载中”或者不为空并且毛重不为空并且车重毛重相差超过一公斤时，执行恢复操作，之后再点击车重填入按钮
    // 当在卸货表格中查找不到当前号码时，则填入毛重；当查到当前号码时，则填入车重
    bool exist = searchUnloadingTableByName(userName);

    bool toShow = true;

    wi_->clear();
    wi_->name_ = userName;
    wi_->num_ = ui->lb_CurrNum->text();

    if (exist) {
        // 当数据未恢复时，则不进行结算
        if (ui->le_RoughWeigh->text().isEmpty() && ui->le_RoughWeigh->text().isEmpty()) {
            qDebug() << ">>>>>>> toShow is false";
            toShow = false;
        } else {
            float tmp = data.toFloat();
            ui->le_VehicleWeigh->setText(QString("%1").arg(tmp));
            wi_->rWeight_ = ui->le_RoughWeigh->text();
            wi_->vWeight_ = ui->le_VehicleWeigh->text();
            on_btn_vechileWeightCapture_clicked();
        }
    } else {
        float tmp = data.toFloat();
        ui->le_RoughWeigh->setText(QString("%1").arg(tmp));

        wi_->rWeight_ = ui->le_RoughWeigh->text();
        on_btn_roughWeightCapture_clicked();
//        if (wi_->rWeight_ != "") {
//            on_btn_roughWeightCapture_clicked();
//        } else {
//            toShow = false;
//            pause_ = true;
//        }
    }

    e1 = clock();
    qDebug() << ">>>>>>> [analyze] analyze : " << (e1 - s1) << " ms";
    QString index = ui->lb_CurrNum->text();
    if(toShow) {
        wi_->flush();
        // pause_ = true;
        wi_->exec();

        if (!exist && flag_) {
            on_btn_Next_clicked();
            hasReturnZero = false;
            ui->tableView_unloading->setFocus();
        }
    }
    // 达标的图片转存到bestFaces下
    QDir dir;
    QString dirName = "bestFaces";
    if(!dir.exists(dirName)) {
        dir.mkdir(dirName);
    }


    QString tag;
    if (ui->le_RoughWeigh->text() == "") {
        tag = "_" + index + "_rw";
    } else {
        tag = "_" + index + "_vw";
    }
    QString fileName = QDir::currentPath() + "/bestFaces/" + QString::fromStdString(userID) + tag+ ".jpg";
    remove(fileName);

    dir.rename(imgPath, fileName);
//    QString bestFeaturePath = fileName.replace('.', "_feature.");
//    dir.rename(featurePath, bestFeaturePath);

    qDebug() << ">>>>>>> [analyze] analyze OUT";
}

void WasteRecycle::on_btn_analyze_clicked()
{
    QString img = ui->lb_path->text();
    if (img.isEmpty()) {
        QString msg = QString::fromLocal8Bit("未选择图片");
        ui->lb_similarity->setText(msg);
        return;
    }
    identify(img);
    ui->lb_path->clear();
}

void WasteRecycle::on_btn_select_clicked()
{
    QString imageName;
    imageName = QFileDialog::getOpenFileName(this,
                                             QString::fromLocal8Bit("选择图片"),
                                             "",
                                             tr("Images (*.png *.bmp *.jpg *.jpeg *.tif *.GIF )"));
    if(imageName.isEmpty()) {
        qWarning() << "image is empty.";
         return;
    }

    int width = ui->lb_show->width();
    int height = ui->lb_show->height();
    QPixmap myPix = QPixmap(imageName).scaled(width, height);
    ui->lb_show->setPixmap(myPix);
    ui->lb_path->setText(imageName);
    ui->le_name->clear();
    ui->le_phone->clear();
}

void WasteRecycle::on_btn_faceAnalyze_clicked()
{
    if(t_ != nullptr) {
        stop_ = true;
        ui->btn_faceAnalyze->setText(QString::fromLocal8Bit("开启人脸识别"));
        delete t_;
        t_ = nullptr;
//        if(t_->joinable()) {
//            qDebug() << "waiting face analyze thread to quit...";
//            t_->join();
//            qDebug() << "face analye thread is already quit.";

//        }
    } else {
        stop_ = false;
        // pause_ = false;
        ui->btn_faceAnalyze->setText(QString::fromLocal8Bit("停止人脸识别"));
        t_ = new std::thread(&WasteRecycle::face_collect_opencv_video, this);
        t_->detach();
    }
}

void WasteRecycle::changeEvent(QEvent *event)
{
    if(event->type()!=QEvent::WindowStateChange) return;
    if(this->windowState()==Qt::WindowMaximized)
    {
       qDebug() << "windows size change...";
    }
}

void WasteRecycle::on_btn_logout_clicked()
{
    std::string userID = ui->le_phone->text().toStdString();

    if (userID.empty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("用户ID为空，用户ID为用户的电话号码"), QString::fromLocal8Bit("关闭"));
    }

    std::string group = "packmen";
    std::string res = api_->user_delete(userID.c_str(), group.c_str());

    // 对返回信息进行解析
    qDebug() << "-----user_delete res:" << QString::fromStdString(res);
    api_->load_db_face();

    // 数据库members 表中删除对应的人员信息，phoneNameMap_中删除对应的人员信息
    oper->sqlDeleteMembersByPhone(QString::fromStdString(userID));

    ui->le_name->clear();
    ui->le_phone->clear();
    ui->lb_similarity->clear();
}

void WasteRecycle::setFlag(bool flag)
{
    std::cout << "flag:" << flag << std::endl;
    flag_ = flag;
}

void WasteRecycle::on_btn_startAnalyze_clicked()
{
    if(!ui->le_VehicleWeigh->text().isEmpty()) {
        qDebug() << QString::fromLocal8Bit("等待报价中，不允许开始新的识别任务");
        return;
    }
    if (pause_ == true) {
        pause_ = false;
        ui->btn_startAnalyze->setText(QString::fromLocal8Bit("停止"));
    } else {
        pause_ = true;
        ui->btn_startAnalyze->setText(QString::fromLocal8Bit("开始"));
    }
}

void WasteRecycle::on_btn_dataAnalysis_clicked()
{
    std::list<QString> elements;
    QString date = ui->de_date->text();
    QString tableName = "record_" + date;
    elements = oper->sqlLimitQueryByDate(date, ui->le_upperLimit->text(), ui->le_downLimit->text());

    int num = 0;//elements.size();
    float totalWeight = 0.0;
    float totalPrice = 0.0;
    QString species = ui->le_species->text();


    for (auto e : elements) {
        QStringList items = e.split("  ");
        QString kind = items.at(8);
        if (species.isEmpty() || species.contains(kind)) {
            totalWeight += items.at(4).toFloat();
            totalPrice += items.at(6).toFloat();
            ++num;
        }
    }

    QString msg = ui->tb_analyzeResults->document()->toPlainText();
    if (!msg.isEmpty()) {
        msg += "\n";
    }
    msg += ui->le_downLimit->text() + " - " + ui->le_upperLimit->text() + " : "  + QString("%1").arg(totalWeight) + QString::fromLocal8Bit("斤") +
            QString::fromLocal8Bit("   数量 : ") + QString("%1").arg(num) + QString::fromLocal8Bit(" 总价 : ") + QString("%1").arg(totalPrice) ;
    ui->tb_analyzeResults->setText(msg);
}

void WasteRecycle::on_btn_priceSet_clicked()
{
    priceWin->exec();
}

void WasteRecycle::on_btn_clear_clicked()
{
    ui->tb_analyzeResults->document()->clear();
}

void WasteRecycle::myLineEditClicked()
{
    MyLineEdit *myle=qobject_cast<MyLineEdit*>(sender());
    myle->selectAll();
}

void WasteRecycle::lineEditClicked()
{
    MyLineEdit *myle=qobject_cast<MyLineEdit*>(sender());
    myle->clear();
}

void WasteRecycle::on_btn_searchAndStatic_clicked()
{
    // 统计近一个月的总量，平均价，总价
    qDebug() << "[WasteRecycle] on_btn_searchAndStatic_clicked IN";
    QDate today = QDateTime::currentDateTime().date();
    QDate monthEarlyDay = today.addMonths(-1);

    QString from = ui->de_from->text();
    QString to = ui->de_to->text();

    QString sqlQueryCharts = "select * from charts where time < '"+ to+"' and time >='" + from+"';";
    std::list<QString> elements = oper->queryTableCharts(sqlQueryCharts);

    float totalWeight = 0.0, averagePrice = 0.0, totalPrice = 0.0;
    int days = 0;
    for (auto element : elements) {
        QStringList line = element.split("  ");
        totalWeight += line.at(2).toFloat();
        totalPrice += line.at(3).toFloat();
        averagePrice += line.at(4).toFloat();
    }

    days = elements.size();
    // days = monthEarlyDay.daysTo(today);
    averagePrice /= days;

    qDebug() << "totalWeight: " << totalWeight << " totalPrice: " << totalPrice << " averagePrice:" << averagePrice;

    monthlyStatics* monthlyWin = new monthlyStatics(totalWeight, averagePrice, totalPrice, days);
    monthlyWin->show();

    qDebug() << "[WasteRecycle] on_btn_searchAndStatic_clicked OUT";
}

void WasteRecycle::on_btn_finalCal_clicked()
{
    kind_ = QString::fromLocal8Bit("板纸");
    float huangban = ui->le_huangban->text().toFloat();
    float chaoshi = ui->le_chaoshi->text().toFloat();
    float tonghuo = ui->le_tonghuo->text().toFloat();

    float hbPrice = priceWin->paperPrices_[QString::fromLocal8Bit("黄板")] * (1-ui->spinBox->value()/100.0) - ui->doubleSpinBox->value(); // + ui->le_fineAdjustment->text().toFloat()/100.0;
    hbPrice = ((int)(hbPrice*100))/100.0;
    float csPrice = priceWin->paperPrices_[QString::fromLocal8Bit("超市")] * (1-ui->spinBox->value()/100.0) - ui->doubleSpinBox->value(); // + ui->le_fineAdjustment->text().toFloat()/100.0;
    csPrice = ((int)(csPrice*100))/100.0;
    float thPrice = priceWin->paperPrices_[QString::fromLocal8Bit("统货")] * (1-ui->spinBox->value()/100.0) - ui->doubleSpinBox->value(); // + ui->le_fineAdjustment->text().toFloat()/100.0;
    thPrice = ((int)(thPrice*100))/100.0;

    float fPrice = 0.0;

    if((huangban+chaoshi+tonghuo)>0) {
        fPrice = (hbPrice*huangban + csPrice*chaoshi + thPrice*tonghuo) / (huangban + chaoshi + tonghuo);  // + ui->le_fineAdjustment->text().toFloat()/100.0;
        fPrice = ((int)(fPrice*100))/100.0;
        fPrice += ui->le_fineAdjustment->text().toFloat()/100.0;
    }

    finalPrice(fPrice);
    ui->lb_unitPrice->setText(QString("%1").arg(fPrice));
    setTextEnabled(false);
    showPrice(fPrice);
}

void WasteRecycle::on_btn_resident_clicked()
{
    float rate = priceWin->LineEditMap_[QString::fromLocal8Bit("量居盈利率")]->value_;
    ui->doubleSpinBox->setValue(rate);
}

void WasteRecycle::on_btn_packman_clicked()
{
    float rate = priceWin->LineEditMap_[QString::fromLocal8Bit("普贩盈利率")]->value_;
    ui->doubleSpinBox->setValue(rate);
}
