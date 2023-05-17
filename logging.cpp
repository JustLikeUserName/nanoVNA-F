#include "logging.h"
#include "qdebug.h"
#include <QDateTime>

logging::logging()
{
}

logging::~logging()
{
}

void logging::init(QString name)
{
    baseName = name;
    /*
     *初始化存储日志的文件, 调试用
     */
    if (file_log.isOpen()) {
        file_log.close();
    }
    // 设置文件名
    file_log.setFileName(QString("./log_%1.txt").arg(name));
    if(!file_log.open(QIODevice::ReadWrite | QIODevice::Text | QFile::Append)) {
        qDebug() <<QDateTime::currentDateTime()<< QObject::tr(":不能打开文件")<<QString("log_%1.txt").arg(name);
    }
    file_stream_log.setDevice(&file_log);
}

void logging::close()
{
    /*
     *关闭打开的文件
     */
    if (file_log.isOpen()) {
        file_log.close();
    }
}

// 往日志里面写入数据
void logging::write(QString msg)
{
    if (file_log.isOpen()) {
        file_stream_log << msg <<endl;
        file_stream_log.flush();
        file_log.flush();
    }
}

void logging::writeWithTime(QString msg)
{
    if (file_log.isOpen()) {
        file_stream_log <<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") <<":"<< msg <<endl;
        file_stream_log.flush();
        file_log.flush();
    }
}

// 读取全部日志数据
QByteArray logging::readAll()
{
    if (file_log.isOpen()) {
        file_log.seek(0);
        return file_log.readAll();
    }
    return "";
}

// 清除日志
void logging::clear()
{
    if (file_log.isOpen()) {
        file_log.remove();
        init(baseName);
    }
}
