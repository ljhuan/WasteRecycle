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
#include <QDir>
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
    QString dbFile = QDir::currentPath() + "/BusinessRecord.db";
    qDebug() << "dbFile:" << dbFile;
    bIsExist = QFile::exists(dbFile);
    if(!pDb->open()) {
        qDebug() << "SqlOper db open failed";
    } else {
        qDebug() << "SqlOper db open success";
        if(!bIsExist) {
            qDebug() << "db is not exist, so create it";
            pDb->exec("create table configure(id INTEGER PRIMARY KEY autoincrement, date varchar(50), fPrice_1 varchar(50), fPrice_2 varchar(50), fPrice_3 varchar(50),"
                      "price_1 varchar(50), price_2 varchar(50), price_3 varchar(50), price_4 varchar(50));");

            QString createChartsTable = "create table charts (time varchar(100), amouts varchar(10), totalWeight varchar(10), totalPrice varchar(10), averagePrice varchar(10));";
            pDb->exec(createChartsTable);

            // 创建正在卸货的表
            pDb->exec("create table unloading(id INTEGER PRIMARY KEY autoincrement, date varchar(50), time varchar(50), idx INTEGER, rWeight varchar(50));");
        } else {
            qDebug() << "db is exist";
        }

        tablesNames =  pDb->tables();
        pDb->close();
    }
}

void SqlOper::sqlInsert(RecordInfo info, QString date)
{
    // 若日期变化， 则新建一张数据表
    if(!pDb->open()) {
        qDebug() << "sqlInsert db open failed";
        return;
    }
    qDebug() << "sqlInsert db open success";

    // QDateTime date = QDateTime::currentDateTime();
    // QString today = date.toString("yyyy_MM_dd");
    QString tableName = "record_" + date;
    qDebug() << tableName;

    // 拼接sql语句
    QString qstrSqlJudgeTableExist = "select * from " + tableName + ";";
    // 需要写入的信息有排队号码, 时间， 毛重， 车重， 净重， 价格， 总价， 姓名，种类
    QString qstrSqlCreateTable = "create table " + tableName + "(id INTEGER PRIMARY KEY, time varchar(100), rWeight varchar(10), vWeight varchar(10), nWeight varchar(10), unitPrice varchar(10), price varchar(10), name varchar(15), kind varchar(10));";
    QSqlQuery query(*pDb);
    query.exec(qstrSqlJudgeTableExist);

    // 如果存在这张表， 则不需要创建， 若不存在， 则新建一张表, 并且将上一天表格中的数据
    if(!query.isActive()) {
        pDb->exec(qstrSqlCreateTable);
    } else {
        query.clear();
    }

    QString qstrIndex = info.m_index;
    QString sqlSqlInsert = "insert into " + tableName + " values(" + qstrIndex + ", '" +  info.m_time + "'" + ", '" +  info.m_rWeight + "'"
            + ", '" +  info.m_vWeight + "'" + ", '" +  info.m_nWeight + "'"+ ", '" +  info.m_unitPrice + "'"+ ", '" +  info.m_price+ "'"
            + ", '" +  info.m_name + "'"+ ", '" +  info.m_kind + "'"+  ");";
    qDebug() << sqlSqlInsert;
    query.exec(sqlSqlInsert);
    query.finish();
    qDebug() << "write to db";
    pDb->close();
}

void SqlOper::sqlInsertUnloading(QString idx, QString rWeight)
{
    if(!pDb->open()) {
        qDebug() << "sqlInsertUnloading db open failed";
        return;
    }
    QDateTime date = QDateTime::currentDateTime();
    QString today = date.toString("yyyy-MM-dd");

    QString time = date.toString("yyyy-MM-dd hh:mm:ss");

    QSqlQuery query(*pDb);
    if (sqlQueryUnloadingIdxIsExist(idx, "unloading")) {
        QString sqlDeleteByIdx = "delete from unloading where idx=" + idx + ";";
        query.exec(sqlDeleteByIdx);
        query.clear();
        qDebug() << sqlDeleteByIdx;
    }
    QString sqlInsert = "insert into unloading(date, time, idx, rWeight) values('" + today + "', '" + time + "', " + idx + ", '" + rWeight + "');";
    query.exec(sqlInsert);
    qDebug() << sqlInsert;
    pDb->close();
}

void SqlOper::sqlDeleteUnloadingByIdx(QString idx)
{
    if(!pDb->open()) {
        qDebug() << "sqlDeleteUnloadingByIdx db open failed";
        return;
    }

    QDateTime date = QDateTime::currentDateTime();
    QString today = date.toString("yyyy-MM-dd");
    QString sqlDel = "delete from unloading where date='" + today + "' and idx=" + idx + ";";
    qDebug() << sqlDel;
    QSqlQuery query(*pDb);
    query.exec(sqlDel);
    pDb->close();
}

bool SqlOper::sqlQueryUnloadingIdxIsExist(QString idx, QString tableName)
{
    qDebug() << "sqlQueryIsExist db open success";
    QSqlQuery query(*pDb);
    QString sqlQuery = "select * from " + tableName + " where idx = " + idx + ";";
    qDebug() << sqlQuery;
    query.exec(sqlQuery);
    while (query.next()) {
        return true;
    }
    return false;
}

std::list<QString> SqlOper::sqlQueryUnloadingByDate(QString date)
{
    std::list<QString> results;
    if(!pDb->open()) {
        qDebug() << "sqlQueryUnloadingByDate db open failed! last error:[" << pDb->lastError().text() << "]";
        return results;
    }

    date.replace("_", "-");
    QSqlQuery query(*pDb);
    QString sqlQuery = "select * from unloading where date = '" + date + "';";
    qDebug() << sqlQuery;
    query.exec(sqlQuery);
    while(query.next()) {
        QString result = query.value("time").toString() + "  " + query.value("idx").toString() + "  " + query.value("rWeight").toString();
        qDebug() << result;
        results.push_back(result);
    }

    return results;
}

QStringList SqlOper::sqlGetAllTableName()
{
//    QStringList tables;
//    if(!pDb->open()) {
//        qDebug() << "sqlGetAllTableName db open failed!";
//        return tables;
//    }

    return tablesNames;
}

void SqlOper::createTable(QString sql)
{
    if(!pDb->open()) {
        qDebug() << "createTable db open failed!";
        return;
    }
    pDb->exec(sql);

    // 更新数据库的表名List
    tablesNames = pDb->tables();
}

void SqlOper::deleteFromTable(QString sql)
{
    if(!pDb->open()) {
        qDebug() << "createTable db open failed!";
        return;
    }
    pDb->exec(sql);

    // 更新数据库的表名List
    tablesNames = pDb->tables();
}

void SqlOper::insertTable(QString sql)
{
    if (!pDb->isOpen() && !pDb->open()) {
        qDebug() << "insertTable db open failed!";
        return;
    }
     QSqlQuery query(*pDb);
     qDebug() << sql;
     query.exec(sql);
     query.finish();
}

bool SqlOper::searchTableWetherExist(QString tableName, QString item, QString value)
{
    if(!pDb->open()) {
        qDebug() << "sqlQueryIsExist db open failed!";
        return false;
    }
     QSqlQuery query(*pDb);
     QString sqlQuery = "select * from " + tableName + " where " + item + " = '" + value + "';";
     // QString sqlQuery = "select * from " + tableName + " where idx = " + idx + ";";
     qDebug() << sqlQuery;
     query.exec(sqlQuery);
     while (query.next()) {
         return true;
     }
     return false;
}

std::list<QString> SqlOper::queryTableRecords(QString sql)
{
    std::list<QString> results;
    if(!pDb->open()) {
        qDebug() << "queryTableRecords db open failed! last error:[" << pDb->lastError().text() << "]";
        return results;
    }
    QSqlQuery query(*pDb);
    qDebug() << sql;
    query.exec(sql);
    while(query.next()) {
        QString result = query.value("time").toString() + UtilityTools::holdPlaces(2) + query.value("amouts").toString() + UtilityTools::holdPlaces(2) +
                query.value("totalWeight").toString() + UtilityTools::holdPlaces(2) + query.value("totalPrice").toString() + UtilityTools::holdPlaces(2) +
                query.value("averagePrice").toString();
        // qDebug() << result;
        results.push_back(result);
    }

    return results;
}

std::list<QString> SqlOper::queryRecords(QString sql)
{
    std::list<QString> results;
    if(!pDb->open()) {
        qDebug() << "queryTableRecords db open failed! last error:[" << pDb->lastError().text() << "]";
        return results;
    }
    QSqlQuery query(*pDb);
    qDebug() << sql;
    query.exec(sql);
    while(query.next()) {
        QString result = query.value("time").toString() + UtilityTools::holdPlaces(2) + query.value("rWeight").toString() + UtilityTools::holdPlaces(2) +
                query.value("vWeight").toString() + UtilityTools::holdPlaces(2) + query.value("nWeight").toString() + UtilityTools::holdPlaces(2) +
                query.value("unitPrice").toString() + UtilityTools::holdPlaces(2) + query.value("price").toString() + UtilityTools::holdPlaces(2) +
                query.value("kind").toString();
        // qDebug() << result;
        results.push_back(result);
    }

    return results;
}

std::list<QString> SqlOper::queryTableCharts(QString sql)
{
    std::list<QString> results;
    if(!pDb->open()) {
        qDebug() << "queryTableRecords db open failed! last error:[" << pDb->lastError().text() << "]";
        return results;
    }
    QSqlQuery query(*pDb);
    qDebug() << sql;
    query.exec(sql);
    while(query.next()) {
        QString result = query.value("time").toString() + UtilityTools::holdPlaces(2) + query.value("amouts").toString() + UtilityTools::holdPlaces(2) +
                query.value("totalWeight").toString() + UtilityTools::holdPlaces(2) + query.value("totalPrice").toString() + UtilityTools::holdPlaces(2) +
                query.value("averagePrice").toString();
        // qDebug() << result;
        results.push_back(result);
    }

    return results;
}

std::list<QString> SqlOper::queryTableMembers(QString sql)
{
    std::list<QString> results;
    if(!pDb->open()) {
        qDebug() << "queryTableMembers db open failed! last error:[" << pDb->lastError().text() << "]";
        return results;
    }
    QSqlQuery query(*pDb);
    qDebug() << sql;
    query.exec(sql);
    while(query.next()) {
        QString result = query.value("phone").toString() + UtilityTools::holdPlaces(2) + query.value("name").toString() + UtilityTools::holdPlaces(2) +
                query.value("time").toString();
        // qDebug() << result;
        results.push_back(result);
    }

    return results;
}

std::list<QString> SqlOper::queryTablePrices(QString sql)
{
    std::list<QString> results;
    if(!pDb->open()) {
        qDebug() << "queryTablePrices db open failed! last error:[" << pDb->lastError().text() << "]";
        return results;
    }
    QSqlQuery query(*pDb);
    qDebug() << sql;
    query.exec(sql);
    while(query.next()) {
        QString result = query.value("kind").toString() + UtilityTools::holdPlaces(2) + query.value("value").toString();
        // qDebug() << result;
        results.push_back(result);
    }

    return results;
}

void SqlOper::sqlDeleteById(QString index, QString date)
{
    if(!pDb->open()) {
        qDebug() << "sqlQueryIsExist db open failed!";
        return;
    }

    // QDateTime date = QDateTime::currentDateTime();
    // QString today = date.toString("yyyy_MM_dd");
    QString tableName = "record_" + date;
    QSqlQuery query(*pDb);
    QString sqlDel = "delete from " + tableName + " where id = " + index;
    qDebug() << sqlDel;

    query.exec(sqlDel);

    pDb->close();
}

void SqlOper::sqlDeleteMembersByPhone(QString phone)
{
    if(!pDb->open()) {
        qDebug() << "sqlQueryIsExist db open failed!";
        return;
    }

    // QDateTime date = QDateTime::currentDateTime();
    // QString today = date.toString("yyyy_MM_dd");
    QString tableName = "members";
    QSqlQuery query(*pDb);
    QString sqlDel = "delete from " + tableName + " where phone = " + phone;
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
        QString result = query.value("id").toString() + UtilityTools::holdPlaces(2) + query.value("time").toString() + UtilityTools::holdPlaces(2) +
                query.value("rWeight").toString() + UtilityTools::holdPlaces(2) + query.value("vWeight").toString() + UtilityTools::holdPlaces(2) +
                query.value("nWeight").toString() + UtilityTools::holdPlaces(2) + query.value("unitPrice").toString() + UtilityTools::holdPlaces(2) +
                query.value("price").toString() + UtilityTools::holdPlaces(2) + query.value("name").toString() + UtilityTools::holdPlaces(2) +
                query.value("kind").toString();
        qDebug() << "record:" << result;
        results.push_back(result);
    }

    return results;
}

std::list<QString> SqlOper::sqlLimitQueryByDate(QString date, QString upLimit, QString downLimit)
{
    std::list<QString> results;
    if(!pDb->open()) {
        qDebug() << "sqlQueryByDate db open failed! last error:[" << pDb->lastError().text() << "]";
        return results;
    }
    qDebug() << "sqlQueryByDate db open success";
    QSqlQuery query(*pDb);
    QString tableName = "record_" + date;
    QString sqlQuery = "select * from " + tableName + " where unitPrice >= " + downLimit + " and unitPrice <= " + upLimit+ ";";
    qDebug() << sqlQuery;
    query.exec(sqlQuery);
    while(query.next()) {
        QString result = query.value("id").toString() + UtilityTools::holdPlaces(2) + query.value("time").toString() + UtilityTools::holdPlaces(2) +
                query.value("rWeight").toString() + UtilityTools::holdPlaces(2) + query.value("vWeight").toString() + UtilityTools::holdPlaces(2) +
                query.value("nWeight").toString() + UtilityTools::holdPlaces(2) + query.value("unitPrice").toString() + UtilityTools::holdPlaces(2) +
                query.value("price").toString() + UtilityTools::holdPlaces(2) + query.value("name").toString() + UtilityTools::holdPlaces(2) +
                query.value("kind").toString();
        qDebug() << "record:" << result;
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
    qDebug() << sqlQuery;
    query.exec(sqlQuery);
    while(query.next()) {
        priceInfo.m_factoryPrice1 = query.value("fPrice_1").toString();
        priceInfo.m_factoryPrice2 = query.value("fPrice_2").toString();
        priceInfo.m_factoryPrice3 = query.value("fPrice_3").toString();
        priceInfo.m_price1 = query.value("price_1").toString();
        priceInfo.m_price2 = query.value("price_2").toString();
        priceInfo.m_price3 = query.value("price_3").toString();
        priceInfo.m_price4 = query.value("price_4").toString();
        qDebug() << "has result";
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

