#ifndef LOGGING_H
#define LOGGING_H

#include <QDir>
#include <QFile>
#include <QTextStream>

class logging
{
public:
    logging();
    ~logging();
    void init(QString name);
    void close();
    void write(QString msg);
    void writeWithTime(QString msg);
    QByteArray readAll();
    void clear();

private:
    QFile file_log; // 日志文件句柄
    QTextStream file_stream_log; // 日志文件流句柄
    QString baseName;
};

#endif // LOGGING_H
