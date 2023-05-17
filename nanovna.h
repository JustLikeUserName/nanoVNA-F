#ifndef NANOVNA_H
#define NANOVNA_H

#include <QObject>
#include <QSerialPort>
#include <logging.h>

class NanoVNA : public QSerialPort
{
    Q_OBJECT
public:
    struct SerialPortSetting
    {
        enum BaudRate baudRate=Baud9600;
        enum DataBits dataBits=Data8;
        enum Parity parity=NoParity;
        enum StopBits stopBits=OneStop;
        enum FlowControl flowControl=NoFlowControl;
    };
    enum CMD4SParameters {S11=3,S21=5,S11AndS21=7};
    enum CMD4CAL {OPEN,SHORT,LOAD,THRU,DONE,RESET};
public:
    NanoVNA(QString comName);
    NanoVNA(QString comName,SerialPortSetting settings);
    ~NanoVNA();
    bool OpenVNA();
    bool OpenVNA(SerialPortSetting settings);
    qint32 CfgVNA(quint32 startFreq,quint32 stopFreq,quint32 sweepPoints);
    qint32 SParametersTest(quint32 startFreq,quint32 stopFreq,quint32 sweepPoints,enum CMD4SParameters cmd);
    qint32 Calibrate(CMD4CAL cmd);
private:
    qint32 SParametersTest(enum CMD4SParameters cmd);

private:
    quint32 startFreq;
    quint32 stopFreq;
    quint32 sweepPoints;
    //log
    logging vnaLog;
};

#endif // NANOVNA_H
