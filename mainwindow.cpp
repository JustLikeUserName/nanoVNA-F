#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QMessageBox>
#include <cmath>
#include <QScreen>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    isVnaOpened = false;
    pVna = nullptr;
    setSelfValidator();
    availableSerialDevice();
    initChart();
    initTimer();
    log.init("vnaErrorLog");
    this->setWindowTitle(QString("NanoVNA F2应用"));
    this->setWindowState(Qt::WindowMaximized);
}

MainWindow::~MainWindow()
{
    delete ui;
    if(pVna)
        pVna->deleteLater();
    if(pS11Callout)
        delete pS11Callout;
    if(pS21Callout)
        delete pS21Callout;
}

void MainWindow::setSelfValidator()
{
    QRegExp regx("^[1-9]?[0-9]*");
    QRegExpValidator validator(regx,this);
    ui->leStartFreq->setValidator(&validator);
    ui->leStopFreq->setValidator(&validator);
}

void MainWindow::availableSerialDevice()
{
    /*****搜索所有可用串口*****/
    foreach (const QSerialPortInfo &inf0, QSerialPortInfo::availablePorts())
    {
        ui->cbSerialportName->addItem(inf0.portName());
    }
    ui->cbBaudrate->addItem("9600");
    ui->cbBaudrate->addItem("115200");
    ui->cbSerialportName->setCurrentIndex(0);
    ui->cbBaudrate->setCurrentIndex(0);
}

void MainWindow::initChart()
{
    s11Series.setName("S11");
    //s11Series.setColor(Qt::yellow);
    s21Series.setName("S21");
    //s21Series.setColor(Qt::cyan);
    sParameterChart.addSeries(&s11Series);
    sParameterChart.addSeries(&s21Series);
    sParameterChart.createDefaultAxes();
    sParameterChart.axisX()->setTitleText("freq(Hz)");
    sParameterChart.axisY()->setTitleText("dB");
    //sParameterChart.setBackgroundBrush(Qt::black);
    ui->chart->setChart(&sParameterChart);
    pS11Callout = new Callout(&sParameterChart);
    pS11Callout->hide();
    connect(&s11Series,SIGNAL(hovered(const QPointF , bool )),this,SLOT(on_s11SeriesHovered(const QPointF, bool)));
    pS21Callout = new Callout(&sParameterChart);
    pS21Callout->hide();
    connect(&s21Series,SIGNAL(hovered(const QPointF , bool )),this,SLOT(on_s21SeriesHovered(const QPointF, bool)));
}

void MainWindow::initTimer()
{
    timer.setInterval(500);
    connect(&timer,SIGNAL(timeout()),this,SLOT(onTimerOut()));
}

bool MainWindow::initVNA(QString comName,QString baudRate)
{
    if(!pVna)
        pVna = new NanoVNA(comName);
    pVna->setPortName(comName);
    pVna->setBaudRate(baudRate.toInt());
    bool res=true;
    if(isVnaOpened)
    {
        pVna->clear();
        pVna->close();
    }
    res = pVna->OpenVNA();
    if(res)
    {
        isVnaOpened = true;
        ui->statusbar->showMessage(QString("nanoVNA-F V2已连接成功"));
        vnaStatus = None;
        connect(pVna,SIGNAL(readyRead()),this,SLOT(onVnaReadyRead()));
        connect(pVna, &QSerialPort::errorOccurred, this, &MainWindow::onVnaError);
    }
    return res;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    int bt =QMessageBox::question(this,"警告","退出软件？",QMessageBox::Ok|QMessageBox::No,QMessageBox::Ok);
    if(bt == QMessageBox::Ok)
        e->accept();
    else
        e->ignore();
}


void MainWindow::on_pbConnect_clicked()
{
    bool res;
    res = initVNA(ui->cbSerialportName->currentText(),ui->cbBaudrate->currentText());
    if(!res)
       QMessageBox::warning(this,QString("警告"),QString("连接网分仪失败，请检查是否正确连线，谢谢！"));
}

void MainWindow::on_pbDisConnect_clicked()
{
    if(isVnaOpened)
    {
        pVna->clearError();
        pVna->clear();
        pVna->close();
        isVnaOpened = false;
        ui->statusbar->showMessage(QString("nanoVNA-F V2断开连接"));
    }
}

void MainWindow::on_pbConfig_clicked()
{
    if(isVnaOpened)
    {
        if(ui->leStartFreq->text().isEmpty() || ui->leStopFreq->text().isEmpty())
        {
            QMessageBox::warning(this,QString("警告"),QString("起始频率/截止频率设置不能为空，谢谢！"));
            return;
        }
        quint32 startFreq = ui->leStartFreq->text().toInt();
        quint32 stopFreq = ui->leStopFreq->text().toInt();
        quint32 sweepPoints;
        sweepPoints = ui->cbSweepPoints->currentText().toInt();
        int retValue = pVna->CfgVNA(startFreq,stopFreq,sweepPoints);
        if(retValue == -1)
            ui->statusbar->showMessage(QString("配置nanoVNA-F V2失败"));
        else
            ui->statusbar->showMessage(QString("配置nanoVNA-F V2成功"));
    }
    else
       QMessageBox::warning(this,QString("警告"),QString("请先连接仪器，谢谢！"));
}

void MainWindow::on_pbOpenCal_clicked()
{
    NanoVNA::CMD4CAL calCmd;
    calCmd = NanoVNA::CMD4CAL::OPEN;
    if(isVnaOpened)
    {
        QMessageBox::information(this,"提示信息",QString("端口1和端口2连接开路负载，按确定键进行开路校准"));
        pVna->Calibrate(calCmd);
        ui->textNote->append(QString("完成开路校准"));
    }
    else
        QMessageBox::warning(this,QString("警告"),QString("请先连接仪器，谢谢！"));
}

void MainWindow::on_pbShortCal_clicked()
{
    NanoVNA::CMD4CAL calCmd;
    calCmd = NanoVNA::CMD4CAL::SHORT;
    if(isVnaOpened)
    {
        QMessageBox::information(this,"提示信息",QString("端口1和端口2连接短路负载，按确定键进行短路校准"));
        pVna->Calibrate(calCmd);
        ui->textNote->append(QString("完成短路校准"));
    }
    else
        QMessageBox::warning(this,QString("警告"),QString("请先连接仪器，谢谢！"));
}

void MainWindow::on_pbLoadCal_clicked()
{
    NanoVNA::CMD4CAL calCmd;
    calCmd = NanoVNA::CMD4CAL::LOAD;
    if(isVnaOpened)
    {
        QMessageBox::information(this,"提示信息",QString("端口1和端口2连接匹配负载，按确定键进行匹配校准"));
        pVna->Calibrate(calCmd);
        ui->textNote->append(QString("完成匹配校准"));
    }
    else
        QMessageBox::warning(this,QString("警告"),QString("请先连接仪器，谢谢！"));
}

void MainWindow::on_pbThruCal_clicked()
{
    NanoVNA::CMD4CAL calCmd;
    calCmd = NanoVNA::CMD4CAL::THRU;
    if(isVnaOpened)
    {
        QMessageBox::information(this,"提示信息",QString("端口1和端口2连接直通接头（THRU），按确定键进行直通校准"));
        pVna->Calibrate(calCmd);
       ui->textNote->append(QString("完成直通校准"));
    }
    else
        QMessageBox::warning(this,QString("警告"),QString("请先连接仪器，谢谢！"));
}

void MainWindow::on_pbDoneCal_clicked()
{
    NanoVNA::CMD4CAL calCmd;
    calCmd = NanoVNA::CMD4CAL::DONE;
    if(isVnaOpened)
    {
        pVna->Calibrate(calCmd);
        ui->textNote->append(QString("已确认当前校准！"));
    }
    else
        QMessageBox::warning(this,QString("警告"),QString("请先连接仪器，谢谢！"));
}

void MainWindow::on_pbResetCal_clicked()
{
    NanoVNA::CMD4CAL calCmd;
    calCmd = NanoVNA::CMD4CAL::RESET;
    if(isVnaOpened)
    {
        int bt =QMessageBox::question(this,"警告","放弃当前校准？",QMessageBox::Ok|QMessageBox::No,QMessageBox::Ok);
        if(bt == QMessageBox::Ok)
        {
            pVna->Calibrate(calCmd);
            ui->textNote->clear();
        }
    }
    else
        QMessageBox::warning(this,QString("警告"),QString("请先连接仪器，谢谢！"));
}

void MainWindow::on_pbSingleStep_clicked()
{
    NanoVNA::CMD4SParameters sParameter;
    if(ui->chbS11->isChecked() && ui->chbS21->isChecked())
    {
        sParameter = NanoVNA::S11AndS21;
        vnaStatus = S11AndS21;
    }
    else if(ui->chbS11->isChecked())
    {
        sParameter = NanoVNA::S11;
        vnaStatus = S11;
    }
    else if(ui->chbS21)
    {
        sParameter = NanoVNA::S21;
        vnaStatus = S21;
    }
    else
    {
        QMessageBox::warning(this,QString("警告"),QString("请勾选测量选项，谢谢！"));
        vnaStatus = None;
        return;
    }
    if(isVnaOpened)
    {
        quint32 startFreq = ui->leStartFreq->text().toUInt();
        quint32 stopFreq = ui->leStopFreq->text().toUInt();
        quint32 sweepPoints = ui->cbSweepPoints->currentText().toUInt();
        pVna->SParametersTest(startFreq,stopFreq,sweepPoints,sParameter);
    }
    else
    {
        vnaStatus = None;
        QMessageBox::warning(this,QString("警告"),QString("请先连接仪器，谢谢！"));
    }
}

void MainWindow::on_pbRun_clicked()
{
    if(isVnaOpened)
        timer.start();
    else
        QMessageBox::warning(this,QString("警告"),QString("请先连接仪器，谢谢！"));
}

void MainWindow::on_pbStop_clicked()
{
    timer.stop();
}

void MainWindow::on_pbSaveImage_clicked()
{
    QPixmap pixmap;
    QScreen *screen=QGuiApplication::primaryScreen();
    pixmap =screen->grabWindow(ui->chart->winId());
    QImage image = pixmap.toImage();
    QString fileName = QFileDialog::getSaveFileName
            (this,"Save Image","./","JPEG Files(*.jpg);PNG Files(*.png)");
    if(fileName.trimmed()!="")
    {
        pixmap.save(fileName);
        ui->statusbar->showMessage("save file:"+fileName,60);
    }
    else
    {
        ui->statusbar->showMessage("please input filename! ",60);
    }
}

void MainWindow::onVnaReadyRead()
{
    QByteArray ba = pVna->readAll();
    QString str(ba);
    retMsg+=str;
    if(retMsg.contains("ch>") && retMsg.contains("scan"))
    {
        int pos1 = retMsg.indexOf("scan"); //S tes's cmd
        int pos2 = retMsg.indexOf("ch>");
        QString str;
        while (pos2<pos1 && pos2!=-1)
        {
            str = retMsg.mid(pos1);
            pos2 = str.indexOf("ch>");
            retMsg = str;
        }
        QString msg;
        if(pos2>pos1) //pos1至少是0
        {
            msg=retMsg.mid(pos1,pos2);
            //Analyzing retMsg
            int N;//扫频点数
            N = ui->cbSweepPoints->currentText().toInt();//获取扫频点数
            //QStringList list = str.split('\n');//N个点，应该是N行
            double freq[102];//定义频率参数
            double s11_real[102];//定义s11实部
            double s11_im[102];//定义s11虚部
            double s21_real[102];//定义s11实部
            double s21_im[102];//定义s11虚部
            double s11Amplitude[102];
            double s21Amplitude[102];
            QStringList dataList = retMsg.split('\n');
            //dada error
            if(dataList.length() < N)
            {
                log.writeWithTime(QString("retMsgLen = %1  and N=%2\n retMsg=\n").arg(dataList.length()).arg(N));
                log.write(retMsg);
                retMsg.clear();
                return;
            }
            retMsg.clear();
            if(vnaStatus == ReadStatus::S11AndS21)
            {
                for(int i=1;i<N+1;++i)
                {
                    QStringList list2 = dataList.at(i).split(' ');//是空格分隔
                    if(list2.length()>=5)
                    {
                        freq[i-1] = list2.at(0).toDouble();
                        if(freq[i-1]<freq[0])
                            return;
                        s11_real[i-1]=list2.at(1).toDouble();
                        s11_im[i-1]=list2.at(2).toDouble();
                        s21_real[i-1]=list2.at(3).toDouble();
                        s21_im[i-1]=list2.at(4).toDouble();
                    }
                    else
                    {
                       return;
                    }
                }
                double max = 0; double min = 0;
                for (int i = 0; i < N; ++i)
                {
                    s11Amplitude[i] =20*0.5*log10(s11_real[i] * s11_real[i] + s11_im[i] * s11_im[i]);
                    s21Amplitude[i] = 20*0.5*log10(s21_real[i] * s21_real[i] + s21_im[i] * s21_im[i]);
                    if (max<s11Amplitude[i])
                        max = s11Amplitude[i];
                    if (min>s11Amplitude[i])
                        min = s11Amplitude[i];
                    if (max<s21Amplitude[i])
                        max = s21Amplitude[i];
                    if (min>s21Amplitude[i])
                        min = s21Amplitude[i];                    
                }

                sParameterChart.axisX()->setRange(freq[0],freq[N-1]);
                sParameterChart.axisY()->setRange(min*1.5,10);
                s11Series.clear();
                s21Series.clear();
                for(int i=0;i<N;++i)
                {
                    s11Series.append(freq[i],s11Amplitude[i]);
                    s21Series.append(freq[i],s21Amplitude[i]);
                    //log.write(QString("Data: i=%1\tS11=%2\tS21=%3\tFreq=%4").arg(i).arg(s11Amplitude[i]).arg(s21Amplitude[i]).arg(freq[i]));
                }
                vnaStatus=ReadStatus::None;
            }
            else if(vnaStatus == S21)
            {
                for(int i=1;i<N+1;++i)
                {
                    QStringList list2 = dataList.at(i).split(' ');//是空格分隔
                    if(list2.length()>=3)
                    {
                        freq[i-1] = list2.at(0).toDouble();
                        if(freq[i-1]<freq[0])
                            return;
                        s21_real[i-1]=list2.at(1).toDouble();
                        s21_im[i-1]=list2.at(2).toDouble();
                    }
                    else
                    {
                        return;
                    }
                }
                double max = 0; double min = 0;
                for (int i = 0; i < N; ++i)
                {
                    s21Amplitude[i] =20*0.5*log10(s21_real[i] * s21_real[i] + s21_im[i] * s21_im[i]);
                    s21Amplitude[i] =20*0.5*log10(s21_real[i] * s21_real[i] + s21_im[i] * s21_im[i]);
                    if (max<s21Amplitude[i])
                        max = s21Amplitude[i];
                    if (min>s21Amplitude[i])
                        min = s21Amplitude[i];
                }

                sParameterChart.axisX()->setRange(freq[0],freq[N-1]);
                sParameterChart.axisY()->setRange(min*1.5,10);
                s11Series.clear();
                s21Series.clear();
                for(int i=0;i<N;++i)
                {
                    s21Series.append(freq[i],s21Amplitude[i]);
                }
                vnaStatus=ReadStatus::None;
            }
            else if(vnaStatus == S11)
            {
                for(int i=1;i<N+1;++i)
                {
                    QStringList list2 = dataList.at(i).split(' ');//是空格分隔
                    if(list2.length()>=3)
                    {
                        freq[i-1] = list2.at(0).toDouble();
                        if(freq[i-1]<freq[0])
                            return;
                        s11_real[i-1]=list2.at(1).toDouble();
                        s11_im[i-1]=list2.at(2).toDouble();
                    }
                    else
                    {
                        return;
                    }
                }
                double max = 0; double min = 0;
                for (int i = 0; i < N; ++i)
                {
                    s11Amplitude[i] =20*0.5*log10(s11_real[i] * s11_real[i] + s11_im[i] * s11_im[i]);
                    s11Amplitude[i] =20*0.5*log10(s11_real[i] * s11_real[i] + s11_im[i] * s11_im[i]);
                    if (max<s11Amplitude[i])
                        max = s11Amplitude[i];
                    if (min>s11Amplitude[i])
                        min = s11Amplitude[i];
                }

                sParameterChart.axisX()->setRange(freq[0],freq[N-1]);
                sParameterChart.axisY()->setRange(min*1.5,10);
                s11Series.clear();
                s21Series.clear();
                for(int i=0;i<N;++i)
                {
                    s11Series.append(freq[i],s11Amplitude[i]);
                }
                vnaStatus=ReadStatus::None;
            }
            else
                return;
        }
    }
}

void MainWindow::onVnaError(QSerialPort::SerialPortError error)
{
    QString str(error);
    log.write(QString("Error:")+str);
}

void MainWindow::onTimerOut()
{
    if(isVnaOpened)
        on_pbSingleStep_clicked();
    else
        timer.stop();
}

void MainWindow::on_s11SeriesHovered(const QPointF &point, bool state)
{
    if (pS11Callout == nullptr)
        pS11Callout = new Callout(&sParameterChart);
    if (state)
    {
       pS11Callout->setText(QString("Freq:%2Hz\nS11: %1dB").arg(point.y()).arg(point.x()));
       pS11Callout->setAnchor(point);
       pS11Callout->setZValue(11);
       pS11Callout->updateGeometry();
       pS11Callout->show();
    }
    else
    {
        pS11Callout->hide();
    }
}

void MainWindow::on_s21SeriesHovered(const QPointF &point, bool state)
{
    if (pS21Callout == nullptr)
        pS21Callout = new Callout(&sParameterChart);
    if (state)
    {
       pS21Callout->setText(QString("Freq:%2Hz\nS21: %1dB").arg(point.y()).arg(point.x()));
       pS21Callout->setAnchor(point);
       pS21Callout->setZValue(11);
       pS21Callout->updateGeometry();
       pS21Callout->show();
    }
    else
    {
        pS21Callout->hide();
    }
}

void MainWindow::on_pbSaveData_clicked()
{
    int Num1 = s11Series.points().length();
    int Num2 = s21Series.points().length();
    if (0==Num1 && 0 == Num2)
    {
        QMessageBox::warning(this,"warning",tr("没有S11或者S22数据需要保存！"));
        return;
    }
    bool isTimerActive=false;
    if(timer.isActive())
    {
        timer.stop();
        isTimerActive = true;
    }
    QString FileName = QFileDialog::getSaveFileName(this,"Save Data","./","Data Files(*.dat)");
    if(FileName.isEmpty())
    {
        QMessageBox::warning(this,"warning",tr("请输入保存文件名"));
        return;
    }
    QFile file(FileName);
    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream out(&file);
        out<<"chl20230517\n";
        if(Num1)
        {
            out<<"S11\n";
            int i=0;
            while(i<Num1)
            {
                out<<s11Series.points().at(i).x()<<','<<s11Series.points().at(i).y();
                ++i;
                if(i<Num1)
                   out<<',';
                else
                    out<<'\n';
            }
        }

        if(Num2)
        {
            out<<"S21\n";
            int i=0;
            int Num2 = s21Series.points().length();
            while(i<Num2)
            {
                out<<s21Series.points().at(i).x()<<','<<s21Series.points().at(i).y();
                ++i;
                if(i<Num2)
                   out<<',';
            }
        }
        file.close();
        if(isTimerActive)
            timer.start();
    }
    else
    {
        QMessageBox::warning(this,"warning",tr("打开文件出错"));
    }
}


void MainWindow::on_pbLoadData_clicked()
{
    QString fileName = QFileDialog::getOpenFileName();
    if(!fileName.isEmpty())
    {
        QFile file(fileName);
        if(file.open(QIODevice::ReadOnly))
        {
            QTextStream in(&file);
            QString str;
            str = in.readLine();
            if(str.contains("chl20230517"))
            {
                if(timer.isActive())
                {
                    timer.stop();
                }
                bool isS11=false;
                bool isS21=false;
                if(!in.atEnd())
                {
                    double freq[401];//定义频率参数
                    double s11Amplitude[401];
                    int Num=0;
                    int min,max,start,stop;
                    min = 0;
                    max=0;
                    str = in.readLine();
                    log.write(str);
                    if(str.contains("S11"))
                    {
                        str = in.readLine();
                        log.write(str);
                        QStringList list = str.split(',');
                        double x;
                        double y;
                        for(int i=0;i<list.length();i=i+2)
                        {
                            x = list.at(i).toDouble();
                            y = list.at(i+1).toDouble();
                            freq[i/2]=x;
                            s11Amplitude[i/2]=y;
                            if(0==i)
                                start = x;
                            if(i==list.length()-2)
                                stop =x;
                            if(min>y)
                                min = y;
                            if(max<y)
                                max = y;
                            ++Num;
                        }
                        s11Series.clear();
                        sParameterChart.axisX()->setRange(start,stop);
                        if(max<10)
                            max = 10;
                        if(abs(min)<1)
                            min=-2;
                        sParameterChart.axisY()->setRange(min*1.5,max);
                        for(int i=0;i<Num;++i)
                        {
                           s11Series.append(freq[i],s11Amplitude[i]);
                        }
                        isS11 = true;
                    }
                    else
                    {
                        s11Series.clear();                       
                    }
                    if(!in.atEnd())
                    {
                        s21Series.clear();
                        double freq[401];//定义频率参数
                        double s21Amplitude[401];
                        int Num=0;
                        int min,max,start,stop;
                        min = 0;
                        max=0;
                        if(isS11)
                            str = in.readLine();
                        if(str.contains("S21"))
                        {
                            str = in.readLine();
                            log.write(str);
                            QStringList list = str.split(',');
                            double x;
                            double y;
                            for(int i=0;i<list.length();i=i+2)
                            {
                                x = list.at(i).toDouble();
                                y = list.at(i+1).toDouble();
                                freq[i/2]=x;
                                s21Amplitude[i/2]=y;
                                if(0==i)
                                    start = x;
                                if(i==list.length()-2)
                                    stop =x;
                                if(min>y)
                                    min = y;
                                if(max<y)
                                    max = y;
                                ++Num;
                            }
                            s21Series.clear();
                            sParameterChart.axisX()->setRange(start,stop);
                            if(max<10)
                                max = 10;
                            if(abs(min)<1)
                                min=-2;
                            sParameterChart.axisY()->setRange(min*1.5,max);
                            for(int i=0;i<Num;++i)
                            {
                               s21Series.append(freq[i],s21Amplitude[i]);
                            }
                            isS21 = true;
                        }
                        else
                            s21Series.clear();
                    }
                    else
                        if(!isS21)
                          s21Series.clear();
                }
            }
            else
            {
                QMessageBox::warning(this,"warning",tr("打开的不是S参数测量保存的历史数据文件！"));
            }
        }
        else
        {
            QMessageBox::warning(this,"warning",tr("打开文件出错"));
            return;
        }
    }
}

void MainWindow::on_pbSaveUIImage_clicked()
{
    QPixmap pixmap;
    pixmap =this->centralWidget()->grab(this->centralWidget()->rect());
    QImage image = pixmap.toImage();
    QString fileName = QFileDialog::getSaveFileName
            (this,"Save Image","./","JPEG Files(*.jpg);PNG Files(*.png)");
    if(fileName.trimmed()!="")
    {
        pixmap.save(fileName);
        ui->statusbar->showMessage("save file:"+fileName,60);
    }
    else
    {
        ui->statusbar->showMessage("please input filename! ",60);
    }
}
