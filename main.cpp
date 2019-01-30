#include "wasterecycle.h"
#include <QApplication>

#include <QMutex>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
// #include <QtGlobal>

// qt 日志函数
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug:");
        break;

    case QtWarningMsg:
        text = QString("Warning:");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;

    case QtFatalMsg:
        text = QString("Fatal:");
    }

    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("(%1)").arg(current_date_time);
    QString message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);

    QString fileName = "log_" +  QDateTime::currentDateTime().toString("yyyy_MM_dd") + ".txt";
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}


int main(int argc, char *argv[])
{
    // qInstallMessageHandler(outputMessage);
    QApplication a(argc, argv);

    //api实例指针
    BaiduFaceApi* api = new BaiduFaceApi();
    //初始化sdk
    // 若采用证件照模式，请把id_card设为true，否则为false，证件照模式和非证件照模式提取的人脸特征值不同，
    // 不能混用
    bool id_card = false;
    api->sdk_init(id_card);
    // 提前加载人脸库到内存
    api->load_db_face();

    WasteRecycle w(api);
    w.show();

    return a.exec();
}
