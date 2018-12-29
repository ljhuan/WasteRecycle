#ifndef SQLOPER_H
#define SQLOPER_H

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QString>
#include <QWidget>
#include <list>
//#include <typeinfo>

//#define VNAME(value)  (#value)

// 需要写入的信息有排队号码, 时间， 毛重， 车重， 净重， 价格， 总价
class RecordInfo
{
public:
    RecordInfo(): m_index(""), m_time(""), m_rWeight(""), m_vWeight(""),
        m_nWeight(""), m_unitPrice(""), m_price(""){}
    QString m_index;  // 当日排队号
    QString m_time;  // 回磅算价时间
    QString m_rWeight;  // 毛重
    QString m_vWeight;  // 车重
    QString m_nWeight;  // 净重
    QString m_unitPrice;  // 单价
    QString m_price;  // 总价
};

class PriceInfo
{
public:
    PriceInfo(): m_factoryPrice1(""), m_factoryPrice2(""), m_factoryPrice3(""),
        m_price1(""), m_price2(""), m_price3(""), m_price4("") {}
    QString m_factoryPrice1;  // 工厂A级黄板纸价格
    QString m_factoryPrice2;  // 工厂超市纸价格
    QString m_factoryPrice3;  // 工厂普通黄板纸价格
    QString m_price1;  // 黄板纸价格
    QString m_price2;  // 超市纸价格
    QString m_price3;  // 统货纸价格
    QString m_price4;  // 居民用纸价格
};

class SqlOper : public QWidget
{
    Q_OBJECT
public:
    explicit SqlOper(QWidget *parent = nullptr);

    // 数据插入（数据插入前判断是否新的一天，若是新的一天则需要新建数据表， 用record_日期为表格名）
    void sqlInsert(RecordInfo info, QString date);

    // 数据重写 date格式:xxxx_xx_xx
    void sqlDeleteById(QString index, QString date);

    // 数据查询（根据时间进行查询）
    std::list<QString> sqlQueryByDate(QString date);

    // 价格查询
    void sqlPriceQuery(PriceInfo & priceInfo);

    // 价格设置
    void sqlPriceSet(PriceInfo & priceInfo);

    // 正在卸货的车辆数据写入卸货表
    void sqlInsertUnloading(QString idx, QString rWeight);

    // 将已结算的车辆从正在卸货车辆的表中删除
    void sqlDeleteUnloadingByIdx(QString idx);

    // 判断该辆车是否已被写入
    bool sqlQueryUnloadingIdxIsExist(QString idx, QString tableName);

    // 查询正在卸货的车辆
    std::list<QString> sqlQueryUnloadingByDate(QString date);

    // 查询所有表名
    QStringList sqlGetAllTableName();

    void createTable(QString sql);

    void insertTable(QString sql);

    bool searchTableWetherExist(QString tableName, QString item, QString value);

    std::list<QString> queryTableRecords(QString sql);

    std::list<QString> queryTableCharts(QString sql);

private:
    QSqlDatabase* pDb;
    QSqlQuery* pQuery;

signals:

public slots:
};

#endif // SQLOPER_H
