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

QString towDecimalPlaces(QString data) {
    QString head = data.split('.').at(0);
    QString tail;
    if (data.split('.').size() > 1) {
        tail = data.split('.').at(1);
        tail.resize(2);
    } else {
        tail = "00";
    }
    return head + '.' + tail;
}

/*QString holdPlaces(int num) {
    QString places = "";
    for (int i = 0; i < num; ++i) {
        places += " ";
    }
    return places;
}*/

WasteRecycle::WasteRecycle(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WasteRecycle),
    toBeUseIndex(100),
    oper(new SqlOper),
    priceSetWin(nullptr),
    fLevel1(0.0),
    fLevel2(0.0),
    fLevel3(0.0),
    fLevel4(0.0)
    // bModifyFlag(false)
{
    priceSetWin = new PriceSetDialog(parent, oper);
    ui->setupUi(this);
    ui->le_RoughWeigh->clear();
    ui->le_VehicleWeigh->clear();

    QDateTime date = QDateTime::currentDateTime();
    QString today = date.toString("yyyy_MM_dd");
    std::list<QString> records;
    records = oper->sqlQueryByDate(today);
    qDebug() << "records size:" <<  records.size();
    // QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
    // QString items("号码                  时间                     毛重    车重   净重   单价   总价");
    // ui->lw_history->addItem(items);
    for (auto e : records) {
        ui->lw_history->addItem(e);
        QString index = e.split(" ").at(0);
        toBeUseIndex = index.toInt() + 1;
    }
    ui->lb_CurrNum->setText(QString::fromStdString(std::to_string(toBeUseIndex)));
    // ui->cbb_NumSwitch->setCurrentText(QString::fromStdString(std::to_string(currentIndex)));
//    for(auto e : unloadingClientList)
//    {
//        ui->cbb_CurrentNum->addItem(QString::fromStdString(std::to_string(e.m_num)));
//    }

    // 增加一个定时器，用于每日索引值归0
    ui->vslider_percent->setMaximum(100);
    ui->vslider_percent->setMinimum(0);
    ui->vslider_percent->setValue(50);
    // int pos = ui->vslider_percent->value();
    // ui->lb_percent->setText(QString("%1").arg(pos));
    ui->lb_percent->setText(QString("%1").arg(fLevel2));
    connect(priceSetWin, SIGNAL(finished(int)), this, SLOT(on_vslider_percent_valueChanged(int)));
}

WasteRecycle::~WasteRecycle()
{
    delete ui;
    ui = nullptr;
    delete priceSetWin;
    priceSetWin = nullptr;
}

float WasteRecycle::priceCalculate(float level)
{
    // parameter available check
    // CheckParameter();
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
}

// 写数据库（排队号码, 时间， 毛重， 车重， 单价， 总价）
void WasteRecycle::writeData(float level)
{
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
}

void WasteRecycle::updateListWidget(float level)
{
    // int num = ui->lw_history->count();

    QDateTime qtime = QDateTime::currentDateTime();
    QString time = qtime.toString("yyyy-MM-dd hh:mm:ss");
    QString record = ui->lb_CurrNum->text() + UtilityTools::holdPlaces(5) + time + UtilityTools::holdPlaces(5) +
            ui->le_RoughWeigh->text() + UtilityTools::holdPlaces(15) + ui->le_VehicleWeigh->text() + UtilityTools::holdPlaces(15) +
            ui->lb_NetWeight->text() + UtilityTools::holdPlaces(15) + towDecimalPlaces(QString("%1").arg(level)) + UtilityTools::holdPlaces(15) +
            ui->lb_Price->text();
    ui->lw_history->addItem(record);
}

bool WasteRecycle::check()
{
    if (ui->le_RoughWeigh->text() == "" || ui->le_VehicleWeigh->text() == "") {
        QMessageBox::warning(this, QObject::tr("提醒"), QObject::tr("毛重或者车重未填写!"), QObject::tr("关闭"));
        return false;
    }
    return true;
}



void WasteRecycle::on_btn_Level1_clicked()
{
    if(!check()) {
        return;
    }
    updatePrice();
    QString ePrice = ui->le_ExtraPrice->text();
    float level = ePrice != "" ? (fLevel1+ePrice.toInt()/100.0) : fLevel1;
    finalPrice(level);
    writeData(level);
    updateListWidget(level);
    ui->lb_unitPrice->setText(QString("%1").arg(level));
    ui->le_RoughWeigh->setDisabled(true);
    ui->le_VehicleWeigh->setDisabled(true);
}

void WasteRecycle::on_btn_Level2_clicked()
{
    if(!check()) {
        return;
    }
    updatePrice();
    float level = (fLevel1*(ui->vslider_percent->value()) + fLevel3*(100 - ui->vslider_percent->value())) / 100;
    finalPrice(level);
    writeData(level);
    updateListWidget(level);
    ui->lb_unitPrice->setText(ui->lb_percent->text());
    ui->le_RoughWeigh->setDisabled(true);
    ui->le_VehicleWeigh->setDisabled(true);
}

void WasteRecycle::on_btn_Level3_clicked()
{
    if(!check()) {
        return;
    }
    updatePrice();
    finalPrice(fLevel3);
    writeData(fLevel3);
    updateListWidget(fLevel3);
    ui->lb_unitPrice->setText(QString("%1").arg(fLevel3));
    ui->le_RoughWeigh->setDisabled(true);
    ui->le_VehicleWeigh->setDisabled(true);
}


void WasteRecycle::on_btn_Level4_clicked()
{
    if(!check()) {
        return;
    }
    updatePrice();
    finalPrice(fLevel4);
    writeData(fLevel4);
    updateListWidget(fLevel4);
    ui->lb_unitPrice->setText(QString("%1").arg(fLevel4));
    ui->le_RoughWeigh->setDisabled(true);
    ui->le_VehicleWeigh->setDisabled(true);
}

void WasteRecycle::on_btn_Next_clicked()
{
    QString qstrText = ui->lb_Price->text();

    // 当未填毛重时，忽略该次点击操作
    if("" == ui->le_RoughWeigh->text()) {
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

//    for (auto e : unloadingClientMap) {
//        qDebug() << e.first;
//    }

    // 当前索引值递增, 并显示下一辆车的索引值
    ++toBeUseIndex;
    ui->le_RoughWeigh->clear();
    ui->le_VehicleWeigh->clear();
    ui->lb_CurrNum->setText(QString::fromStdString(std::to_string(toBeUseIndex)));

    currentClient.m_num = toBeUseIndex;
    currentClient.m_qstrRw = ui->le_RoughWeigh->text();
    currentClient.m_qstrVw = ui->le_VehicleWeigh->text();

    ui->lb_Price->clear();
    ui->le_RoughWeigh->setDisabled(false);
    ui->le_VehicleWeigh->setDisabled(false);
    ui->lw_history->sortItems();
    ui->vslider_percent->setValue(50);
}


void WasteRecycle::on_cbb_NumSwitch_activated(const QString &arg1)
{
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
    ui->le_RoughWeigh->setEnabled(true);
    ui->le_VehicleWeigh->setEnabled(true);
}


void WasteRecycle::on_btn_PriceSet_clicked()
{
    // retry
    if (priceSetWin == nullptr) {
        priceSetWin = new PriceSetDialog;
    }

    priceSetWin->show();
}

void WasteRecycle::on_le_VehicleWeigh_textChanged(const QString &arg1)
{
    QString strRw = ui->le_RoughWeigh->text();
    QString strVw = ui->le_VehicleWeigh->text();

    float fRw = strRw.toFloat();
    float fVw = strVw.toFloat();

    // ui->lb_NetWeight->setText(towDecimalPlaces(QString::fromStdString(std::to_string((fRw-fVw)*2))));
    ui->lb_NetWeight->setText(towDecimalPlaces(QString("%1").arg((fRw - fVw)*2)));
}

void WasteRecycle::on_btn_modify_clicked()
{
    ui->le_RoughWeigh->setEnabled(true);
    ui->le_VehicleWeigh->setEnabled(true);
    oper->sqlDeleteById(ui->lb_CurrNum->text());
    if (ui->lb_Price->text() != "") {
        int num = ui->lw_history->count();
        ui->lw_history->takeItem(num - 1);
        ui->lb_Price->clear();
        ui->lb_unitPrice->clear();
    }

    // bModifyFlag = true;
}

void WasteRecycle::on_vslider_percent_valueChanged(int value)
{
    updatePrice();
    int pos = ui->vslider_percent->value();
    // float level = (fLevel1*pos + fLevel3*(100 - pos)) / 100;
    float level = pos > 50 ? (fLevel2 + (fLevel1 - fLevel2)*(pos - 50)/50) : (fLevel2 - (fLevel2 - fLevel3)*(50 - pos)/50);
    QString uPrice = QString("%1").arg(level);
    ui->lb_percent->setText(towDecimalPlaces(uPrice));
    // ui->lb_unitPrice->setText(towDecimalPlaces(uPrice));
}
