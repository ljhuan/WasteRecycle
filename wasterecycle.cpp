#include <string>
#include <list>
#include <map>
#include <cctype>
#include "wasterecycle.h"
#include "ui_wasterecycle.h"
#include "sqloper.h"
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

#define VNAME(value)  (#value)
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) > (y) ? (y) : (x))
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

WasteRecycle::WasteRecycle(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WasteRecycle),
    toBeUseIndex(100),
    oper(new SqlOper),
    priceSetWin(nullptr),
    model(new QStandardItemModel),
    fLevel1(0.0),
    fLevel2(0.0),
    fLevel3(0.0),
    fLevel4(0.0),
    bPriceInit(false),
    fWeight(0.0)
{
    priceSetWin = new PriceSetDialog(parent, oper);
    ui->setupUi(this);
    ui->le_RoughWeigh->clear();
    ui->le_VehicleWeigh->clear();

    // 按键的事件注册
    ui->btn_Level1->installEventFilter(this);
    ui->btn_Level2->installEventFilter(this);
    ui->btn_Level3->installEventFilter(this);
    ui->btn_Level4->installEventFilter(this);

    // 初始化tableView
    initTableView();

    QDateTime date = QDateTime::currentDateTime();
    QString today = date.toString("yyyy_MM_dd");
    std::list<QString> records, unloadings;
    records = oper->sqlQueryByDate(today);
    qDebug() << "records size:" <<  records.size();
    int i = 0;

    for (auto itor = records.begin(); i < records.size(); ++i, ++itor) {
        QStringList lines = itor->split("  ");
        for(int j = 0; j < lines.length(); ++j) {
            model->setItem(i, j, new QStandardItem(lines.at(j).trimmed()));
        }
        toBeUseIndex = lines.at(0).toInt() + 1;
    }

    unloadings = oper->sqlQueryUnloadingByDate(today);
    for (auto e : unloadings) {
        QString tim = e.split("  ").at(0);
        QString idx = e.split("  ").at(1);
        QString rW = e.split("  ").at(2);
        int row = model->rowCount();
        model->setItem(row, 0, new QStandardItem(idx));
        model->setItem(row, 1, new QStandardItem(tim));
        model->setItem(row, 2, new QStandardItem(rW));
        model->setItem(row, 3, new QStandardItem(QString::fromLocal8Bit("卸货中")));

        // ui->le_RoughWeigh->setText(rW);
        if (idx.toInt() >= toBeUseIndex) {
            toBeUseIndex = idx.toInt() + 1;
        }
    }
    model->sort(0);

    // 隐藏各项数据统计
    ui->lb_Amount->setHidden(true);
    ui->lb_Amount0->setHidden(true);
    ui->lb_AveragePrice->setHidden(true);
    ui->lb_AveragePrice0->setHidden(true);
    ui->lb_TotalPrices->setHidden(true);
    ui->lb_TotalPrices0->setHidden(true);
    ui->lb_TotalWeight->setHidden(true);
    ui->lb_TotalWeight0->setHidden(true);

    ui->lb_CurrNum->setText(QString::fromStdString(std::to_string(toBeUseIndex)));

    updatePrice();
    float ps = (fLevel2-fLevel3)/(fLevel1-fLevel3)  * 100;
    int pos = ps;
    // qDebug() << "ps:" << ps << "  pos:" << pos;

    ui->vslider_percent->setMaximum(100);
    ui->vslider_percent->setMinimum(0);
    bPriceInit = true;
    ui->vslider_percent->setValue(pos);
    ui->lb_percent->setText(QString("%1").arg(fLevel2));


    // 配置背景图
    QPixmap pixmap = QPixmap( ":/images/0.jpg").scaled(this->size());
    QPalette  palette (this->palette());
    palette .setBrush(QPalette::Background, QBrush(pixmap));
    this->setPalette( palette );

    // 设置金龙纸业label
    QPixmap myPix = QPixmap(":/images/mimeiti_da.png").scaled(425, 97);
    ui->lb_display->setPixmap(myPix);
    ui->lb_display->show();
    // ui->lb_display->setScaledContents(true);

    // 设置默认的焦点
    ui->le_RoughWeigh->setFocus();

    connect(priceSetWin, SIGNAL(finished(int)), this, SLOT(priceChanged()));

    // 日历设置
    QCalendarWidget* calendar = new QCalendarWidget(this);
    calendar->hide();

    ui->dateEdit->setCalendarPopup(true);
    ui->dateEdit->setDisplayFormat("yyyy-MM-dd");
    ui->dateEdit->setDate(calendar->selectedDate());

    // 串口设置
    m_serial = new QSerialPort();
    initActionsConnections();

    // 定时器设置
    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
}

WasteRecycle::~WasteRecycle()
{
    delete ui;
    ui = nullptr;
    delete priceSetWin;
    priceSetWin = nullptr;
    delete oper;
    oper = nullptr;
    delete model;
    model = nullptr;
}

void WasteRecycle::initTableView() {
    model->setHorizontalHeaderItem(0, new QStandardItem(QString::fromLocal8Bit("号码")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString::fromLocal8Bit("时间")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QString::fromLocal8Bit("毛重")));
    model->setHorizontalHeaderItem(3, new QStandardItem(QString::fromLocal8Bit("车重")));
    model->setHorizontalHeaderItem(4, new QStandardItem(QString::fromLocal8Bit("净重")));
    model->setHorizontalHeaderItem(5, new QStandardItem(QString::fromLocal8Bit("单价")));
    model->setHorizontalHeaderItem(6, new QStandardItem(QString::fromLocal8Bit("价格")));

    ui->tableView->setModel(model);
    ui->tableView->setColumnWidth(0, 40);
    ui->tableView->setColumnWidth(1, 160);
    ui->tableView->setColumnWidth(2, 70);
    ui->tableView->setColumnWidth(3, 70);
    ui->tableView->setColumnWidth(4, 70);
    ui->tableView->setColumnWidth(5, 50);
    ui->tableView->setColumnWidth(6, 70);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);  // 设置选中模式为选中行
    ui->tableView->setSelectionMode( QAbstractItemView::SingleSelection);  // 设置选中单个
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 设置不可编辑
}

void WasteRecycle::keyPressEvent(QKeyEvent *e)
{
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
        if(ui->btn_Level1->hasFocus()) {
            on_btn_Level1_clicked();
        } else if (ui->btn_Level2->hasFocus()) {
            on_btn_Level2_clicked();
        } else if (ui->btn_Level3->hasFocus()) {
            on_btn_Level3_clicked();
        } else if (ui->btn_Level4->hasFocus()) {
            on_btn_Level4_clicked();
        } else if (ui->btn_Next->hasFocus()) {
            on_btn_Next_clicked();
        }
        break;
    case Qt::Key_Return:
        qDebug() << "key return";
        if(ui->btn_Level1->hasFocus()) {
            on_btn_Level1_clicked();
        } else if (ui->btn_Level2->hasFocus()) {
            on_btn_Level2_clicked();
        } else if (ui->btn_Level3->hasFocus()) {
            on_btn_Level3_clicked();
        } else if (ui->btn_Level4->hasFocus()) {
            on_btn_Level4_clicked();
        } else if (ui->btn_Next->hasFocus()) {
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
    default:
         qDebug() << "default";
         this->setFocus();
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
    if (obj == ui->btn_Level1) {
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
        }
        storeData(level);
        nextVehicle();
        break;
    case 1:
        // on_btn_modify_clicked();
        setTextEnabled(true);
        break;
    default:
        break;
    }
}

void WasteRecycle::clearData()
{
    ui->le_Level1ExtraPrice->clear();
    ui->le_Level4ExtraPrice->clear();
    ui->le_RoughWeigh->clear();
    ui->le_VehicleWeigh->clear();
    ui->lb_Price->clear();
    ui->lb_unitPrice->clear();
}

void WasteRecycle::nextVehicle()
{
    // 当未填毛重时，忽略该次点击操作
    /*if("" == ui->le_RoughWeigh->text()) {
        qWarning() << "le_RoughWeigh is empty";
        return;
    }*/

    // 当前索引值递增, 并显示下一辆车的索引值
    if(ui->lb_CurrNum->text().toInt() == toBeUseIndex) {
        ++toBeUseIndex;
    }

    clearData();
    ui->lb_CurrNum->setText(QString::fromStdString(std::to_string(toBeUseIndex)));

    setTextEnabled(true);
    ui->le_RoughWeigh->setFocus();

    model->sort(0);
    float ps = (fLevel2-fLevel3)/(fLevel1-fLevel3)  * 100;
    int pos = ps;
    bPriceInit = true;
    ui->vslider_percent->setValue(pos);
    ui->lb_percent->setText(QString("%1").arg(fLevel2));
}

void WasteRecycle::deleteData()
{
    QString date = ui->dateEdit->text().replace("-", "_");
    int row = ui->tableView->currentIndex().row();
    qDebug() << "row: " << row << " data:" << model->index(row, 0).data().toString() << "  currNum:" << ui->lb_CurrNum->text();
//    if (model->index(row, 0).data().toString() == ui->lb_CurrNum->text()) {
//        model->removeRow(row);
//    }
    if (bModify == false && model->index(row, 0).data().toString() == ui->lb_CurrNum->text()) {
        oper->sqlDeleteById(model->index(row, 0).data().toString(), date);
        model->removeRow(row);
    } else if (bModify == true) {
        oper->sqlDeleteById(model->index(row, 0).data().toString(), date);
        model->removeRow(row);
    }
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

        //     a. 对每一天的数据进行统计
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
    // qDebug() << "weighBridgeData:" << QString(weighBridgeData);
    if(weighBridgeData.contains("\n")) {
        weighBridgeData.clear();
    }
    if(weighBridgeData.length()>12) {
        QString qqba = QString(weighBridgeData);
        qDebug() << "qqba:" << qqba;
        qqba = qqba.mid(qqba.indexOf("n")+1, 8);
        if(fWeight != qqba.toFloat()) {
            m_pTimer->stop();
            fWeight = qqba.toFloat();
            ui->lb_display->setText(QString("%1").arg(fWeight));

            // 设置1分钟超时
            m_pTimer->start(1000*60*1);
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
    fLevel1 = priceSetWin->getAPrice();
    fLevel2 = priceSetWin->getBPrice();
    fLevel3 = priceSetWin->getCPrice();
    fLevel4 = priceSetWin->getDPrice();
}

void WasteRecycle::finalPrice(float level)
{
    qDebug() << "finalPrice IN";
    ui->lb_Price->clear();
    if (ui->le_VehicleWeigh->text().trimmed() == "") {
        qDebug() << "level: " << level << " VehicleWeigh is empty.";
        return;
    }
    ui->lb_Price->setText(towDecimalPlaces(QString("%1").arg(priceCalculate(level))));
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

    QDateTime qdtTime =QDateTime::currentDateTime();
    info.m_time = qdtTime.toString("yyyy-MM-dd hh:mm:ss");

    QString date = ui->dateEdit->text().replace("-", "_");
    oper->sqlInsert(info, date);
    qDebug() << "writeData OUT";
}

void WasteRecycle::updateListWidget(float level)
{
    // int num = ui->lw_history->count();

//    QDateTime qtime = QDateTime::currentDateTime();
//    QString time = qtime.toString("yyyy-MM-dd hh:mm:ss");
//    QString record = ui->lb_CurrNum->text() + UtilityTools::holdPlaces(5) + time + UtilityTools::holdPlaces(5) +
//            ui->le_RoughWeigh->text() + UtilityTools::holdPlaces(15) + ui->le_VehicleWeigh->text() + UtilityTools::holdPlaces(15) +
//            ui->lb_NetWeight->text() + UtilityTools::holdPlaces(15) + towDecimalPlaces(QString("%1").arg(level)) + UtilityTools::holdPlaces(15) +
//            ui->lb_Price->text();
//    ui->lw_history->addItem(record);
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
    ui->le_Level1ExtraPrice->setEnabled(bValue);
    ui->le_Level4ExtraPrice->setEnabled(bValue);
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
    int amount = 0;
    int rows = model->rowCount();
    for (int i =  0; i < rows ; ++i) {
        totalWeight += model->index(i, 4).data().toFloat();
        totalPrice += model->index(i, 6).data().toFloat();
        if (model->index(i, 6).data().toString().trimmed() != "") {
            ++amount;
        }
    }

    averagePrice = (totalWeight==0 ? 0 :  (totalPrice / totalWeight) * 2000);
    ui->lb_Amount->setText(QString("%1").arg(amount));
    ui->lb_TotalWeight->setText(QString("%1").arg(totalWeight));
    ui->lb_TotalPrices->setText(QString("%1").arg(totalPrice));
    ui->lb_AveragePrice->setText(QString("%1").arg(averagePrice));
    qDebug() << "statistics OUT";
}

void WasteRecycle::storeData(float level) {
    writeData(level);
    // updateListWidget(level);
    updateTableView(level);
    oper->sqlDeleteUnloadingByIdx(ui->lb_CurrNum->text());
}

void WasteRecycle::on_btn_Level1_clicked()
{
    qDebug() << "on_btn_Level1_clicked IN";
    if(!check()) {
        return;
    }

    on_btn_modify_clicked();
    updatePrice();
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
    on_btn_modify_clicked();
    updatePrice();
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
    on_btn_modify_clicked();
    updatePrice();
    finalPrice(fLevel3);
//    writeData(fLevel3);
//    // updateListWidget(fLevel3);
//    updateTableView(fLevel3);
//    oper->sqlDeleteUnloadingByIdx(ui->lb_CurrNum->text());
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
    on_btn_modify_clicked();
    updatePrice();
    QString ePrice = ui->le_Level4ExtraPrice->text();
    float level = ePrice != "" ? (fLevel4+ePrice.toInt()/100.0) : fLevel4;
    finalPrice(level);
//    writeData(level);
//    // updateListWidget(level);
//    updateTableView(level);
//    oper->sqlDeleteUnloadingByIdx(ui->lb_CurrNum->text());
    ui->lb_unitPrice->setText(QString("%1").arg(level));
    setTextEnabled(false);
    showPrice(level);
    qDebug() << "on_btn_Level4_clicked OUT";
}

void WasteRecycle::on_btn_Next_clicked()
{
    qDebug() << "on_btn_Next_clicked IN";
    // QString qstrText = ui->lb_Price->text();
    if(ui->le_RoughWeigh->text().trimmed() == "") {
        qWarning() << "le_RoughWeigh is empty";
        return;
    }
    if (ui->lb_Price->text().trimmed() == "") {
        bModify = false;
        deleteData();
//        writeData(0);
//        updateTableView(0);
//        oper->sqlDeleteUnloadingByIdx(ui->lb_CurrNum->text());
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


void WasteRecycle::on_btn_PriceSet_clicked()
{
    qDebug() << "on_btn_PriceSet_clicked IN";
    priceSetWin->updatePrices();
    priceSetWin->exec();
    qDebug() << "on_btn_PriceSet_clicked OUT";
}

void WasteRecycle::on_le_VehicleWeigh_textChanged(const QString &arg1)
{
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
    }

    if (isAllDigit && strVw.trimmed() != "") {
        ui->lb_NetWeight->setText(towDecimalPlaces(QString("%1").arg((fRw - fVw)*2)));
    } else {
        ui->lb_NetWeight->setText("");
    }
}

void WasteRecycle::on_btn_modify_clicked()
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
}

void WasteRecycle::on_vslider_percent_valueChanged(int value)
{
    if (bPriceInit) {
        bPriceInit = false;
        return;
    }
    qDebug() << "on_vslider_percent_valueChanged IN";
    // updatePrice();
    int pos = ui->vslider_percent->value();
    // float level = pos > 50 ? (fLevel2 + (fLevel1 - fLevel2)*(pos - 50)/50) : (fLevel2 - (fLevel2 - fLevel3)*(50 - pos)/50);
    float level = (pos/100.0) * (fLevel1 - fLevel3) + fLevel3;
    QString uPrice = QString("%1").arg(level);
    ui->lb_percent->setText(towDecimalPlaces(uPrice));
    ui->le_RoughWeigh->setFocus();
    qDebug() << "on_vslider_percent_valueChanged OUT";
}

void WasteRecycle::on_le_RoughWeigh_editingFinished()
{
    QString idx = ui->lb_CurrNum->text();
    QString rW = ui->le_RoughWeigh->text();
    if (rW != "" && idx.toInt() == toBeUseIndex) {
        qDebug() << "rWeight = " << ui->le_RoughWeigh->text();
        oper->sqlInsertUnloading(idx, rW);
    }
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
    } else {
        ui->lb_Amount->setHidden(true);
        ui->lb_Amount0->setHidden(true);
        ui->lb_AveragePrice->setHidden(true);
        ui->lb_AveragePrice0->setHidden(true);
        ui->lb_TotalPrices->setHidden(true);
        ui->lb_TotalPrices0->setHidden(true);
        ui->lb_TotalWeight->setHidden(true);
        ui->lb_TotalWeight0->setHidden(true);
    }
    qDebug() << "on_btn_Statistics_clicked OUT";
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

    // ui->lb_CurrNum->setText(currIndex);

    // on_btn_Next_clicked();

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
}

void WasteRecycle::saveCurrentData()
{
    if (ui->lb_Price->text().trimmed() == "" && ui->lb_CurrNum->text().toInt() == toBeUseIndex && ui->le_RoughWeigh->text() != "") {
        writeData(0);
        updateTableView(0);
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



    QString msg = "<font size='25' color='black'>" +QString::fromLocal8Bit("修改或者删除数据？") + "</font>";
    int ret = QMessageBox::information(this,
                                                               QString::fromLocal8Bit("警告"),
                                                               msg,
                                                               QString::fromLocal8Bit("修改"),
                                                               QString::fromLocal8Bit("删除"),
                                                               QString::fromLocal8Bit("取消"));

    switch (ret) {
    case 0:
        saveCurrentData();
        dataRecoveryFromTableView(index);
        on_btn_modify_clicked();
        break;
    case 1:
        bModify = true;
        deleteData();
        break;
    case 2:
        qDebug() << "cancel";
        break;
    default:
        break;
    }


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
    model->clear();
    initTableView();
    std::list<QString> records;
    QString dat = ui->dateEdit->text().replace("-", "_");
    records = oper->sqlQueryByDate(dat);
    qDebug() << "records size:" <<  records.size();
    int i = 0;

    for (auto itor = records.begin(); i < records.size(); ++i, ++itor) {
        qDebug() << "1 *itor=" << (*itor);
        QStringList lines = itor->split("  ");
        for(int j = 0; j < lines.length(); ++j) {
            // qDebug() << "i=" << i << "  j=" << j << "  data=" << lines.at(j).trimmed();
            model->setItem(i, j, new QStandardItem(lines.at(j).trimmed()));
        }
    }
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
    QLineSeries *series = new QLineSeries();

    T max = data.begin()->second;
    T min = data.begin()->second;
    for (auto element : data) {
        max = MAX(max, element.second);
        min = MIN(min, element.second);
        series->append(element.first.toMSecsSinceEpoch(), element.second);
    }

    QChartView *chartView = new QChartView();
    chartView->chart()->addSeries(series);

    // ...
    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("dd/MM/yyyy");
    auto it = data.rbegin();
    ++it;
    int days = data.begin()->first.daysTo(it->first);
    ++days;
    int param = (5 - days%5);
    param = (param==5 ? 0:param);

    axisX->setRange(data.begin()->first, it->first.addDays(param+1));
    axisX->setTickCount((days+param)/5 + 1);
    qDebug() << "days=" << days << " param=" << param;

    chartView->chart()->setAxisX(axisX, series);

    QValueAxis* axisY = new QValueAxis();

    int coefficient = 1;
    dataProcess(max, coefficient, 10);
    int top = (int)(max+1)*coefficient;
    coefficient = 1;
    dataProcess(min, coefficient, 10);
    int bottom = (int)(min)*coefficient;

    axisY->setRange(bottom, top);
    chartView->chart()->setAxisY(axisY, series);

    chartView->resize(800, 500);
    chartView->chart()->setAnimationDuration(QChart::SeriesAnimations);
    // chartView->chart()->createDefaultAxes();
    chartView->chart()->setTitle(QString::fromLocal8Bit("均价走势图"));
    chartView->chart()->legend()->hide();
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->show();
}

void WasteRecycle::on_btn_charts_clicked()
{
    // parType(std::pair<QString, int>("name", 1),2,3, QString("hello"));

    // 更新charts数据表
    updateTableCharts();

    // 查询charts表中的所有记录
    std::list<QString> elements;
    QString queryChartsTable = "select * from charts order by time asc;";
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
    QString queryChartsTable = "select * from charts order by time asc;";
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
    updatePrice();
    ui->lb_percent->setText(QString("%1").arg(fLevel2));

    float ps = (fLevel2-fLevel3)/(fLevel1-fLevel3)  * 100;
    int pos = ps;
    bPriceInit = true;
    ui->vslider_percent->setValue(pos);
}

void WasteRecycle::openSerialPort()
{
    QString port;
    if(ui->le_com->text() != "") {
        port = "COM" + ui->le_com->text();
    } else {
        port = "COM4";
    }
    m_serial->setPortName(port);
    m_serial->setBaudRate(1200);
    m_serial->open(QIODevice::ReadWrite);
}

void WasteRecycle::handleTimeout()
{
    m_pTimer->stop();
    // ui->lb_display->setText(QString::fromLocal8Bit("金龙纸业"));
    QPixmap myPix = QPixmap(":/images/mimeiti_da.png").scaled(425, 97);
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
}

void WasteRecycle::on_btn_vWrite_clicked()
{
    QString data = ui->lb_display->text();
    if(data != QString::fromLocal8Bit("金龙纸业") && data != "") {
        float tmp = data.toFloat();
        ui->le_VehicleWeigh->setText(QString("%1").arg(tmp));
    }
}

void WasteRecycle::on_btn_rClear_clicked()
{
    ui->le_RoughWeigh->clear();
    ui->le_RoughWeigh->setFocus();
}

void WasteRecycle::on_btn_vClear_clicked()
{
    ui->le_VehicleWeigh->clear();
     ui->le_VehicleWeigh->setFocus();
}

void WasteRecycle::on_le_com_textChanged(const QString &arg1)
{
    m_serial->close();
    openSerialPort();
    QPixmap myPix = QPixmap(":/images/mimeiti_da.png").scaled(425, 97);
    ui->lb_display->setPixmap(myPix);
    ui->lb_display->show();
}
