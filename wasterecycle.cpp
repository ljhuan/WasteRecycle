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
#include <QKeyEvent>
#include <Qt>

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
    default:
         qDebug() << "default";
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
            ui->btn_Level4->setText(QString::fromLocal8Bit("居民用纸"));
        }
    }

    return false;
}

void WasteRecycle::showPrice()
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
        nextVehicle();
        break;
    case 1:
        on_btn_modify_clicked();
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
    ui->vslider_percent->setValue(50);
}

void WasteRecycle::deleteData()
{
    oper->sqlDeleteById(ui->lb_CurrNum->text());
    int row = ui->tableView->currentIndex().row();
    qDebug() << "row: " << row << " data:" << model->index(row, 0).data().toString() << "  currNum:" << ui->lb_CurrNum->text();
    if (model->index(row, 0).data().toString() == ui->lb_CurrNum->text()) {
        model->removeRow(row);
    }
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

    for (int i = 0; i < records.size(); ++i) {
        static auto itor = records.begin();
        QStringList lines = itor->split("  ");
        ++itor;
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

    ui->vslider_percent->setMaximum(100);
    ui->vslider_percent->setMinimum(0);
    ui->vslider_percent->setValue(50);
    ui->lb_percent->setText(QString("%1").arg(fLevel2));

    // 配置背景图
    QPixmap pixmap = QPixmap( ":/images/1.jpg").scaled(this->size());
    QPalette  palette (this->palette());
    palette .setBrush(QPalette::Background, QBrush(pixmap));
    this->setPalette( palette );

    // 设置默认的焦点
    ui->le_RoughWeigh->setFocus();

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
    showPrice();
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
    showPrice();
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
    showPrice();
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
    showPrice();
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
        deleteData();
        writeData(0);
        updateTableView(0);
        oper->sqlDeleteUnloadingByIdx(ui->lb_CurrNum->text());
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

    float fRw = strRw.toFloat();
    float fVw = strVw.toFloat();

    if(fRw < fVw) {
        ui->lb_NetWeight->setText("");
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("毛重或者车重数据错误!"), QString::fromLocal8Bit("关闭"));
        qWarning() << "le_RoughWeigh or le_VehicleWeigh data is wrong.";
        return;
    }

    // ui->lb_NetWeight->setText(towDecimalPlaces(QString::fromStdString(std::to_string((fRw-fVw)*2))));
    if (strVw.trimmed() != QString::fromLocal8Bit("卸货中")) {
        if ( strVw.trimmed() != "") {
            ui->lb_NetWeight->setText(towDecimalPlaces(QString("%1").arg((fRw - fVw)*2)));
        } else {
            ui->lb_NetWeight->setText("");
        }
    }
}

void WasteRecycle::on_btn_modify_clicked()
{
    qDebug() << "on_btn_modify_clicked IN";
    setTextEnabled(true);
    oper->sqlDeleteById(ui->lb_CurrNum->text());
    int row = ui->tableView->currentIndex().row();
    qDebug() << "row:" << row << " data:" << model->index(row, 0).data().toString() << "   currNum:" << ui->lb_CurrNum;
    if (model->index(row, 0).data().toString() == ui->lb_CurrNum->text()) {
        model->removeRow(row);
    }
    ui->lb_Price->clear();

    qDebug() << "on_btn_modify_clicked OUT";
}

void WasteRecycle::on_vslider_percent_valueChanged(int value)
{
    updatePrice();
    int pos = ui->vslider_percent->value();
    float level = pos > 50 ? (fLevel2 + (fLevel1 - fLevel2)*(pos - 50)/50) : (fLevel2 - (fLevel2 - fLevel3)*(50 - pos)/50);
    QString uPrice = QString("%1").arg(level);
    ui->lb_percent->setText(towDecimalPlaces(uPrice));
    ui->le_RoughWeigh->setFocus();
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
    if (model->index(index.row(), 3).data().toString().trimmed() == QString::fromLocal8Bit("卸货中")) {
        ui->le_VehicleWeigh->setFocus();
        ui->le_VehicleWeigh->selectAll();
    } else {
        setTextEnabled(false);
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
