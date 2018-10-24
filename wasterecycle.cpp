#include <string>
#include <list>
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
    ui->tableView->setColumnWidth(1, 150);
    ui->tableView->setColumnWidth(2, 70);
    ui->tableView->setColumnWidth(3, 70);
    ui->tableView->setColumnWidth(4, 70);
    ui->tableView->setColumnWidth(5, 50);
    ui->tableView->setColumnWidth(6, 70);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);  // 设置选中模式为选中行
    ui->tableView->setSelectionMode( QAbstractItemView::SingleSelection);  // 设置选中单个
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 设置不可编辑
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
    fLevel4(0.0)
{
    priceSetWin = new PriceSetDialog(parent, oper);
    ui->setupUi(this);
    ui->le_RoughWeigh->clear();
    ui->le_VehicleWeigh->clear();

    // 初始化tableView
    initTableView();

    QDateTime date = QDateTime::currentDateTime();
    QString today = date.toString("yyyy_MM_dd");
    std::list<QString> records, unloadings;
    records = oper->sqlQueryByDate(today);
    qDebug() << "records size:" <<  records.size();

    for (int i = 0; i < records.size(); ++i) {
        static auto itor = records.begin();
        QStringList lines = itor->split("  ", QString::SkipEmptyParts);
        ++itor;
        for(int j = 0; j < lines.length(); ++j) {
            model->setItem(i, j, new QStandardItem(lines.at(j)));
        }
        toBeUseIndex = lines.at(0).toInt() + 1;
    }

    /*for (auto e : records) {
        ui->lw_history->addItem(e);
        QString index = e.split(" ").at(0);
        if (index.toInt() >= toBeUseIndex) {
            toBeUseIndex = index.toInt() + 1;
        }
    }*/

    unloadings = oper->sqlQueryUnloadingByDate(today);
    for (auto e : unloadings) {
        QString idx = e.split(" ").at(0);
        QString rW = e.split(" ").at(1);
        ClientData data;
        data.m_num = idx.toInt();
        data.m_qstrRw = rW;
        unloadingClientMap[data.m_num] = data;
        ui->cbb_NumSwitch->addItem(idx);
        // ui->le_RoughWeigh->setText(rW);
        if (idx.toInt() >= toBeUseIndex) {
            toBeUseIndex = idx.toInt() + 1;
        }
    }

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

    ui->vslider_percent->setMaximum(100);
    ui->vslider_percent->setMinimum(0);
    ui->vslider_percent->setValue(50);
    ui->lb_percent->setText(QString("%1").arg(fLevel2));

    /*QScrollBar*  eventTableSlider = new QScrollBar(Qt::Vertical, this);
    eventTableSlider->setRange(0, 99);
    eventTableSlider->setPageStep(20);
    eventTableSlider->setSingleStep(1);
    ui->lw_history->setVerticalScrollBar(eventTableSlider);*/
    // ui->lw_history->setAutoScroll(true);
    // ui->lw_history->setVerticalScrollMode(QListWidget::ScrollPerPixel);
    // ui->vsb_history->setValue(10);
    // ui->lw_history->setVerticalScrollBar(ui->vsb_history);
    // ui->lw_history->scrollToBottom();

    // 配置背景图
    QPixmap pixmap = QPixmap( ":/images/1.jpg").scaled(this->size());
    QPalette  palette (this->palette());
    palette .setBrush(QPalette::Background, QBrush(pixmap));
    this->setPalette( palette );

    connect(priceSetWin, SIGNAL(finished(int)), this, SLOT(on_vslider_percent_valueChanged(int)));
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
    ui->lb_Price->setText(towDecimalPlaces(QString("%1").arg(priceCalculate(level))));
    QString qstrCurrIndex = ui->lb_CurrNum->text();
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
    }
    qDebug() << "finalPrice OUT";
}

// 写数据库（排队号码, 时间， 毛重， 车重， 单价， 总价）
void WasteRecycle::writeData(float level)
{
    qDebug() << "writeData IN";
    RecordInfo info;
    info.m_index = ui->lb_CurrNum->text();
    info.m_rWeight = ui->le_RoughWeigh->text();
    info.m_vWeight = ui->le_VehicleWeigh->text();
    info.m_nWeight = ui->lb_NetWeight->text();
    info.m_price = ui->lb_Price->text();
    info.m_unitPrice =  towDecimalPlaces(QString("%1").arg(level));

    QDateTime qdtTime =QDateTime::currentDateTime();
    info.m_time = qdtTime.toString("yyyy-MM-dd hh:mm:ss");

    oper->sqlInsert(info);
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
    model->setItem(row, 3, new QStandardItem(ui->le_VehicleWeigh->text()));
    model->setItem(row, 4, new QStandardItem(ui->lb_NetWeight->text()));
    model->setItem(row, 5, new QStandardItem(towDecimalPlaces(QString("%1").arg(level))));
    model->setItem(row, 6, new QStandardItem(ui->lb_Price->text()));
    ui->tableView->selectRow(row);
    qDebug() << "updateTableView OUT";
}

bool WasteRecycle::check()
{
    if (ui->le_RoughWeigh->text() == "" || ui->le_VehicleWeigh->text() == "") {
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
    int amount = model->rowCount();
    ui->lb_Amount->setText(QString("%1").arg(amount));
    for (int i =  0; i < amount ; ++i) {
        totalWeight += model->index(i, 4).data().toFloat();
        totalPrice += model->index(i, 6).data().toFloat();
    }

    averagePrice = (totalWeight==0 ? 0 :  (totalPrice / totalWeight) * 2000);
    ui->lb_TotalWeight->setText(QString("%1").arg(totalWeight));
    ui->lb_TotalPrices->setText(QString("%1").arg(totalPrice));
    ui->lb_AveragePrice->setText(QString("%1").arg(averagePrice));
    qDebug() << "statistics OUT";
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
    writeData(level);
    // updateListWidget(level);
    updateTableView(level);
    oper->sqlDeleteUnloadingByIdx(ui->lb_CurrNum->text());
    ui->lb_unitPrice->setText(QString("%1").arg(level));
    setTextEnabled(false);
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
    writeData(level);
    updateTableView(level);
    oper->sqlDeleteUnloadingByIdx(ui->lb_CurrNum->text());
    ui->lb_unitPrice->setText(ui->lb_percent->text());
    // ui->le_RoughWeigh->setDisabled(true);
    // ui->le_VehicleWeigh->setDisabled(true);
    setTextEnabled(false);
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
    writeData(fLevel3);
    // updateListWidget(fLevel3);
    updateTableView(fLevel3);
    oper->sqlDeleteUnloadingByIdx(ui->lb_CurrNum->text());
    ui->lb_unitPrice->setText(QString("%1").arg(fLevel3));
    // ui->le_RoughWeigh->setDisabled(true);
    // ui->le_VehicleWeigh->setDisabled(true);
    setTextEnabled(false);
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
    writeData(level);
    // updateListWidget(level);
    updateTableView(level);
    oper->sqlDeleteUnloadingByIdx(ui->lb_CurrNum->text());
    ui->lb_unitPrice->setText(QString("%1").arg(level));
    // ui->le_RoughWeigh->setDisabled(true);
    // ui->le_VehicleWeigh->setDisabled(true);
    setTextEnabled(false);
    qDebug() << "on_btn_Level4_clicked OUT";
}

void WasteRecycle::on_btn_Next_clicked()
{
    qDebug() << "on_btn_Next_clicked IN";
    QString qstrText = ui->lb_Price->text();

    // 当未填毛重时，忽略该次点击操作
    if("" == ui->le_RoughWeigh->text()) {
        qWarning() << "le_RoughWeigh is empty";
        return;
    }

    // 当价格尚未计算时，需要将该车辆登记入正在卸货车辆map中
    if ("" == qstrText) {
        currentClient.m_num = toBeUseIndex;
        currentClient.m_qstrRw = ui->le_RoughWeigh->text();
        currentClient.m_qstrVw = ui->le_VehicleWeigh->text();
        unloadingClientMap[currentClient.m_num] = currentClient;

        // 当下拉列表中不存在一样的索引值时 才将索引值添加进下拉列表
        if (ui->cbb_NumSwitch->findText(QString::fromStdString(std::to_string(currentClient.m_num))) == -1) {
            ui->cbb_NumSwitch->addItem(QString::fromStdString(std::to_string(currentClient.m_num)));
        }
    }

    // 当前索引值递增, 并显示下一辆车的索引值
    ++toBeUseIndex;
    ui->le_RoughWeigh->clear();
    ui->le_VehicleWeigh->clear();
    ui->le_Level1ExtraPrice->clear();
    ui->le_Level4ExtraPrice->clear();
    ui->lb_CurrNum->setText(QString::fromStdString(std::to_string(toBeUseIndex)));

    currentClient.m_num = toBeUseIndex;
    currentClient.m_qstrRw = ui->le_RoughWeigh->text();
    currentClient.m_qstrVw = ui->le_VehicleWeigh->text();

    ui->lb_Price->clear();
    ui->lb_unitPrice->clear();
    /* ui->le_RoughWeigh->setDisabled(false);
    ui->le_VehicleWeigh->setDisabled(false); */
    setTextEnabled(true);
    // ui->lw_history->sortItems();
    model->sort(0);
    ui->vslider_percent->setValue(50);
    qDebug() << "on_btn_Next_clicked OUT";
}


void WasteRecycle::on_cbb_NumSwitch_activated(const QString &arg1)
{
    qDebug() << "on_cbb_NumSwitch_activated IN [arg1=" << arg1 << "]";
    if (ui->le_RoughWeigh->text() == "" && ui->lb_CurrNum->text().toInt() == toBeUseIndex) {
        --toBeUseIndex;
    } else if(ui->lb_CurrNum->text().toInt()  == toBeUseIndex) {
        QString qstrText = ui->lb_Price->text();

        // 当价格尚未计算时，需要将该车辆登记入正在卸货车辆map中
        if ("" == qstrText) {
            currentClient.m_num = toBeUseIndex;
            currentClient.m_qstrRw = ui->le_RoughWeigh->text();
            currentClient.m_qstrVw = ui->le_VehicleWeigh->text();
            unloadingClientMap[currentClient.m_num] = currentClient;

            // 当下拉列表中不存在一样的索引值时 才将索引值添加进下拉列表
            if (ui->cbb_NumSwitch->findText(QString::fromStdString(std::to_string(currentClient.m_num))) == -1) {
                ui->cbb_NumSwitch->addItem(QString::fromStdString(std::to_string(currentClient.m_num)));
            }
        }
    }
    int index = arg1.toInt();
    ui->le_RoughWeigh->setText(unloadingClientMap[index].m_qstrRw);
    ui->le_VehicleWeigh->setText(unloadingClientMap[index].m_qstrVw);

    // 当前用户显示切换
    currentClient.m_num = unloadingClientMap[index].m_num;
    currentClient.m_qstrRw = unloadingClientMap[index].m_qstrRw;
    currentClient.m_qstrVw = unloadingClientMap[index].m_qstrVw;

    ui->lb_CurrNum->setText(QString::fromStdString(std::to_string(currentClient.m_num)));
    ui->lb_unitPrice->clear();
    ui->lb_Price->clear();
    setTextEnabled(true);
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

    float fRw = strRw.toFloat();
    float fVw = strVw.toFloat();

    if(fRw < fVw) {
        ui->lb_NetWeight->setText("");
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("毛重或者车重数据错误!"), QString::fromLocal8Bit("关闭"));
        qWarning() << "le_RoughWeigh or le_VehicleWeigh data is wrong.";
        return;
    }

    // ui->lb_NetWeight->setText(towDecimalPlaces(QString::fromStdString(std::to_string((fRw-fVw)*2))));
    ui->lb_NetWeight->setText(towDecimalPlaces(QString("%1").arg((fRw - fVw)*2)));
}

void WasteRecycle::on_btn_modify_clicked()
{
    qDebug() << "on_btn_modify_clicked IN";
    setTextEnabled(true);
    oper->sqlDeleteById(ui->lb_CurrNum->text());
    if (ui->lb_Price->text() != "") {
        /* int num = ui->lw_history->count();
        ui->lw_history->takeItem(num - 1);*/
        int row = ui->tableView->currentIndex().row();
        model->removeRow(row);
        ui->lb_Price->clear();
        ui->lb_unitPrice->clear();
    }

    qDebug() << "on_btn_modify_clicked OUT";
}

void WasteRecycle::on_vslider_percent_valueChanged(int value)
{
    updatePrice();
    int pos = ui->vslider_percent->value();
    float level = pos > 50 ? (fLevel2 + (fLevel1 - fLevel2)*(pos - 50)/50) : (fLevel2 - (fLevel2 - fLevel3)*(50 - pos)/50);
    QString uPrice = QString("%1").arg(level);
    ui->lb_percent->setText(towDecimalPlaces(uPrice));
}

void WasteRecycle::on_le_RoughWeigh_editingFinished()
{
    QString idx = ui->lb_CurrNum->text();
    QString rW = ui->le_RoughWeigh->text();
    if (rW != "") {
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

void WasteRecycle::on_tableView_doubleClicked(const QModelIndex &index)
{
    qDebug() << "on_tableView_doubleClicked IN";
    ui->tableView->selectRow(index.row());
    QString currIndex = model->index(index.row(), 0).data().toString();
    ui->lb_CurrNum->setText(currIndex);
    ui->le_RoughWeigh->setText(model->index(index.row(), 2).data().toString());
    ui->le_VehicleWeigh->setText(model->index(index.row(), 3).data().toString());
    ui->lb_NetWeight->setText(model->index(index.row(), 4).data().toString());
    ui->lb_unitPrice->setText(model->index(index.row(), 5).data().toString());
    ui->lb_Price->setText(model->index(index.row(), 6).data().toString());
    setTextEnabled(false);
    qDebug() << "on_tableView_doubleClicked OUT";
}
