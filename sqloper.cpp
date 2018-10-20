#include "sqloper.h"
#include <string>
#include <QString>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QSqlResult>
#include <QSqlError>
#include <utilitytools.h>

SqlOper::SqlOper(QWidget *parent) : QWidget(parent), pDb(nullptr)
{
    if (!pDb) {
        pDb = new QSqlDatabase();
    }

    *pDb = QSqlDatabase::addDatabase("QSQLITE");
    pDb->setDatabaseName("BusinessRecord.db");
    pDb->setUserName("ljhuan");
    pDb->setPassword("*Ab123456");
    bool bIsExist = false;
    bIsExist = QFile::exists("./BusinessRecord.db");
    if(!pDb->open()) {
        qDebug() << "SqlOper db open failed";
    } else {
        qDebug() << "SqlOper db open success";
        if(!bIsExist) {
            qDebug() << "db is not exist, so create it";
            pDb->exec("create table configure(id INTEGER PRIMARY KEY autoincrement, date varchar(50), fPrice_1 varchar(50), fPrice_2 varchar(50), fPrice_3 varchar(50),"
                      "price_1 varchar(50), price_2 varchar(50), price_3 varchar(50), price_4 varchar(50));");
            QSqlQuery query;
            query.prepare("insert into configure(date, fPrice_1, fPrice_2, fPrice_3, price_1, price_2, price_3, price_4) values(:date,:fPrice_1, :fPrice_2, :fPrice_3,:price_1,:price_2,:price_3,:price_4);");

            QDateTime date = QDateTime::currentDateTime();
            QString today = date.toString("yyyy-MM-dd hh:mm:ss");
            query.bindValue(":date", today);
            query.bindValue(":fPrice_1", "2350");
            query.bindValue(":fPrice_2", "0");
            query.bindValue(":fPrice_3", "2070");
            query.bindValue(":price_1", "1.1");
            query.bindValue(":price_2", "1.05");
            query.bindValue(":price_3", "1.0");
            query.bindValue(":price_4", "0.9");
            query.exec();
        } else {
            qDebug() << "db is exist";
        }
        pDb->close();
    }
}

void SqlOper::sqlInsert(RecordInfo info)
{
    // 若日期变化， 则新建一张数据表
    if(!pDb->open()) {
        qDebug() << "sqlInsert db open failed";
        return;
    }
    qDebug() << "sqlInsert db open success";

    QDateTime date = QDateTime::currentDateTime();
    QString today = date.toString("yyyy_MM_dd");
    QString tableName = "record_" + today;
    qDebug() << tableName;

    // 拼接sql语句
    QString qstrSqlJudgeTableExist = "select * from " + tableName + ";";
    // 需要写入的信息有排队号码, 时间， 毛重， 车重， 净重， 价格， 总价
    QString qstrSqlCreateTable = "create table " + tableName + "(id INTEGER PRIMARY KEY, time varchar(100), rWeight varchar(10), vWeight varchar(10), nWeight varchar(10), unitPrice varchar(10), price varchar(10));";
    QSqlQuery query(*pDb);
    query.exec(qstrSqlJudgeTableExist);

    // 如果存在这张表， 则不需要创建， 若不存在， 则新建一张表
    if(!query.isActive()) {
        pDb->exec(qstrSqlCreateTable);
    } else {
        query.clear();
    }

    QString qstrIndex = info.m_index;
    QString sqlSqlInsert = "insert into " + tableName + " values(" + qstrIndex + ", '" +  info.m_time + "'" + ", '" +  info.m_rWeight + "'"
            + ", '" +  info.m_vWeight + "'" + ", '" +  info.m_nWeight + "'"+ ", '" +  info.m_unitPrice + "'"+ ", '" +  info.m_price + "'"+  ");";
    qDebug() << sqlSqlInsert;
    query.exec(sqlSqlInsert);
    query.finish();
    qDebug() << "write to db";
    pDb->close();
}

bool SqlOper::sqlQueryIsExist(int index)
{
    if(!pDb->open()) {
        qDebug() << "sqlQueryIsExist db open failed!";
        return false;
    }
    qDebug() << "sqlQueryIsExist db open success";
    QSqlQuery query(*pDb);
    // QString sqlQuery = "select"
    pDb->close();
    return true;
}

void SqlOper::sqlDeleteById(QString index)
{
    if(!pDb->open()) {
        qDebug() << "sqlQueryIsExist db open failed!";
        return;
    }

    QDateTime date = QDateTime::currentDateTime();
    QString today = date.toString("yyyy_MM_dd");
    QString tableName = "record_" + today;
    QSqlQuery query(*pDb);
    QString sqlDel = "delete from " + tableName + " where id = " + index;
    qDebug() << sqlDel;

    query.exec(sqlDel);

    pDb->close();
}

std::list<QString> SqlOper::sqlQueryByDate(QString date)
{
    std::list<QString> results;
    if(!pDb->open()) {
        qDebug() << "sqlQueryByDate db open failed! last error:[" << pDb->lastError().text() << "]";
        return results;
    }
    qDebug() << "sqlQueryByDate db open success";
    QSqlQuery query(*pDb);
    QString tableName = "record_" + date;
    QString sqlQuery = "select * from " + tableName + ";";
    qDebug() << sqlQuery;
    query.exec(sqlQuery);
    while(query.next()) {
        QString result = query.value("id").toString() + UtilityTools::holdPlaces(5) + query.value("time").toString() + UtilityTools::holdPlaces(5) +
                query.value("rWeight").toString() + UtilityTools::holdPlaces(15) + query.value("vWeight").toString() + UtilityTools::holdPlaces(15) +
                query.value("nWeight").toString() + UtilityTools::holdPlaces(15) + query.value("unitPrice").toString() + UtilityTools::holdPlaces(15) +
                query.value("price").toString();
        qDebug() << result;
        results.push_back(result);
    }

    return results;
}

void SqlOper::sqlPriceQuery(PriceInfo & priceInfo)
{
    if(!pDb->open()) {
        qDebug() << "sqlPriceQuery db open failed!";
        return;
    }
    qDebug() << "sqlPriceQuery db open success";
    QSqlQuery query(*pDb);
    QString sqlQuery = "SELECT * FROM configure ORDER BY id DESC LIMIT 1;";
    query.exec(sqlQuery);
    while(query.next()) {
        priceInfo.m_factoryPrice1 = query.value("fPrice_1").toString();
        priceInfo.m_factoryPrice2 = query.value("fPrice_2").toString();
        priceInfo.m_factoryPrice3 = query.value("fPrice_3").toString();
        priceInfo.m_price1 = query.value("price_1").toString();
        priceInfo.m_price2 = query.value("price_2").toString();
        priceInfo.m_price3 = query.value("price_3").toString();
        priceInfo.m_price4 = query.value("price_4").toString();
    }
    pDb->close();
}

void SqlOper::sqlPriceSet(PriceInfo & priceInfo)
{
    if(!pDb->open()) {
        qDebug() << "sqlPriceSet db open failed! last error:[" << pDb->lastError().text() << "]";
        return;
    }
    qDebug() << "sqlPriceSet db open success";
    QSqlQuery query(*pDb);
    QString sqlPriceInsert = "insert into configure(date, fPrice_1, fPrice_2, fPrice_3, price_1, price_2, price_3, price_4) values(:date,:fPrice_1, :fPrice_2, :fPrice_3,:price_1,:price_2,:price_3,:price_4);";
    query.prepare(sqlPriceInsert);
    QDateTime date = QDateTime::currentDateTime();
    QString today = date.toString("yyyy-MM-dd hh:mm:ss");
    query.bindValue(":date", today);
    query.bindValue(":fPrice_1", priceInfo.m_factoryPrice1);
    query.bindValue(":fPrice_2", priceInfo.m_factoryPrice2);
    query.bindValue(":fPrice_3", priceInfo.m_factoryPrice3);
    query.bindValue(":price_1", priceInfo.m_price1);
    query.bindValue(":price_2", priceInfo.m_price2);
    query.bindValue(":price_3", priceInfo.m_price3);
    query.bindValue(":price_4", priceInfo.m_price4);
    qDebug() << "fPrice_1 = " << priceInfo.m_factoryPrice1;
    query.exec();
    pDb->close();
}

