#include "nanovna.h"

NanoVNA::NanoVNA(QString comName)
{
    startFreq = 50000;
    stopFreq = 3000000000;
    sweepPoints = 51;
    SerialPortSetting settings;
    this->setPortName(comName);
    this->setBaudRate(settings.baudRate);
    this->setDataBits(settings.dataBits);
    this->setParity(settings.parity);
    this->setStopBits(settings.stopBits);
    this->setFlowControl(settings.flowControl);
    vnaLog.init(QString("vnaLog"));
}

NanoVNA::NanoVNA(QString comName, NanoVNA::SerialPortSetting settings)
{
    startFreq = 50000;
    stopFreq = 3000000000;
    sweepPoints = 51;
    this->setPortName(comName);
    this->setBaudRate(settings.baudRate);
    this->setDataBits(settings.dataBits);
    this->setParity(settings.parity);
    this->setStopBits(settings.stopBits);
    this->setFlowControl(settings.flowControl);
    vnaLog.init(QString("vnaLog.txt"));
}

NanoVNA::~NanoVNA()
{

}

bool NanoVNA::OpenVNA()
{
    bool res = this->open(QIODevice::ReadWrite);
    return res;
}

bool NanoVNA::OpenVNA(NanoVNA::SerialPortSetting settings)
{
    this->setBaudRate(settings.baudRate);
    this->setDataBits(settings.dataBits);
    this->setParity(settings.parity);
    this->setStopBits(settings.stopBits);
    this->setFlowControl(settings.flowControl);
    bool res = this->open(QIODevice::ReadWrite);
    return res;
}

qint32 NanoVNA::CfgVNA(quint32 startFreq, quint32 stopFreq, quint32 sweepPoints)
{
    this->startFreq = startFreq;
    this->stopFreq = stopFreq;
    this->sweepPoints = sweepPoints;
    QString cmd = QString("sweep %1 %2 %3\r\n").arg(startFreq).arg(stopFreq).arg(sweepPoints);
    qint32 retValue;
    if(this->isOpen())
    {
        QByteArray ba = cmd.toLocal8Bit();
        this->clear();
        retValue = this->write(ba);
    }
    else
        retValue = -1;
    return retValue;
}

qint32 NanoVNA::SParametersTest(NanoVNA::CMD4SParameters cmd)
{

    QString sCmd = QString("scan %1 %2 %3 %4\r\n").arg(startFreq).arg(stopFreq).arg(sweepPoints).arg(cmd);
    qint32 retValue;
    if(this->isOpen())
    {
        QByteArray ba = sCmd.toLocal8Bit();
        this->clear();
        retValue = this->write(ba);
    }
    else
        retValue = -1;
    return retValue;
}

qint32 NanoVNA::SParametersTest(quint32 startFreq, quint32 stopFreq,quint32 sweepPoints, NanoVNA::CMD4SParameters cmd)
{
    qreal oldStartFreq,oldStopFreq,oldSweepPoints;
    oldStartFreq = this->startFreq;
    oldStopFreq = this->stopFreq;
    oldSweepPoints = this->sweepPoints;
    this->startFreq = startFreq;
    this->stopFreq = stopFreq;
    this->sweepPoints = sweepPoints;
    qreal retValue = this->SParametersTest(cmd);
    if(retValue==-1)
    {
        this->startFreq = oldStartFreq;
        this->stopFreq = stopFreq;
        this->sweepPoints = oldSweepPoints;
    }
    return retValue;
}

qint32 NanoVNA::Calibrate(NanoVNA::CMD4CAL cmd)
{
    this->clear();
    qint32 retValue;
    QString calCMD;
    if(cmd == OPEN)
        calCMD = QString("cal open\r\n");
    else if (cmd == SHORT)
        calCMD = QString("cal short\r\n");
    else if (cmd == LOAD)
        calCMD = QString("cal load\r\n");
    else if (cmd == THRU)
        calCMD = QString("cal thru\r\n");
    else if (cmd == DONE)
         calCMD = QString("cal done\r\n");
    else if (cmd == RESET)
         calCMD = QString("cal reset\r\n");
    else
    {
        retValue = -1;
        return retValue;
    }
    if(this->isOpen())
    {
        QByteArray ba = calCMD.toLocal8Bit();
        this->clear();
        retValue = this->write(ba);
    }
    else
        retValue = -1;
    return retValue;

}
