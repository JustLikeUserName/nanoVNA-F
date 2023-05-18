#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QValidator>
#include <QDateTime>
#include <QMessageBox>
#include <QStringListModel>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);
    QString str;
    str = "传输线理论计算器";
    this->setWindowTitle(str);
    QPixmap img(":/image/basic"); //图片已经存入资源文件
    ui->labelImg->setText("");
    ui->labelImg->setPixmap(img);
    ui->labelImg->setScaledContents(true);
    ui->tabWidget->setCurrentIndex(0);
    setSelfValidator();
    setConfig();
    initList();
    initChartPainter();
    setMouseTracking(false);
}

MainWindow::~MainWindow()
{
    if(pPainter)
        delete pPainter;
    if(pImg)
        delete pImg;
    if(pPainter4Basic)
        delete  pPainter4Basic;
    if(pImg4Basic)
        delete pImg4Basic;
    if(pPainter4QuarnterLamda)
        delete pPainter4QuarnterLamda;
    for(int i=0;i<2;++i)
    {
        if(pPainter4SeriesStub[i])
            delete pPainter4SeriesStub[i];
        if(pImg4SeriesStub[i])
            delete pImg4SeriesStub[i];
    }
    for(int i=0;i<4;++i)
    {
        if(pPainter4PStub[i])
            delete pPainter4PStub[i];
        if(pImg4PStub[i])
            delete pImg4PStub[i];
    }
    delete ui;
}

void MainWindow::clearInputs()
{
    qDebug("Users clear all input parameters\n");
    ui->leZ0->setText("");
    ui->leZl_real->setText("");
    ui->leZl_img->setText("");
    ui->leFreq ->setText("");
    ui->leDist->setText("");
    ui->chbZin->setCheckState(Qt::Unchecked);
    ui->chbRCoef->setCheckState(Qt::Unchecked);
    ui->chbVswr->setCheckState(Qt::Unchecked);
    resList1.clear();
    model1.setStringList(resList1);
}

void MainWindow::setSelfValidator()
{
    QRegExp regx("^[-]?[0-9]*[.]?[0-9]*");
    QValidator* pValidator= new QRegExpValidator(regx,this);
    ui->leZ0->setValidator(pValidator);
    ui->leDist->setValidator(pValidator);
    ui->leFreq->setValidator(pValidator);
    ui->leZl_img->setValidator(pValidator);
    ui->leZl_real->setValidator(pValidator);

    ui->le_z0->setValidator(pValidator);
    ui->le_Freq->setValidator(pValidator);
    ui->le_zlR->setValidator(pValidator);
    ui->le_zlim->setValidator(pValidator);

    ui->leZ0_Series->setValidator(pValidator);
    ui->leFreq_Series->setValidator(pValidator);
    ui->leZl_real_Series->setValidator(pValidator);
    ui->leZl_img_Series->setValidator(pValidator);

    ui->leZ0_Parallel->setValidator(pValidator);
    ui->leFreq_Parallel->setValidator(pValidator);
    ui->leZl_real_Parallel->setValidator(pValidator);
    ui->leZl_img_Parallel->setValidator(pValidator);
}

void MainWindow::showWarning(const QString msg)
{
    QMessageBox::warning(this,"警告",msg);
    ui->statusbar->showMessage(msg);
    return;
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    //QMessageBox::Button bt =QMessageBox::question(this,"警告","退出软件？",QMessageBox::Ok|QMessageBox::No);
    int bt =QMessageBox::question(this,"警告","退出软件？",QMessageBox::Ok|QMessageBox::No,QMessageBox::Ok);
    if(bt == QMessageBox::Ok)
        e->accept();
    else
        e->ignore();
}

void MainWindow::setConfig()
{
    QFile* pFile = new QFile("./parameters.cfg");
    if(pFile->open(QIODevice::ReadOnly))
    {
        QTextStream infile(pFile);
        double z0,freq,d,zl_re,zl_im;
        infile>>z0>>d;
        infile>>freq;
        infile>>zl_re>>zl_im;
        // Tab 1
        ui->leZ0->setText(QString("%1").arg(z0));
        ui->leDist->setText(QString("%1").arg(d));
        ui->leFreq->setText(QString("%1").arg(freq));
        ui->leZl_real->setText(QString("%1").arg(zl_re));
        ui->leZl_img->setText(QString("%1").arg(zl_im));
        // Tab 2
        ui->le_z0->setText(QString("%1").arg(z0));
        ui->le_Freq->setText(QString("%1").arg(freq));
        ui->le_zlR->setText(QString("%1").arg(zl_re));
        ui->le_zlim->setText(QString("%1").arg(zl_im));
        // Tab 3
        ui->leZ0_Series->setText(QString("%1").arg(z0));
        ui->leFreq_Series->setText(QString("%1").arg(freq));
        ui->leZl_real_Series->setText(QString("%1").arg(zl_re));
        ui->leZl_img_Series->setText(QString("%1").arg(zl_im));
        // Tab 4
        ui->leZ0_Parallel->setText(QString("%1").arg(z0));
        ui->leFreq_Parallel->setText(QString("%1").arg(freq));
        ui->leZl_real_Parallel->setText(QString("%1").arg(zl_re));
        ui->leZl_img_Parallel->setText(QString("%1").arg(zl_im));
    }
    else
        showWarning("不能找到parameters.cfg文件");
    if(pFile)
    {
        pFile->close();
        pFile->deleteLater();
    }
}

void MainWindow::initList()
{
    model1.setStringList(resList1);
    model4.setStringList(resList4);
    model2.setStringList(resList2);
    model3.setStringList(resList3);
    ui->listResult->setModel(&model1);
    ui->ls_res->setModel(&model2);
    ui->listResult_Series->setModel(&model3);
    ui->listResult_Parallel->setModel(&model4);
    model_y.setStringList(strList_y);
    model_z.setStringList(strList_z);
    strList_z.append(QString("阻抗圆图求解步骤说明"));
    strList_y.append(QString("导纳圆图求解步骤说明"));
    model_y.setStringList(strList_y);
    model_z.setStringList(strList_z);
    strList2_z.append(QString("阻抗圆图求解步骤说明"));
    strList2_y.append(QString("导纳圆图求解步骤说明"));
    model2_y.setStringList(strList2_y);
    model2_z.setStringList(strList2_z);
    ui->list_Parallel_1->setModel(&model_z);
    ui->list_Parallel_2->setModel(&model_y);
    ui->list_Parallel_3->setModel(&model2_z);
    ui->list_Parallel_4->setModel(&model2_y);

    //SmithChart Section
    // the information of help
    strList_z.append("*************利用阻抗圆图求解（双击圆图，可以一步步演示做图求解过程）*******************");
    strList_z.append(QString("1. 求出终端负载的归一化阻抗，然后绘制电阻圆和电抗圆"));
    strList_z.append(QString("2. 找到终端负载电阻圆和电抗圆的交点，过交点绘制出终端负载的反射系数圆"));
    strList_z.append(QString("3. 由于采用并联枝节，其导纳才符合“+”性，因此需要利用阻抗与导纳相位差180°的关系\n \
                             将阻抗圆图视作导纳圆图，因此需要沿该反射系数圆旋转180°绘制出导纳圆图上等效的终端负载其电导圆和电纳圆的交点"));
    strList_z.append(QString("4. 绘制电导为1的匹配圆，找到并标记处其与终端负载的反射系数圆的交点（一般会有2个）"));
    strList_z.append(QString("5. 从负载位置向电源方向转到与匹配圆的2个交点（对应两组解），所转过的长度即为并联单枝节加入位置"));
    strList_z.append(QString("6. 若已选中一个与匹配圆相交的一个交点，则绘制出过该点的电纳，读取其对应的归一化电纳值"));
    strList_z.append(QString("7. 为了抵消该归一化电纳，绘制出与之取反的电纳值所对应的电纳圆，并绘制出其与反射系数为1的圆的交点"));
    strList_z.append(QString("8. 沿反射系数为1的反射系数圆将该交点旋转180°，逆向等效转换到阻抗圆图，并绘制该等效的枝节长度对应交点"));
    strList_z.append(QString("9. 从短路点（开路点）沿向电源方向转到该交点，其转过的长度即对应的短路（开路）枝节长度"));
    strList_z.append(QString("10. 其对应的两组解的求解做图过程请查阅上侧的对应的两个解的SmithChart。"));
    strList_z.append("******************************************************************************");
    model_z.setStringList(strList_z);

    strList2_z.append("*************利用阻抗圆图求解（双击圆图，可以一步步演示做图求解过程）****************");
    strList2_z.append(QString("1. 求出终端负载的归一化阻抗，然后绘制电阻圆和电抗圆"));
    strList2_z.append(QString("2. 找到终端负载电阻圆和电抗圆的交点，过交点绘制出终端负载的反射系数圆"));
    strList2_z.append(QString("3. 由于采用并联枝节，其导纳才符合“+”性，因此需要利用阻抗与导纳相位差180°的关系\n \
                             将阻抗圆图视作导纳圆图，因此需要沿该反射系数圆旋转180°绘制出导纳圆图上等效的终端负载其电导圆和电纳圆的交点"));
    strList2_z.append(QString("4. 绘制电导为1的匹配圆，找到并标记处其与终端负载的反射系数圆的交点（一般会有2个）"));
    strList2_z.append(QString("5. 从负载位置向电源方向转到与匹配圆的2个交点（对应两组解），所转过的长度即为并联单枝节加入位置"));
    strList2_z.append(QString("6. 若已选中一个与匹配圆相交的一个交点，则绘制出过该点的电纳，读取其对应的归一化电纳值"));
    strList2_z.append(QString("7. 为了抵消该归一化电纳，绘制出与之取反的电纳值所对应的电纳圆，并绘制出其与反射系数为1的圆的交点"));
    strList2_z.append(QString("8. 沿反射系数为1的反射系数圆将该交点旋转180°，逆向等效转换到阻抗圆图，并绘制该等效的枝节长度对应交点"));
    strList2_z.append(QString("9. 从短路点（开路点）沿向电源方向转到该交点，其转过的长度即对应的短路（开路）枝节长度"));
    strList2_z.append(QString("10. 其对应的两组解的求解做图过程请查阅上侧的对应的两个解的SmithChart。"));
    strList2_z.append("******************************************************************************");
    model2_z.setStringList(strList2_z);

    strList_y.append("**************利用导纳圆图求解（双击圆图，可以一步步演示做图求解过程）*****************");
    strList_y.append(QString("1. 由于采用并联枝节，其导纳符合“+”性，可以直接求出终端负载的归一化导纳，然后绘制电导圆和电纳圆"));
    strList_y.append(QString("2. 找到终端负载电导圆和电纳圆的交点，过交点绘制出终端负载的反射系数圆"));
    strList_y.append(QString("3. 绘制电导为1的匹配圆，找到并标记处其与终端负载的反射系数圆的交点（一般会有2个）"));
    strList_y.append(QString("4. 从负载位置向电源方向转到与匹配圆的2个交点（对应两组解），所转过的长度即为并联单枝节加入位置"));
    strList_y.append(QString("5. 若已选中一个与电导为1的匹配圆相交的一个交点，则绘制出过该点的电纳，读取其对应的归一化电纳值"));
    strList_y.append(QString("6. 为了抵消该归一化电纳，绘制出与之取反的电纳值所对应的电纳圆，并绘制出其与反射系数为1的圆的交点"));
    strList_y.append(QString("7. 从短路点（开路点）沿向电源方向转到该交点，其转过的长度即对应的短路（开路）枝节长度"));
    strList_y.append(QString("8. 其对应的两组解的求解做图过程请查阅上侧的对应的两个解的SmithChart。"));
    strList_y.append("******************************************************************************");
    model_y.setStringList(strList_z);

    strList2_y.append("**************利用导纳圆图求解（双击圆图，可以一步步演示做图求解过程）*****************");
    strList2_y.append(QString("1. 由于采用并联枝节，其导纳符合“+”性，可以直接求出终端负载的归一化导纳，然后绘制电导圆和电纳圆"));
    strList2_y.append(QString("2. 找到终端负载电导圆和电纳圆的交点，过交点绘制出终端负载的反射系数圆"));
    strList2_y.append(QString("3. 绘制电导为1的匹配圆，找到并标记处其与终端负载的反射系数圆的交点（一般会有2个）"));
    strList2_y.append(QString("4. 从负载位置向电源方向转到与匹配圆的2个交点（对应两组解），所转过的长度即为并联单枝节加入位置"));
    strList2_y.append(QString("5. 若已选中一个与电导为1的匹配圆相交的一个交点，则绘制出过该点的电纳，读取其对应的归一化电纳值"));
    strList2_y.append(QString("6. 为了抵消该归一化电纳，绘制出与之取反的电纳值所对应的电纳圆，并绘制出其与反射系数为1的圆的交点"));
    strList2_y.append(QString("7. 从短路点（开路点）沿向电源方向转到该交点，其转过的长度即对应的短路（开路）枝节长度"));
    strList2_y.append(QString("8. 其对应的两组解的求解做图过程请查阅上侧的对应的两个解的SmithChart。"));
    strList2_y.append("******************************************************************************");
    model2_y.setStringList(strList2_z);
}

void MainWindow::initChartPainter()
{
    qint32 width,height;
    height = imgHeight;
    width = height;
    pImg = new QPixmap(height,width);
    pPainter = new QPainter(pImg);
    pPainter->translate(width/2,height/2);
    SmithChart smithchart(height-80,width-80);
    smithchart.setPainter(pPainter);
    smithchart.drawSmithChart();
//    smithchart.drawOpenPoint();
//    smithchart.drawShortPoint();
//    smithchart.drawMatchPoint();
    //SmithChart for Resistor
    ui->label_SmithImage->setPixmap(*pImg);
    ui->label_BasicSmithChart->setPixmap(*pImg);
    ui->label_BasicSmithChart->setScaledContents(true);
    ui->label_quanterLamdaSmithChart->setPixmap(*pImg);
    ui->label_SingleStubSsmithChart->setPixmap(*pImg);
    ui->label_SingleStubSsmithChart->setScaledContents(true);
    ui->label_SingleStubSsmithChart_2->setPixmap(*pImg);
    ui->label_SingleStubSsmithChart_2->setScaledContents(true);
    ui->toolBox->setCurrentIndex(0);
    ui->label_SingleStubPsmithChart_1->setPixmap(*pImg);
    ui->label_SingleStubPsmithChart_1->setScaledContents(true);
    ui->label_SingleStubPsmithChart_3->setPixmap(*pImg);
    ui->label_SingleStubPsmithChart_3->setScaledContents(true);
    //Smithchart for g
    smithchart.drawSmithChart_2();
    ui->label_SmithImage_2->setPixmap(*pImg);
    ui->label_SingleStubPsmithChart_2->setPixmap(*pImg);
    ui->label_SingleStubPsmithChart_2->setScaledContents(true);
    ui->label_SingleStubPsmithChart_4->setPixmap(*pImg);
    ui->label_SingleStubPsmithChart_4->setScaledContents(true);
    ui->toolBox_2->setCurrentIndex(0);
    //for DoubleClick
    pImg4Basic = nullptr;
    pPainter4Basic = nullptr;
    step4Base = 0;
    pImg4QuarnterLamda = nullptr;
    pPainter4QuarnterLamda = nullptr;
    step4QuarnterLamda = 0;
    for(int i=0;i<2;++i)
    {
        pImg4PStub[i]=nullptr;
        pPainter4PStub[i]=nullptr;
        step4PStub[i]=0;
    }
    for(int i=0;i<4;++i)
    {
        pImg4PStub[i] = nullptr;
        pPainter4PStub[i] = nullptr;
        step4PStub[i] = 0;
    }
}

//void MainWindow::mouseMoveEvent(QMouseEvent *event)
//{
//    if(event->buttons()&Qt::LeftButton)
//    {
//        QPointF p1 = event->localPos();
//        qDebug()<<"p1="<<p1;
//        QPointF p2 = event->globalPos();
//        qDebug()<<"p2="<<p2;
//        QPointF p3 = event->pos();
//        qDebug()<<"p3="<<p3;
//        QPointF p4 = event->screenPos();
//        qDebug()<<"p4="<<p4;
//        QPointF p5 = event->windowPos();
//        qDebug()<<"p5="<<p5;
//    }
//    else
//    {

//    }
//}


void MainWindow::on_pbClear_clicked()
{
    clearInputs();
}

void MainWindow::on_pbCount_clicked()
{
    if(ui->chbZin->isChecked()||ui->chbVswr->isChecked()||ui->chbRCoef->isChecked())
    {
        if(ui->leZ0->text().isEmpty())
        {
            showWarning("请输入传输线的特性阻抗");
            return;
        }
        if(ui->leFreq->text().isEmpty())
        {
            showWarning("请输入传输线的工作频率");
            return;
        }
        if(ui->leDist->text().isEmpty())
        {
            showWarning("请输入距离终端负载的距离");
            return;
        }
        if(ui->leZl_real->text().isEmpty())
        {
            showWarning("请输入传输线的终端负载的实部");
            return;
        }
        if(ui->leZl_real->text().isEmpty())
        {
            showWarning("请输入传输线的终端负载的虚部");
            return;
        }
        double z0=ui->leZ0->text().toDouble();
        Complex zl(ui->leZl_real->text().toDouble(),ui->leZl_img->text().toDouble());
        double freq = ui->leFreq->text().toDouble()*1000000;//转换成Hz
        double d = ui->leDist->text().toDouble();
        double v = 300000000;//相速
        //TransmissionLine tl(z0,zl,v/freq);
        TransmissionLine tl(z0,zl,v/freq);
        //qDebug("传输线特性阻抗：%f\t工作频率:%f\t终端负载阻抗：%f+j%f\t距离终端负载距离：%fm \n",z0,freq,zl.getReal(),zl.getImag(),d);
        //显示输出结果的StringList
        //QStringList strList;  //只能显示当前的计算结果，如何显示历史结果呢？=》保留上一次的strList信息，即strList的声明周期不能局限在这个函数中
        QString str;
        str.sprintf("传输线特性阻抗：%f\t工作频率:%f\t终端负载阻抗：%f+j%f\t距离终端负载距离：%fm",z0,freq,zl.getReal(),zl.getImag(),d);
        resList1.append(str);
        str = QString("公式计算结果:");
        resList1.append(str);
        Complex zin;
        if(ui->chbZin->isChecked())
        {
            zin = tl.calZin(d);
            qDebug("在距离终端负载%fm处的输入阻抗为%f+j%f \n",d,zin.getReal(),zin.getImag());
            str.sprintf("在距离终端负载%fm处的输入阻抗为%f+j%f ",d,zin.getReal(),zin.getImag());
            resList1.append(str);
        }
        Complex rc;
        if(ui->chbRCoef->isChecked())
        {
            rc = tl.calRflectCoef(d);
            qDebug("在距离终端负载%fm处的反射系数为%f+j%f \n",d,rc.getReal(),rc.getImag());
            //QString str=QString("在距离终端负载%1m处的反射系数为%2+j%3 \n").arg(d).arg(rc.getReal()).arg(rc.getImag());
            qDebug() << QString("在距离终端负载%1m处的反射系数为%2+j%3").arg(d).arg(rc.getReal()).arg(rc.getImag());

            str.sprintf("在距离终端负载%fm处的反射系数为%f+j%f ",d,rc.getReal(),rc.getImag());
            resList1.append(str);
        }
        double rou;
        if(ui->chbVswr->isChecked())
        {
            rou = tl.calSwr(d);
            qDebug("在距离终端负载%fm处的驻波比为%f \n",d,rou);
            str.sprintf("在距离终端负载%fm处的驻波比为%f ",d,rou);
            resList1.append(str);
        }        
        //draw Smithchart background
        SmithChart smithchart(imgHeight-80,imgWidth-80);
        smithchart.setPainter(pPainter);
        smithchart.drawBackground();
        //draw Rcircle and XCircle， then draw Cross Point
        qreal resistor = zl.getReal()/z0;
        qreal x = zl.getImag()/z0;
        QPointF crossP = smithchart.getRandXCrossP(resistor,x);
        QPointF point;
        smithchart.resistorCircle(resistor,Qt::red);
        point = smithchart.getRandXAxisCrossp(resistor);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(resistor));
        smithchart.xCircle(x,Qt::red);
        point = smithchart.getRcoefandXCrossp(1,x);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(x));
        //draw rcoef circle
        smithchart.drawCrossPoint(crossP);
        smithchart.setText(crossP,QString("(%1,%2)").arg(crossP.x()).arg(crossP.y()));
        qreal rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
        resList1.append(QString("SmithChart 分析结果："));
        smithchart.rCoefCircle(rCoef,Qt::blue);
        //draw SWR Rcircle
        struct CrossPoints crossP2;
        crossP2 = smithchart.getRcoefandXAxisCrossp(rCoef);
        QPointF point2 = crossP2.crossP[0];
        qreal swr = smithchart.CalAndDrawSwrCircle(rCoef,Qt::darkMagenta);
        if(ui->chbVswr->isChecked())
            resList1.append(QString("\t驻波比结果:%1").arg(swr));
        smithchart.setText(point2,QString("%1").arg(swr));
        //read Zin
        qreal startAng = atan2(crossP.y(),crossP.x());
        startAng = startAng/M_PI*180;
        if(startAng<0)
            startAng = startAng + 360;
        qreal spanAng = 2*tl.getBeta()*d;
        spanAng=spanAng/M_PI*180;
        while(spanAng>=360.0)
            spanAng = spanAng-360;
        // -spanAngle is toward Power
        QPointF resPoint;
        resPoint = smithchart.calAndDrawArc(rCoef,startAng,-spanAng,Qt::green);
        if(ui->chbRCoef->isChecked())
            resList1.append(QString("\t反射系数结果:%1+j%2").arg(resPoint.x()).arg(resPoint.y()));
        // cal the R of CrossP2
        resistor = smithchart.calRfromCrossp(resPoint);
        smithchart.resistorCircle(resistor,Qt::cyan);
        point = smithchart.getRandXAxisCrossp(resistor);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(resistor));
        //cal the X of CrossP2
        x = smithchart.calXfromCrossp(resPoint);
        smithchart.xCircle(x,Qt::cyan);
        point = smithchart.getRcoefandXCrossp(1,x);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(x));
        if(ui->chbZin->isChecked())
        {
            resList1.append(QString("\t输入阻抗:%1+j%2").arg(resistor*z0).arg(x*z0));
        }
        ui->label_BasicSmithChart->setPixmap(*pImg);
        //显示输出结果
        model1.setStringList(resList1);
    }
    else
    {
        showWarning("请勾选输入阻抗、反射系数或驻波比等复选框，给出计算要求");
        return;
    }
}

void MainWindow::on_pb_CountQuanLamda_clicked()
{
    if(ui->le_z0->text().isEmpty())
    {
        showWarning("请输入传输线的特性阻抗");
        return;
    }
    if(ui->le_Freq->text().isEmpty())
    {
        showWarning("请输入传输线的工作频率");
        return;
    }
    if(ui->le_zlR->text().isEmpty())
    {
        showWarning("请输入传输线的终端负载的实部");
        return;
    }
    if(ui->le_zlim->text().isEmpty())
    {
        showWarning("请输入传输线的终端负载的虚部");
        return;
    }
    double z0=ui->le_z0->text().toDouble();
    Complex zl(ui->le_zlR->text().toDouble(),ui->le_zlim->text().toDouble());
    double freq = ui->le_Freq->text().toDouble()*1000000;//转换成Hz
    double v = 300000000.0;//相速
    QString str;
    str = QString("1/4λ阻抗匹配：工作频率:%1MHz\t特性阻抗:%2Ω\t终端负载阻抗：%3+j%4(Ω)\t").arg(freq/1000000).arg(z0).arg(zl.getReal()).arg(zl.getImag());
    //qDebug()<<str;

    //显示输出结果的StringList
    //QStringList strList;  //只能显示当前的计算结果，如何显示历史结果呢？=》保留上一次的strList信息，即strList的声明周期不能局限在这个函数中
    resList2.append(str);
    str = QString("公式计算结果:");
    resList2.append(str);
    //TransmissionLine tl(z0,zl,v/freq);
    lamdaDiv4 tl(z0,zl,v/freq);
    bool ok;
    ok = tl.calMatchParameter();
    if(ok)
        str = QString("结果: 枝节距终端负载距离l1=%1m\t 和枝节特性阻抗z01=%2Ω").arg(tl.getL1()).arg(tl.getZ01());
    else
        str = QString("已匹配，毋需加入匹配枝节");
    resList2.append(str);
    //SmithChart
    //draw Smithchart background
    SmithChart smithchart(imgHeight-80,imgWidth-80);
    smithchart.setPainter(pPainter);
    smithchart.drawBackground();
    //draw Rcircle and XCircle， then draw Cross Point
    qreal resistor = zl.getReal()/z0;
    qreal x = zl.getImag()/z0;
    QPointF crossP = smithchart.getRandXCrossP(resistor,x);
    QPointF point;
    smithchart.resistorCircle(resistor,Qt::red);
    point = smithchart.getRandXAxisCrossp(resistor);
    smithchart.drawCrossPoint(point);
    smithchart.setText(point,QString("%1").arg(resistor));
    smithchart.xCircle(x,Qt::red);
    point = smithchart.getRcoefandXCrossp(1,x);
    smithchart.drawCrossPoint(point);
    smithchart.setText(point,QString("%1").arg(x));
    //draw rcoef circle
    smithchart.drawCrossPoint(crossP);
    smithchart.setText(crossP,QString("(%1,%2)").arg(crossP.x()).arg(crossP.y()));
    qreal rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
    resList1.append(QString("SmithChart 分析结果："));
    smithchart.rCoefCircle(rCoef,Qt::blue);
    //draw SWR Rcircle
    struct CrossPoints crossP2;
    crossP2 = smithchart.getRcoefandXAxisCrossp(rCoef);
    QPointF point2 = crossP2.crossP[0];
    qreal swr1 = smithchart.CalAndDrawSwrCircle(rCoef,Qt::darkMagenta);
    smithchart.setText(point2,QString("%1").arg(swr1));
    QPointF point3 = crossP2.crossP[1];
    qreal swr2 = smithchart.CalAndDrawSwrCircle(-rCoef,Qt::darkMagenta);
    smithchart.setText(point3,QString("%1").arg(swr2));
    //cal phi
    qreal startPhi,stopPhi1,stopPhi2;
    qreal spanPhi;
    startPhi =smithchart.getRcoefPhiFromCrossp(crossP);
    stopPhi1 = 0;
    stopPhi2 = M_PI;
    //stopPhi1 =smithchart.getRcoefPhiFromCrossp(point2);
    //stopPhi2 =smithchart.getRcoefPhiFromCrossp(point3);
    resList2.append(QString("SmithChart作图的结果："));
    qreal Rin;
    bool isHalfLamda;
    if(startPhi<=M_PI)
    {
        spanPhi = stopPhi1-startPhi;
        Rin = swr1;
        isHalfLamda = false;
    }
    else
    {
        spanPhi = stopPhi2-startPhi;
        Rin = swr2;
        isHalfLamda= true;
    }
    QPointF PForLamda = smithchart.calAndDrawLineFromOrign(crossP,Qt::darkBlue);
    smithchart.drawCrossPoint(PForLamda);
    qreal phiLamda = smithchart.calLamdaFromCrossp(PForLamda);
    smithchart.setText(PForLamda,QString("%1λ").arg(phiLamda));
    smithchart.calAndDrawArc(rCoef,startPhi/M_PI*180,spanPhi/M_PI*180,Qt::black);
    if(isHalfLamda)
    {
        resList2.append(QString("\t从终端负载位置%1λ转到0.5λ：0.5λ-%2λ=%3λ").arg(phiLamda).arg(phiLamda).arg(0.5-phiLamda));
        resList2.append(QString("\t所以1/4λ阻抗变换器加在距离终端负载ZL %1m处").arg((0.5-phiLamda)*tl.getLamda()));
    }
    else
    {
        resList2.append(QString("\t从终端负载位置%1λ转到0.25λ：0.25λ-%1λ=%1λ").arg(phiLamda).arg(phiLamda).arg(0.25-phiLamda));
        resList2.append(QString("\t所以1/4λ阻抗变换器加在距离终端负载ZL %1m处").arg((0.25-phiLamda)*tl.getLamda()));
    }
    resList2.append(QString("\t因为加入1/4λ阻抗变换器距离终端负载ZL %1m处的归一化输入阻抗为%2Ω").arg((0.5-phiLamda)*tl.getLamda()).arg(Rin));
    qreal z01 = sqrt(Rin)*z0;
    resList2.append(QString("\t1/4λ阻抗变换器的特性阻抗为%1Ω").arg(z01));
    ui->label_quanterLamdaSmithChart->setPixmap(*pImg);
    //Display Result
    model2.setStringList(resList2);
}

void MainWindow::on_pb_clear_clicked()
{
    ui->le_z0->setText("");
    ui->le_zlR->setText("");
    ui->le_zlim->setText("");
    ui->le_Freq->setText("");
    ui->ls_res->reset();
    resList2.clear();
    model2.setStringList(resList2);
}

void MainWindow::on_pbClear_series_clicked()
{
    ui->leZ0_Series->setText("");
    ui->leFreq_Series->setText("");
    ui->leZl_real_Series->setText("");
    ui->leZl_img_Series->setText("");
    resList3.clear();
    model3.setStringList(resList3);
}

void MainWindow::on_pbCount_Series_clicked()
{
    if(ui->leZ0_Series->text().isEmpty())
    {
        showWarning("请输入传输线的特性阻抗");
        return;
    }
    if(ui->leFreq_Series->text().isEmpty())
    {
        showWarning("请输入传输线的工作频率");
        return;
    }
    if(ui->leZl_real_Series->text().isEmpty())
    {
        showWarning("请输入传输线的终端负载的实部");
        return;
    }
    if(ui->leZl_img_Series->text().isEmpty())
    {
        showWarning("请输入传输线的终端负载的虚部");
        return;
    }
    resList3.clear();
    double z0=ui->leZ0_Series->text().toDouble();
    Complex zl(ui->leZl_real_Series->text().toDouble(),ui->leZl_img_Series->text().toDouble());
    double freq = ui->leFreq_Series->text().toDouble()*1000000;//转换成Hz
    double v = 300000000.0;//相速
    QString str;
    str = QString("单枝节串联匹配：工作频率:%1MHz\t特性阻抗:%2Ω\t终端负载阻抗：%3+j%4(Ω)\t").arg(freq/1000000).arg(z0).arg(zl.getReal()).arg(zl.getImag());
    //qDebug()<<str;
    //显示输出结果的StringList
    //QStringList strList;  //只能显示当前的计算结果，如何显示历史结果呢？=》保留上一次的strList信息，即strList的声明周期不能局限在这个函数中
    resList3.append(str);
    str = QString("公式计算结果:");
    resList3.append(str);
    //TransmissionLine tl(z0,zl,v/freq);
    SingleStubSeriesMatching tl(z0,zl,v/freq);
    if(ui->radio_shortStub->isChecked())
        tl.setStubType(shortStub);
    if(ui->radio_openStub->isChecked())
        tl.setStubType(openStub);

    struct StubMatchResult res[2];
    bool ok;
    ok = tl.calMatchParameter();
    if(ok)
    {
        tl.getMatchRes(res);
        for(int i=0;i<2;++i)
        {
            str = QString("Result %1: Pos l1=%2m\t stub l2=%3m").arg(i+1).arg(res[i].stubPos).arg(res[i].stubLen);
            resList3.append(str);
        }
        //show SmithChart
        resList3.append("****************************************");
        resList3.append(QString("SmithChart做图方法如下："));
        resList3.append(QString("1. 求出终端负载的归一化阻抗，然后绘制电阻圆和电抗圆"));
        resList3.append(QString("2. 找到终端负载电阻圆和电抗圆的交点，过交点绘制出终端负载的反射系数圆"));
        resList3.append(QString("3. 绘制电阻为1的匹配圆，找到并标记处其与终端负载的反射系数圆的交点（一般会有2个）"));
        resList3.append(QString("4. 从负载位置向电源方向转到与匹配圆的2个交点（对应两组解），所转过的长度即为串联单枝节加入位置"));
        resList3.append(QString("5. 若已选中一个与匹配圆相交的一个交点，则绘制出过该点的电抗圆，读取其对应的归一化电抗值"));
        resList3.append(QString("6. 为了抵消该归一化电抗，绘制出与之取反的电抗值所对应的电抗圆，并绘制出其与反射系数为1的圆的交点"));
        resList3.append(QString("7. 从短路点（开路点）沿向电源方向转到该交点，其转过的长度即对应的短路（开路）枝节长度"));
        resList3.append(QString("8. 其对应的两组解的求解做图过程请查阅右侧的对应的两个解的SmithChart。"));
        resList3.append("****************************************");
        resList3.append("具体做图的结果如下，亦可通过右图读取对应的数值");
        //draw Smithchart background
        SmithChart smithchart(imgHeight-80,imgWidth-80);
        smithchart.setPainter(pPainter);
        smithchart.drawBackground();
        //draw Rcircle and XCircle， then draw Cross Point
        qreal resistor = zl.getReal()/z0;
        qreal x = zl.getImag()/z0;
        QPointF crossP = smithchart.getRandXCrossP(resistor,x);
        QPointF point;
        smithchart.resistorCircle(resistor,Qt::red);
        point = smithchart.getRandXAxisCrossp(resistor);
        smithchart.drawCrossPoint(point);
        smithchart.xCircle(x,Qt::red);
        point = smithchart.getRcoefandXCrossp(1,x);
        smithchart.drawCrossPoint(point);
        //draw rcoef circle
        smithchart.drawCrossPoint(crossP);
        qreal rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
        smithchart.rCoefCircle(rCoef,Qt::blue);
        //draw match Rcircle and Count crossPoint;
        struct CrossPoints crossP2;
        smithchart.resistorCircle(1,Qt::cyan);
        crossP2 = smithchart.getRcoefandRCrossP(rCoef,1);
        for(int i=0;i<crossP2.nPoints;++i)
        {
            smithchart.drawCrossPoint(crossP2.crossP[i]);
        }
        // from CrossP to crossP2.crossP[0]
        qreal startAng = atan2(crossP.y(),crossP.x());
        startAng = startAng/M_PI*180;
        if(startAng<0)
            startAng = startAng + 360;
        qreal stopAng = atan2(crossP2.crossP[0].y(),crossP2.crossP[0].x());
        stopAng = stopAng/M_PI*180;
        if(stopAng<0)
            stopAng = stopAng +360;
        qreal spanAng = stopAng-startAng;
        if(spanAng<0)
            spanAng = -spanAng;
        // -spanAngle is toward Power
        QPointF resPoint;
        resPoint = smithchart.calAndDrawArc(rCoef,startAng,-spanAng,Qt::green);
        //cal and draw the position of stub
        QPointF PForLamda1 = smithchart.calAndDrawLineFromOrign(crossP,Qt::darkBlue);
        smithchart.drawCrossPoint(PForLamda1);
        qreal phiLamda1 = smithchart.calLamdaFromCrossp(PForLamda1);
        smithchart.setText(PForLamda1,QString("%1λ").arg(phiLamda1));
        QPointF PForLamda2 = smithchart.calAndDrawLineFromOrign(crossP2.crossP[0],Qt::darkBlue);
        smithchart.drawCrossPoint(PForLamda2);
        qreal phiLamda2 = smithchart.calLamdaFromCrossp(PForLamda2);
        smithchart.setText(PForLamda2,QString("%1λ").arg(phiLamda2));
        //cal and draw x circle
        qreal x1 = smithchart.calXfromCrossp(crossP2.crossP[0]);
        smithchart.xCircle(x1,Qt::blue);
        QPointF x1Point = smithchart.getRcoefandXCrossp(1,x1);
        smithchart.drawCrossPoint(x1Point);
        smithchart.setText(x1Point,QString("%1").arg(x1));
        qreal x2 = -x1;
        smithchart.xCircle(x2,Qt::blue);
        QPointF x2Point = smithchart.getRcoefandXCrossp(1,x2);
        smithchart.calAndDrawLineFromOrign(x2Point,Qt::darkBlue);
        smithchart.drawCrossPoint(x2Point);
        //smithchart.setText(x2Point,QString("%1").arg(x2));
        qreal posLamda1 = smithchart.calLamdaFromCrossp(x2Point);
        smithchart.setText(x2Point,QString("%1λ").arg(posLamda1));
        qreal stubPos1,stubPos2;
        qreal stubLen1=0,stubLen2=0;

        if(ui->radio_shortStub->isChecked())
        {
            smithchart.drawShortPoint();
            startAng = 180;
            stopAng = atan2(x2Point.y(),x2Point.x());
            stopAng = stopAng/M_PI*180;
            qreal spanAng;
            if(stopAng<0)
                spanAng = 180-stopAng;
            else
                spanAng = stopAng-startAng;
            if(spanAng<0)
                spanAng = -spanAng;
            resPoint = smithchart.calAndDrawArc(1,startAng,-spanAng,Qt::green);
            if(phiLamda1>phiLamda2)
            {
                stubPos1 =(0.5- (phiLamda1-phiLamda2))*v/freq;
            }
            else
            {
                stubPos1 = (phiLamda2-phiLamda1)*v/freq;
            }
            stubLen1 = posLamda1*v/freq;
        }
        else
        {
            smithchart.drawOpenPoint();
            startAng = 0;
            stopAng = atan2(x2Point.y(),x2Point.x());
            stopAng = stopAng/M_PI*180;
            qreal spanAng;
            if(stopAng<0)
                spanAng = -stopAng;
            else
                spanAng = 360-stopAng;
            if(spanAng<0)
                spanAng = -spanAng;
            resPoint = smithchart.calAndDrawArc(1,startAng,-spanAng,Qt::green);
            if(phiLamda1>phiLamda2)
            {
                stubPos1 =(0.5- (phiLamda1-phiLamda2))*v/freq;
            }
            else
            {
                stubPos1 = (phiLamda2-phiLamda1)*v/freq;
            }
            if(posLamda1>0.25)
                stubLen1 = (posLamda1-0.25)*v/freq;
            else
                stubLen1 = (posLamda1+0.25)*v/freq;
        }
        str = QString("Result 1: Pos l1=%1m\t stub l2=%2m").arg(stubPos1).arg(stubLen1);
        resList3.append(str);

        ui->label_SingleStubSsmithChart->setPixmap(*pImg);
        /* ******************************
         * the second resolution        *
         * ******************************/
        smithchart.drawBackground();
        //draw Rcircle and XCircle， then draw Cross Point
        resistor = zl.getReal()/z0;
        x = zl.getImag()/z0;
        crossP = smithchart.getRandXCrossP(resistor,x);
        smithchart.resistorCircle(resistor,Qt::red);
        point = smithchart.getRandXAxisCrossp(resistor);
        smithchart.drawCrossPoint(point);
        smithchart.xCircle(x,Qt::red);
        point = smithchart.getRcoefandXCrossp(1,x);
        smithchart.drawCrossPoint(point);
        //draw rcoef circle
        smithchart.drawCrossPoint(crossP);
        rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
        smithchart.rCoefCircle(rCoef,Qt::blue);
        //draw match Rcircle and Count crossPoint;
        smithchart.resistorCircle(1,Qt::cyan);
        crossP2 = smithchart.getRcoefandRCrossP(rCoef,1);
        for(int i=0;i<crossP2.nPoints;++i)
        {
            smithchart.drawCrossPoint(crossP2.crossP[i]);
        }
        // from CrossP to crossP2.crossP[1]
        startAng = atan2(crossP.y(),crossP.x());
        startAng = startAng/M_PI*180;
        if(startAng<0)
            startAng = startAng + 360;
        stopAng = atan2(crossP2.crossP[1].y(),crossP2.crossP[1].x());
        stopAng = stopAng/M_PI*180;
        if(stopAng<0)
            stopAng = stopAng +360;
        spanAng = stopAng-startAng;
        if(spanAng<0)
            spanAng = -spanAng;
        else
            spanAng = 360-spanAng;
        // -spanAngle is toward Power
        resPoint = smithchart.calAndDrawArc(rCoef,startAng,-spanAng,Qt::green);
        //cal and draw the position of stub
        PForLamda1 = smithchart.calAndDrawLineFromOrign(crossP,Qt::darkBlue);
        smithchart.drawCrossPoint(PForLamda1);
        phiLamda1 = smithchart.calLamdaFromCrossp(PForLamda1);
        smithchart.setText(PForLamda1,QString("%1λ").arg(phiLamda1));
        PForLamda2 = smithchart.calAndDrawLineFromOrign(crossP2.crossP[1],Qt::darkBlue);
        smithchart.drawCrossPoint(PForLamda2);
        phiLamda2 = smithchart.calLamdaFromCrossp(PForLamda2);
        smithchart.setText(PForLamda2,QString("%1λ").arg(phiLamda2));
        //cal and draw x circle
        x1 = smithchart.calXfromCrossp(crossP2.crossP[1]);
        smithchart.xCircle(x1,Qt::blue);
        x1Point = smithchart.getRcoefandXCrossp(1,x1);
        smithchart.drawCrossPoint(x1Point);
        smithchart.setText(x1Point,QString("%1").arg(x1));
        x2 = -x1;
        smithchart.xCircle(x2,Qt::blue);
        x2Point = smithchart.getRcoefandXCrossp(1,x2);
        smithchart.calAndDrawLineFromOrign(x2Point,Qt::darkBlue);
        smithchart.drawCrossPoint(x2Point);
        //smithchart.setText(x2Point,QString("%1").arg(x2));
        posLamda1 = smithchart.calLamdaFromCrossp(x2Point);
        smithchart.setText(x2Point,QString("%1λ").arg(posLamda1));
        if(ui->radio_shortStub->isChecked())
        {
            smithchart.drawShortPoint();
            startAng = 180;
            stopAng = atan2(x2Point.y(),x2Point.x());
            stopAng = stopAng/M_PI*180;
            qreal spanAng;
            if(stopAng<0)
                spanAng = 180-stopAng;
            else
                spanAng = stopAng-startAng;
            if(spanAng<0)
                spanAng = -spanAng;
            else
                spanAng = 360-spanAng;
            resPoint = smithchart.calAndDrawArc(1,startAng,-spanAng,Qt::green);
            if(phiLamda1>phiLamda2)
            {
                stubPos1 =(0.5- (phiLamda1-phiLamda2))*v/freq;
            }
            else
            {
                stubPos1 = (phiLamda2-phiLamda1)*v/freq;
            }
            stubLen1 = posLamda1*v/freq;
        }
        else
        {
            smithchart.drawOpenPoint();
            startAng = 0;
            stopAng = atan2(x2Point.y(),x2Point.x());
            stopAng = stopAng/M_PI*180;
            qreal spanAng;
            if(stopAng<0)
                spanAng = -stopAng;
            else
                spanAng = 360-stopAng;
            if(spanAng<0)
                spanAng = -spanAng;
            resPoint = smithchart.calAndDrawArc(1,startAng,-spanAng,Qt::green);
            if(phiLamda1>phiLamda2)
            {
                stubPos1 =(0.5- (phiLamda1-phiLamda2))*v/freq;
            }
            else
            {
                stubPos1 = (phiLamda2-phiLamda1)*v/freq;
            }
            if(posLamda1>0.25)
                stubLen1 = (posLamda1-0.25)*v/freq;
            else
                stubLen1 = (posLamda1+0.25)*v/freq;
        }
        str = QString("Result 2: Pos l1=%1m\t stub l2=%2m").arg(stubPos1).arg(stubLen1);
        resList3.append(str);
        ui->label_SingleStubSsmithChart_2->setPixmap(*pImg);
    }
    else
    {
        str = QString("已匹配，毋需加入匹配枝节");
        resList3.append(str);
    }
    model3.setStringList(resList3);   
}

void MainWindow::on_pbClear_Parallel_clicked()
{
    ui->leZ0_Parallel->setText("");
    ui->leFreq_Parallel->setText("");
    ui->leZl_real_Parallel->setText("");
    ui->leZl_img_Parallel->setText("");
    resList4.clear();
    model4.setStringList(resList4);
}

void MainWindow::on_pbCount_Parallel_clicked()
{
    if(ui->leZ0_Parallel->text().isEmpty())
    {
        showWarning("请输入传输线的特性阻抗");
        return;
    }
    if(ui->leFreq_Parallel->text().isEmpty())
    {
        showWarning("请输入传输线的工作频率");
        return;
    }
    if(ui->leZl_real_Parallel->text().isEmpty())
    {
        showWarning("请输入传输线的终端负载的实部");
        return;
    }
    if(ui->leZl_img_Parallel->text().isEmpty())
    {
        showWarning("请输入传输线的终端负载的虚部");
        return;
    }
    double z0=ui->leZ0_Parallel->text().toDouble();
    Complex zl(ui->leZl_real_Parallel->text().toDouble(),ui->leZl_img_Parallel->text().toDouble());
    double freq = ui->leFreq_Parallel->text().toDouble()*1000000;//转换成Hz
    double v = 300000000.0;//相速
    QString str;
    str = QString("单枝节串联匹配：工作频率:%1MHz\t特性阻抗:%2Ω\t终端负载阻抗：%3+j%4(Ω)\t").arg(freq/1000000).arg(z0).arg(zl.getReal()).arg(zl.getImag());
    //qDebug()<<str;
    //显示输出结果的StringList
    //QStringList strList;  //只能显示当前的计算结果，如何显示历史结果呢？=》保留上一次的strList信息，即strList的声明周期不能局限在这个函数中
    resList4.clear();
    resList4.append(str);
    str = QString("公式计算结果:");
    resList4.append(str);
    //TransmissionLine tl(z0,zl,v/freq);
    SingleStubParallelMatching tl(z0,zl,v/freq);
    if(ui->radio_shortStub->isChecked())
        tl.setStubType(shortStub);
    if(ui->radio_openStub->isChecked())
        tl.setStubType(openStub);

    struct StubMatchResult res[2];
    bool ok;
    ok = tl.calMatchParameter();
    if(ok)
    {
        tl.getMatchRes(res);
        for(int i=0;i<2;++i)
        {
            str = QString("Result %1: Pos l1=%2m\t stub l2=%3m").arg(i+1).arg(res[i].stubPos).arg(res[i].stubLen);
            resList4.append(str);
        }
        // No.1 solution in Z circle
        resList4.append("SmithChart具体做图的结果如下，亦可通过右图读取对应的数值");
        //draw Smithchart background
        SmithChart smithchart(imgHeight-80,imgWidth-80);
        smithchart.setPainter(pPainter);
        smithchart.drawBackground();
        //draw Rcircle and XCircle， then draw Cross Point
        qreal resistor = zl.getReal()/z0;
        qreal x = zl.getImag()/z0;
        smithchart.resistorCircle(resistor,Qt::red);
        smithchart.xCircle(x,Qt::red);
        QPointF point = smithchart.getRandXCrossP(resistor,x);
        smithchart.drawCrossPoint(point);
        QPointF crossP;
        crossP = -point;
        //draw rcoef circle
        smithchart.drawCrossPoint(crossP);
        qreal rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
        smithchart.rCoefCircle(rCoef,Qt::blue);
        //draw rotate 180° to Y circle
        smithchart.drawLine(point,crossP,Qt::darkBlue);
        //draw match Rcircle and Count crossPoint;
        struct CrossPoints crossP2;
        smithchart.resistorCircle(1,Qt::cyan);
        crossP2 = smithchart.getRcoefandRCrossP(rCoef,1);
        for(int i=0;i<crossP2.nPoints;++i)
        {
            smithchart.drawCrossPoint(crossP2.crossP[i]);
        }
        // from CrossP to crossP2.crossP[0]
        qreal startAng = atan2(crossP.y(),crossP.x());
        startAng = startAng/M_PI*180;
        if(startAng<0)
            startAng = startAng + 360;
        qreal stopAng = atan2(crossP2.crossP[0].y(),crossP2.crossP[0].x());
        stopAng = stopAng/M_PI*180;
        if(stopAng<0)
            stopAng = stopAng +360;
        qreal spanAng = stopAng-startAng;
        if(spanAng<0)
            spanAng = -spanAng;
        // -spanAngle is toward Power
        QPointF resPoint;
        resPoint = smithchart.calAndDrawArc(rCoef,startAng,-spanAng,Qt::green);
        //cal and draw the position of stub
        QPointF PForLamda1 = smithchart.calAndDrawLineFromOrign(crossP,Qt::darkBlue);
        smithchart.drawCrossPoint(PForLamda1);
        qreal phiLamda1 = smithchart.calLamdaFromCrossp(PForLamda1);
        smithchart.setText(PForLamda1,QString("%1λ").arg(phiLamda1));
        QPointF PForLamda2 = smithchart.calAndDrawLineFromOrign(crossP2.crossP[0],Qt::darkBlue);
        smithchart.drawCrossPoint(PForLamda2);
        qreal phiLamda2 = smithchart.calLamdaFromCrossp(PForLamda2);
        smithchart.setText(PForLamda2,QString("%1λ").arg(phiLamda2));
        //cal and draw x circle
        qreal x1 = smithchart.calXfromCrossp(crossP2.crossP[0]);
        smithchart.xCircle(x1,Qt::blue);
        QPointF x1Point = smithchart.getRcoefandXCrossp(1,x1);
        smithchart.drawCrossPoint(x1Point);
        smithchart.setText(x1Point,QString("%1").arg(x1));
        qreal x2 = -x1;
        smithchart.xCircle(x2,Qt::blue);
        QPointF x2Point1 = smithchart.getRcoefandXCrossp(1,x2);
        smithchart.drawCrossPoint(x2Point1);
        smithchart.setText(x2Point1,QString("%1").arg(x2));
        QPointF x2Point2 = -x2Point1;
        smithchart.drawLine(x2Point1,x2Point2,Qt::darkBlue);
        smithchart.drawCrossPoint(x2Point2);
        qreal posLamda1 = smithchart.calLamdaFromCrossp(x2Point2);
        smithchart.setText(x2Point2,QString("%1λ").arg(posLamda1));
        qreal stubPos1,stubPos2;
        qreal stubLen1=0,stubLen2=0;
        if(ui->radio_shortStub_2->isChecked())
        {
            smithchart.drawShortPoint();
            startAng = 180;
            stopAng = atan2(x2Point2.y(),x2Point2.x());
            stopAng = stopAng/M_PI*180;
            qreal spanAng;
            if(stopAng<0)
                spanAng = 180-stopAng;
            else
                spanAng = stopAng-startAng;
            if(spanAng<0)
                spanAng = -spanAng;
            else
                spanAng = 360-spanAng;
            resPoint = smithchart.calAndDrawArc(1,startAng,-spanAng,Qt::green);
            if(phiLamda1>phiLamda2)
            {
                stubPos1 =(0.5- (phiLamda1-phiLamda2))*v/freq;
            }
            else
            {
                stubPos1 = (phiLamda2-phiLamda1)*v/freq;
            }
            stubLen1 = posLamda1*v/freq;
        }
        else
        {
            smithchart.drawOpenPoint();
            startAng = 0;
            stopAng = atan2(x2Point2.y(),x2Point2.x());
            stopAng = stopAng/M_PI*180;
            qreal spanAng;
            if(stopAng<0)
                spanAng = -stopAng;
            else
                spanAng = 360-stopAng;
            if(spanAng<0)
                spanAng = -spanAng;
            resPoint = smithchart.calAndDrawArc(1,startAng,-spanAng,Qt::green);
            if(phiLamda1>phiLamda2)
            {
                stubPos1 =(0.5- (phiLamda1-phiLamda2))*v/freq;
            }
            else
            {
                stubPos1 = (phiLamda2-phiLamda1)*v/freq;
            }
            if(posLamda1>0.25)
                stubLen1 = (posLamda1-0.25)*v/freq;
            else
                stubLen1 = (posLamda1+0.25)*v/freq;
        }
        str = QString("阻抗圆图求解 Result 1: Pos l1=%1m\t stub l2=%2m").arg(stubPos1).arg(stubLen1);
        resList4.append(str);
        ui->label_SingleStubPsmithChart_1->setPixmap(*pImg);

        //No.1 sloution in Y circle
        //draw Smithchart background
        smithchart.setPainter(pPainter);
        smithchart.drawBackground();
        //draw Gcircle and bCircle， then draw Cross Point
        qreal g,b;
        g = tl.calZl2Y().getReal()*z0;
        b = tl.calZl2Y().getImag()*z0;
        smithchart.gCircle(g,Qt::red);
        smithchart.bCircle(b,Qt::red);
        crossP = smithchart.getGandBCrossP(g,b);
        smithchart.drawCrossPoint(point);
        //draw rcoef circle
        smithchart.drawCrossPoint(crossP);
        rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
        smithchart.rCoefCircle(rCoef,Qt::blue);
        //draw match gcircle and Count crossPoint;
        smithchart.gCircle(1,Qt::cyan);
        crossP2 = smithchart.getRcoefandGCrossP(rCoef,1);
        for(int i=0;i<crossP2.nPoints;++i)
        {
            smithchart.drawCrossPoint(crossP2.crossP[i]);
        }
        // from CrossP to crossP2.crossP[1]
        startAng = atan2(crossP.y(),crossP.x());
        startAng = startAng/M_PI*180;
        if(startAng<0)
            startAng = startAng + 360;
        stopAng = atan2(crossP2.crossP[1].y(),crossP2.crossP[1].x());
        stopAng = stopAng/M_PI*180;
        if(stopAng<0)
            stopAng = stopAng +360;
        spanAng = stopAng-startAng;
        if(spanAng<0)
            spanAng = -spanAng;
        else
            spanAng = 360-spanAng;
        // -spanAngle is toward Power
        resPoint = smithchart.calAndDrawArc(rCoef,startAng,-spanAng,Qt::green);
        //cal and draw the position of stub
        PForLamda1 = smithchart.calAndDrawLineFromOrign(crossP,Qt::darkBlue);
        smithchart.drawCrossPoint(PForLamda1);
        phiLamda1 = smithchart.calLamdaFromCrossp(PForLamda1);
        smithchart.setText(PForLamda1,QString("%1λ").arg(phiLamda1));
        PForLamda2 = smithchart.calAndDrawLineFromOrign(crossP2.crossP[1],Qt::darkBlue);
        smithchart.drawCrossPoint(PForLamda2);
        phiLamda2 = smithchart.calLamdaFromCrossp(PForLamda2);
        smithchart.setText(PForLamda2,QString("%1λ").arg(phiLamda2));
        //cal and draw bcircle
        qreal b1 = smithchart.calBfromCrossp(crossP2.crossP[1]);
        smithchart.bCircle(b1,Qt::blue);
        QPointF b1Point = smithchart.getRcoefandXCrossp(1,b1);
        smithchart.drawCrossPoint(b1Point);
        smithchart.setText(b1Point,QString("%1").arg(b1));
        qreal b2 = -b1;
        smithchart.bCircle(b2,Qt::blue);
        QPointF b2Point = smithchart.getRcoefandBCrossp(1,b2);
        smithchart.drawCrossPoint(b2Point);
        //smithchart.setText(b2Point,QString("%1").arg(b2));
        smithchart.calAndDrawLineFromOrign(b2Point,Qt::darkBlue);
        posLamda1 = smithchart.calLamdaFromCrossp(b2Point);
        smithchart.setText(b2Point,QString("%1λ").arg(posLamda1));
        if(ui->radio_shortStub_2->isChecked())
        {
            smithchart.drawShortPoint();
            startAng = 180;
            stopAng = atan2(b2Point.y(),b2Point.x());
            stopAng = stopAng/M_PI*180;
            qreal spanAng;
            if(stopAng<0)
                spanAng = 180-stopAng;
            else
                spanAng = stopAng-startAng;
            if(spanAng<0)
                spanAng = -spanAng;
            resPoint = smithchart.calAndDrawArc(1,startAng,-spanAng,Qt::green);
            if(phiLamda1>phiLamda2)
            {
                stubPos2 =(0.5- (phiLamda1-phiLamda2))*v/freq;
            }
            else
            {
                stubPos2 = (phiLamda2-phiLamda1)*v/freq;
            }
            stubLen2 = posLamda1*v/freq;
        }
        else
        {
            smithchart.drawOpenPoint();
            startAng = 0;
            stopAng = atan2(b2Point.y(),b2Point.x());
            stopAng = stopAng/M_PI*180;
            qreal spanAng;
            if(stopAng<0)
                spanAng = -stopAng;
            else
                spanAng = 360-stopAng;
            if(spanAng<0)
                spanAng = -spanAng;
            resPoint = smithchart.calAndDrawArc(1,startAng,-spanAng,Qt::green);
            if(phiLamda1>phiLamda2)
            {
                stubPos2 =(0.5- (phiLamda1-phiLamda2))*v/freq;
            }
            else
            {
                stubPos2= (phiLamda2-phiLamda1)*v/freq;
            }
            if(posLamda1>0.25)
                stubLen2 = (posLamda1-0.25)*v/freq;
            else
                stubLen2 = (posLamda1+0.25)*v/freq;
        }
        str = QString("导纳圆图求解 Result 1: Pos l1=%1m\t stub l2=%2m").arg(stubPos2).arg(stubLen2);
        resList4.append(str);
        ui->label_SingleStubPsmithChart_2->setPixmap(*pImg);

        // No.2 solution in Z circle
        //draw Smithchart background
        smithchart.setPainter(pPainter);
        smithchart.drawBackground();
        //draw Rcircle and XCircle， then draw Cross Point
        smithchart.resistorCircle(resistor,Qt::red);
        smithchart.xCircle(x,Qt::red);
        point = smithchart.getRandXCrossP(resistor,x);
        smithchart.drawCrossPoint(point);
        crossP = -point;
        //draw rcoef circle
        smithchart.drawCrossPoint(crossP);
        rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
        smithchart.rCoefCircle(rCoef,Qt::blue);
        //draw rotate 180° to Y circle
        smithchart.drawLine(point,crossP,Qt::darkBlue);
        //draw match Rcircle and Count crossPoint;
        smithchart.resistorCircle(1,Qt::cyan);
        crossP2 = smithchart.getRcoefandRCrossP(rCoef,1);
        for(int i=0;i<crossP2.nPoints;++i)
        {
            smithchart.drawCrossPoint(crossP2.crossP[i]);
        }
        // from CrossP to crossP2.crossP[0]
        startAng = atan2(crossP.y(),crossP.x());
        startAng = startAng/M_PI*180;
        if(startAng<0)
            startAng = startAng + 360;
        stopAng = atan2(crossP2.crossP[1].y(),crossP2.crossP[1].x());
        stopAng = stopAng/M_PI*180;
        if(stopAng<0)
            stopAng = stopAng +360;
        spanAng = stopAng-startAng;
        if(spanAng<0)
            spanAng = -spanAng;
        else
            spanAng = 360-spanAng;
        // -spanAngle is toward Power
        resPoint = smithchart.calAndDrawArc(rCoef,startAng,-spanAng,Qt::green);
        //cal and draw the position of stub
        PForLamda1 = smithchart.calAndDrawLineFromOrign(crossP,Qt::darkBlue);
        smithchart.drawCrossPoint(PForLamda1);
        phiLamda1 = smithchart.calLamdaFromCrossp(PForLamda1);
        smithchart.setText(PForLamda1,QString("%1λ").arg(phiLamda1));
        PForLamda2 = smithchart.calAndDrawLineFromOrign(crossP2.crossP[1],Qt::darkBlue);
        smithchart.drawCrossPoint(PForLamda2);
        phiLamda2 = smithchart.calLamdaFromCrossp(PForLamda2);
        smithchart.setText(PForLamda2,QString("%1λ").arg(phiLamda2));
        //cal and draw x circle
        x1 = smithchart.calXfromCrossp(crossP2.crossP[1]);
        smithchart.xCircle(x1,Qt::blue);
        x1Point = smithchart.getRcoefandXCrossp(1,x1);
        smithchart.drawCrossPoint(x1Point);
        smithchart.setText(x1Point,QString("%1").arg(x1));
        x2 = -x1;
        smithchart.xCircle(x2,Qt::blue);
        x2Point1 = smithchart.getRcoefandXCrossp(1,x2);
        smithchart.drawCrossPoint(x2Point1);
        smithchart.setText(x2Point1,QString("%1").arg(x2));
        x2Point2 = -x2Point1;
        smithchart.drawLine(x2Point1,x2Point2,Qt::darkBlue);
        smithchart.drawCrossPoint(x2Point2);
        posLamda1 = smithchart.calLamdaFromCrossp(x2Point2);
        smithchart.setText(x2Point2,QString("%1λ").arg(posLamda1));
        if(ui->radio_shortStub_2->isChecked())
        {
            smithchart.drawShortPoint();
            startAng = 180;
            stopAng = atan2(x2Point2.y(),x2Point2.x());
            stopAng = stopAng/M_PI*180;
            qreal spanAng;
            if(stopAng<0)
                spanAng = 180-stopAng;
            else
                spanAng = stopAng-startAng;
            if(spanAng<0)
                spanAng = -spanAng;
            resPoint = smithchart.calAndDrawArc(1,startAng,-spanAng,Qt::green);
            if(phiLamda1>phiLamda2)
            {
                stubPos2 =(0.5- (phiLamda1-phiLamda2))*v/freq;
            }
            else
            {
                stubPos2 = (phiLamda2-phiLamda1)*v/freq;
            }
            stubLen2 = posLamda1*v/freq;
        }
        else
        {
            smithchart.drawOpenPoint();
            startAng = 0;
            stopAng = atan2(x2Point2.y(),x2Point2.x());
            stopAng = stopAng/M_PI*180;
            qreal spanAng;
            if(stopAng<0)
                spanAng = -stopAng;
            else
                spanAng = 360-stopAng;
            if(spanAng<0)
                spanAng = -spanAng;
            resPoint = smithchart.calAndDrawArc(1,startAng,-spanAng,Qt::green);
            if(phiLamda1>phiLamda2)
            {
                stubPos2 =(0.5- (phiLamda1-phiLamda2))*v/freq;
            }
            else
            {
                stubPos2= (phiLamda2-phiLamda1)*v/freq;
            }
            if(posLamda1>0.25)
                stubLen2 = (posLamda1-0.25)*v/freq;
            else
                stubLen2 = (posLamda1+0.25)*v/freq;
        }
        str = QString("阻抗圆图求解 Result 2: Pos l1=%1m\t stub l2=%2m").arg(stubPos2).arg(stubLen2);
        resList4.append(str);
        ui->label_SingleStubPsmithChart_3->setPixmap(*pImg);

        //No.2 sloution in Y circle
        //draw Smithchart background
        smithchart.setPainter(pPainter);
        smithchart.drawBackground();
        //draw Gcircle and bCircle， then draw Cross Point
        g = tl.calZl2Y().getReal()*z0;
        b = tl.calZl2Y().getImag()*z0;
        smithchart.gCircle(g,Qt::red);
        smithchart.bCircle(b,Qt::red);
        crossP = smithchart.getGandBCrossP(g,b);
        smithchart.drawCrossPoint(point);
        //draw rcoef circle
        smithchart.drawCrossPoint(crossP);
        rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
        smithchart.rCoefCircle(rCoef,Qt::blue);
        //draw match gcircle and Count crossPoint;
        smithchart.gCircle(1,Qt::cyan);
        crossP2 = smithchart.getRcoefandGCrossP(rCoef,1);
        for(int i=0;i<crossP2.nPoints;++i)
        {
            smithchart.drawCrossPoint(crossP2.crossP[i]);
        }
        // from CrossP to crossP2.crossP[0]
        startAng = atan2(crossP.y(),crossP.x());
        startAng = startAng/M_PI*180;
        if(startAng<0)
            startAng = startAng + 360;
        stopAng = atan2(crossP2.crossP[0].y(),crossP2.crossP[0].x());
        stopAng = stopAng/M_PI*180;
        if(stopAng<0)
            stopAng = stopAng +360;
        spanAng = stopAng-startAng;
        if(spanAng<0)
            spanAng = -spanAng;
        else
            spanAng = 360-spanAng;
        // -spanAngle is toward Power
        resPoint = smithchart.calAndDrawArc(rCoef,startAng,-spanAng,Qt::green);
        //cal and draw the position of stub
        PForLamda1 = smithchart.calAndDrawLineFromOrign(crossP,Qt::darkBlue);
        smithchart.drawCrossPoint(PForLamda1);
        phiLamda1 = smithchart.calLamdaFromCrossp(PForLamda1);
        smithchart.setText(PForLamda1,QString("%1λ").arg(phiLamda1));
        PForLamda2 = smithchart.calAndDrawLineFromOrign(crossP2.crossP[0],Qt::darkBlue);
        smithchart.drawCrossPoint(PForLamda2);
        phiLamda2 = smithchart.calLamdaFromCrossp(PForLamda2);
        smithchart.setText(PForLamda2,QString("%1λ").arg(phiLamda2));
        //cal and draw bcircle
        b1 = smithchart.calBfromCrossp(crossP2.crossP[0]);
        smithchart.bCircle(b1,Qt::blue);
        b1Point = smithchart.getRcoefandXCrossp(1,b1);
        smithchart.drawCrossPoint(b1Point);
        smithchart.setText(b1Point,QString("%1").arg(b1));
        b2 = -b1;
        smithchart.bCircle(b2,Qt::blue);
        b2Point = smithchart.getRcoefandBCrossp(1,b2);
        smithchart.drawCrossPoint(b2Point);
        //smithchart.setText(b2Point,QString("%1").arg(b2));
        smithchart.calAndDrawLineFromOrign(b2Point,Qt::darkBlue);
        posLamda1 = smithchart.calLamdaFromCrossp(b2Point);
        smithchart.setText(b2Point,QString("%1λ").arg(posLamda1));
        if(ui->radio_shortStub_2->isChecked())
        {
            smithchart.drawShortPoint();
            startAng = 180;
            stopAng = atan2(b2Point.y(),b2Point.x());
            stopAng = stopAng/M_PI*180;
            qreal spanAng;
            if(stopAng<0)
                spanAng = 180-stopAng;
            else
                spanAng = stopAng-startAng;
            if(spanAng<0)
                spanAng = -spanAng;
            resPoint = smithchart.calAndDrawArc(1,startAng,-spanAng,Qt::green);
            if(phiLamda1>phiLamda2)
            {
                stubPos2 =(0.5- (phiLamda1-phiLamda2))*v/freq;
            }
            else
            {
                stubPos2 = (phiLamda2-phiLamda1)*v/freq;
            }
            stubLen2 = posLamda1*v/freq;
        }
        else
        {
            smithchart.drawOpenPoint();
            startAng = 0;
            stopAng = atan2(b2Point.y(),b2Point.x());
            stopAng = stopAng/M_PI*180;
            qreal spanAng;
            if(stopAng<0)
                spanAng = -stopAng;
            else
                spanAng = 360-stopAng;
            if(spanAng<0)
                spanAng = -spanAng;
            resPoint = smithchart.calAndDrawArc(1,startAng,-spanAng,Qt::green);
            if(phiLamda1>phiLamda2)
            {
                stubPos2 =(0.5- (phiLamda1-phiLamda2))*v/freq;
            }
            else
            {
                stubPos2= (phiLamda2-phiLamda1)*v/freq;
            }
            if(posLamda1>0.25)
                stubLen2 = (posLamda1-0.25)*v/freq;
            else
                stubLen2 = (posLamda1+0.25)*v/freq;
        }
        str = QString("导纳圆图求解 Result 1: Pos l1=%1m\t stub l2=%2m").arg(stubPos2).arg(stubLen2);
        resList4.append(str);
        ui->label_SingleStubPsmithChart_4->setPixmap(*pImg);
    }
    else
    {
        str = QString("已匹配，毋需加入匹配枝节");
        resList4.append(str);
    }
    model4.setStringList(resList4);
}


void MainWindow::on_label_BasicSmithChart_doubleClicked()
{
    if(ui->chbZin->isChecked()||ui->chbVswr->isChecked()||ui->chbRCoef->isChecked())
    {
        if(ui->leZ0->text().isEmpty())
        {
            return;
        }
        if(ui->leFreq->text().isEmpty())
        {
            return;
        }
        if(ui->leDist->text().isEmpty())
        {
            return;
        }
        if(ui->leZl_real->text().isEmpty())
        {
            return;
        }
        if(ui->leZl_real->text().isEmpty())
        {
            return;
        }
        double z0=ui->leZ0->text().toDouble();
        Complex zl(ui->leZl_real->text().toDouble(),ui->leZl_img->text().toDouble());
        double freq = ui->leFreq->text().toDouble()*1000000;//转换成Hz
        double d = ui->leDist->text().toDouble();
        double v = 300000000;//相速
        TransmissionLine tl(z0,zl,v/freq);
        QString str;
        Complex zin;
        QPointF crossP;
        QPointF point;
        QPointF point2;
        QPointF resPoint;
        struct CrossPoints crossP2;
        qreal resistor;
        qreal x;
        qreal rCoef ;
        qreal swr;
        qreal startAng;
        qreal spanAng;
        qreal phiLamda1,phiLamda2;
        SmithChart smithchart(imgHeight-80,imgWidth-80);
        switch(step4Base)
        {
        case 0:
            str = QString("SmithChart做图求解全过程如下:");
            resList1.append(str);
            //draw Smithchart background

            smithchart.setPainter(pPainter);
            smithchart.drawBackground();
            //draw Rcircle and XCircle， then draw Cross Point
            resistor = zl.getReal()/z0;
            x = zl.getImag()/z0;
            smithchart.resistorCircle(resistor,Qt::red);
            point = smithchart.getRandXAxisCrossp(resistor);
            smithchart.drawCrossPoint(point);
            smithchart.setText(point,QString("%1").arg(resistor));
            smithchart.xCircle(x,Qt::red);
            point = smithchart.getRcoefandXCrossp(1,x);
            smithchart.drawCrossPoint(point);
            smithchart.setText(point,QString("%1").arg(x));
            str = QString("Step 1: 计算负载的归一化电阻和电抗值，然后在SmithChart上绘制出对应的电阻圆R=%1和电抗圆X=%2；").arg(resistor).arg(x);
            resList1.append(str);
            ++step4Base;
            break;
        case 1:
            //Step 0
            smithchart.setPainter(pPainter);
            smithchart.drawBackground();
            //draw Rcircle and XCircle， then draw Cross Point
            resistor = zl.getReal()/z0;
            x = zl.getImag()/z0;
            smithchart.resistorCircle(resistor,Qt::red);
            point = smithchart.getRandXAxisCrossp(resistor);
            smithchart.drawCrossPoint(point);
            smithchart.setText(point,QString("%1").arg(resistor));
            smithchart.xCircle(x,Qt::red);
            point = smithchart.getRcoefandXCrossp(1,x);
            smithchart.drawCrossPoint(point);
            smithchart.setText(point,QString("%1").arg(x));
            //step 1
            crossP = smithchart.getRandXCrossP(resistor,x);
            //draw rcoef circle
            smithchart.drawCrossPoint(crossP);
            smithchart.setText(crossP,QString("(%1,%2)").arg(crossP.x()).arg(crossP.y()));
            rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
            smithchart.rCoefCircle(rCoef,Qt::blue);
            smithchart.setText(QPointF(0,rCoef),QString("%1").arg(rCoef));
            str = QString("Step 2: 找到R=%1的电阻圆和X=%2电抗圆的交点，过交点做反射系数圆，并读取反射系数为%3；").arg(resistor).arg(x).arg(rCoef);
            resList1.append(str);
            ++step4Base;
            break;
       case 2:
            //Step 0
            smithchart.setPainter(pPainter);
            smithchart.drawBackground();
            //draw Rcircle and XCircle， then draw Cross Point
            resistor = zl.getReal()/z0;
            x = zl.getImag()/z0;
            smithchart.resistorCircle(resistor,Qt::red);
            point = smithchart.getRandXAxisCrossp(resistor);
            smithchart.drawCrossPoint(point);
            smithchart.setText(point,QString("%1").arg(resistor));
            smithchart.xCircle(x,Qt::red);
            point = smithchart.getRcoefandXCrossp(1,x);
            smithchart.drawCrossPoint(point);
            smithchart.setText(point,QString("%1").arg(x));
            //step 1
            crossP = smithchart.getRandXCrossP(resistor,x);
            //draw rcoef circle
            smithchart.drawCrossPoint(crossP);
            smithchart.setText(crossP,QString("(%1,%2)").arg(crossP.x()).arg(crossP.y()));
            rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
            smithchart.rCoefCircle(rCoef,Qt::blue);
            smithchart.setText(QPointF(0,rCoef),QString("%1").arg(rCoef));
            //Step 2
            //draw SWR Rcircle
            crossP2 = smithchart.getRcoefandXAxisCrossp(rCoef);
            point2 = crossP2.crossP[0].x()>crossP2.crossP[1].x()?crossP2.crossP[0]:crossP2.crossP[1];
            smithchart.drawCrossPoint(point2);
            swr = smithchart.CalAndDrawSwrCircle(rCoef,Qt::darkMagenta);
            smithchart.setText(point2,QString("%1").arg(swr));
            str = QString("Step 3: 找到反射系数为%1的反射系数圆和X轴正半轴的交点，过交点做电阻圆，读取该电阻圆对应\n的归一化电阻值即驻波比为%2；").arg(rCoef).arg(swr);
            resList1.append(str);
            ++step4Base;
            break;
        case 3:
            //Step 0
            smithchart.setPainter(pPainter);
            smithchart.drawBackground();
            //draw Rcircle and XCircle， then draw Cross Point
            resistor = zl.getReal()/z0;
            x = zl.getImag()/z0;
            smithchart.resistorCircle(resistor,Qt::red);
            point = smithchart.getRandXAxisCrossp(resistor);
            smithchart.drawCrossPoint(point);
            smithchart.setText(point,QString("%1").arg(resistor));
            smithchart.xCircle(x,Qt::red);
            point = smithchart.getRcoefandXCrossp(1,x);
            smithchart.drawCrossPoint(point);
            smithchart.setText(point,QString("%1").arg(x));
            //step 1
            crossP = smithchart.getRandXCrossP(resistor,x);
            //draw rcoef circle
            smithchart.drawCrossPoint(crossP);
            smithchart.setText(crossP,QString("(%1,%2)").arg(crossP.x()).arg(crossP.y()));
            rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
            smithchart.rCoefCircle(rCoef,Qt::blue);
            smithchart.setText(QPointF(0,rCoef),QString("%1").arg(rCoef));
            //Step 2
            //draw SWR Rcircle
            crossP2 = smithchart.getRcoefandXAxisCrossp(rCoef);
            point2 = crossP2.crossP[0].x()>crossP2.crossP[1].x()?crossP2.crossP[0]:crossP2.crossP[1];
            smithchart.drawCrossPoint(point2);
            swr = smithchart.CalAndDrawSwrCircle(rCoef,Qt::darkMagenta);
            smithchart.setText(point2,QString("%1").arg(swr));
            //Step 3
            startAng = atan2(crossP.y(),crossP.x());
            startAng = startAng/M_PI*180;
            if(startAng<0)
                startAng = startAng + 360;
            point2 = smithchart.calAndDrawLineFromOrign(crossP,Qt::red);
            smithchart.drawCrossPoint(point2);
            phiLamda1 = smithchart.calLamdaFromCrossp(point2);
            smithchart.setText(point2,QString("%1λ").arg(phiLamda1));
            spanAng = 2*tl.getBeta()*d;
            spanAng=spanAng/M_PI*180;
            while(spanAng>=360.0)
                spanAng = spanAng-360;
            // -spanAngle is toward Power
            resPoint = smithchart.calAndDrawArc(rCoef,startAng,-spanAng,Qt::green);
            point2 = smithchart.calAndDrawLineFromOrign(resPoint,Qt::red);
            smithchart.drawCrossPoint(point2);
            phiLamda2 = smithchart.calLamdaFromCrossp(point2);
            smithchart.setText(point2,QString("%1λ").arg(phiLamda2));
            str = QString("Step 4: 将距终端负载距离转换为波长表示量%1λ，从终端负载对应的电阻圆和电抗圆的交点沿过交点的\n反射系数圆逆时针方向（向电源方向)旋转%1λ到所求输入阻抗的点（%2，%3）").arg(2*tl.getBeta()*d).arg(resPoint.x()).arg(resPoint.y());
            resList1.append(str);
            ++step4Base;
            break;
        case 4:
            //Step 0
            smithchart.setPainter(pPainter);
            smithchart.drawBackground();
            //draw Rcircle and XCircle， then draw Cross Point
            resistor = zl.getReal()/z0;
            x = zl.getImag()/z0;
            smithchart.resistorCircle(resistor,Qt::red);
            point = smithchart.getRandXAxisCrossp(resistor);
            smithchart.drawCrossPoint(point);
            smithchart.setText(point,QString("%1").arg(resistor));
            smithchart.xCircle(x,Qt::red);
            point = smithchart.getRcoefandXCrossp(1,x);
            smithchart.drawCrossPoint(point);
            smithchart.setText(point,QString("%1").arg(x));
            //step 1
            crossP = smithchart.getRandXCrossP(resistor,x);
            //draw rcoef circle
            smithchart.drawCrossPoint(crossP);
            smithchart.setText(crossP,QString("(%1,%2)").arg(crossP.x()).arg(crossP.y()));
            rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
            smithchart.rCoefCircle(rCoef,Qt::blue);
            smithchart.setText(QPointF(0,rCoef),QString("%1").arg(rCoef));
            //Step 2
            //draw SWR Rcircle
            crossP2 = smithchart.getRcoefandXAxisCrossp(rCoef);
            point2 = crossP2.crossP[0].x()>crossP2.crossP[1].x()?crossP2.crossP[0]:crossP2.crossP[1];
            smithchart.drawCrossPoint(point2);
            swr = smithchart.CalAndDrawSwrCircle(rCoef,Qt::darkMagenta);
            smithchart.setText(point2,QString("%1").arg(swr));
            //Step 3
            startAng = atan2(crossP.y(),crossP.x());
            startAng = startAng/M_PI*180;
            if(startAng<0)
                startAng = startAng + 360;
            point2 = smithchart.calAndDrawLineFromOrign(crossP,Qt::red);
            smithchart.drawCrossPoint(point2);
            phiLamda1 = smithchart.calLamdaFromCrossp(point2);
            smithchart.setText(point2,QString("%1λ").arg(phiLamda1));
            spanAng = 2*tl.getBeta()*d;
            spanAng=spanAng/M_PI*180;
            while(spanAng>=360.0)
                spanAng = spanAng-360;
            // -spanAngle is toward Power
            resPoint = smithchart.calAndDrawArc(rCoef,startAng,-spanAng,Qt::green);
            point2 = smithchart.calAndDrawLineFromOrign(resPoint,Qt::red);
            smithchart.drawCrossPoint(point2);
            phiLamda2 = smithchart.calLamdaFromCrossp(point2);
            smithchart.setText(point2,QString("%1λ").arg(phiLamda2));
            //step 5
            // cal the R of CrossP2
            resistor = smithchart.calRfromCrossp(resPoint);
            smithchart.resistorCircle(resistor,Qt::cyan);
            point = smithchart.getRandXAxisCrossp(resistor);
            smithchart.drawCrossPoint(point);
            smithchart.setText(point,QString("%1").arg(resistor));
            //cal the X of CrossP2
            x = smithchart.calXfromCrossp(resPoint);
            smithchart.xCircle(x,Qt::cyan);
            point = smithchart.getRcoefandXCrossp(1,x);
            smithchart.drawCrossPoint(point);
            smithchart.setText(point,QString("%1").arg(x));
            str = QString("Step 5: 过距终端负载距离转换为波长表示量%1λ处反射系数圆上的对应点做电阻圆和电抗圆，可以读取归一化输入阻抗为%2+j%3;").arg(2*tl.getBeta()*d).arg(resistor).arg(x);
            resList1.append(str);
            ++step4Base;
            break;
        case 5:
            //Step 0
            smithchart.setPainter(pPainter);
            smithchart.drawBackground();
            //draw Rcircle and XCircle， then draw Cross Point
            resistor = zl.getReal()/z0;
            x = zl.getImag()/z0;
            smithchart.resistorCircle(resistor,Qt::red);
            point = smithchart.getRandXAxisCrossp(resistor);
            smithchart.drawCrossPoint(point);
            smithchart.setText(point,QString("%1").arg(resistor));
            smithchart.xCircle(x,Qt::red);
            point = smithchart.getRcoefandXCrossp(1,x);
            smithchart.drawCrossPoint(point);
            smithchart.setText(point,QString("%1").arg(x));
            //step 1
            crossP = smithchart.getRandXCrossP(resistor,x);
            //draw rcoef circle
            smithchart.drawCrossPoint(crossP);
            smithchart.setText(crossP,QString("(%1,%2)").arg(crossP.x()).arg(crossP.y()));
            rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
            smithchart.rCoefCircle(rCoef,Qt::blue);
            smithchart.setText(QPointF(0,rCoef),QString("%1").arg(rCoef));
            //Step 2
            //draw SWR Rcircle
            crossP2 = smithchart.getRcoefandXAxisCrossp(rCoef);
            point2 = crossP2.crossP[0].x()>crossP2.crossP[1].x()?crossP2.crossP[0]:crossP2.crossP[1];
            smithchart.drawCrossPoint(point2);
            swr = smithchart.CalAndDrawSwrCircle(rCoef,Qt::darkMagenta);
            smithchart.setText(point2,QString("%1").arg(swr));
            //Step 3
            startAng = atan2(crossP.y(),crossP.x());
            startAng = startAng/M_PI*180;
            if(startAng<0)
                startAng = startAng + 360;
            point2 = smithchart.calAndDrawLineFromOrign(crossP,Qt::red);
            smithchart.drawCrossPoint(point2);
            phiLamda1 = smithchart.calLamdaFromCrossp(point2);
            smithchart.setText(point2,QString("%1λ").arg(phiLamda1));
            spanAng = 2*tl.getBeta()*d;
            spanAng=spanAng/M_PI*180;
            while(spanAng>=360.0)
                spanAng = spanAng-360;
            // -spanAngle is toward Power
            resPoint = smithchart.calAndDrawArc(rCoef,startAng,-spanAng,Qt::green);
            point2 = smithchart.calAndDrawLineFromOrign(resPoint,Qt::red);
            smithchart.drawCrossPoint(point2);
            phiLamda2 = smithchart.calLamdaFromCrossp(point2);
            smithchart.setText(point2,QString("%1λ").arg(phiLamda2));
            //step 5
            // cal the R of CrossP2
            resistor = smithchart.calRfromCrossp(resPoint);
            smithchart.resistorCircle(resistor,Qt::cyan);
            point = smithchart.getRandXAxisCrossp(resistor);
            smithchart.drawCrossPoint(point);
            smithchart.setText(point,QString("%1").arg(resistor));
            //cal the X of CrossP2
            x = smithchart.calXfromCrossp(resPoint);
            smithchart.xCircle(x,Qt::cyan);
            point = smithchart.getRcoefandXCrossp(1,x);
            smithchart.drawCrossPoint(point);
            smithchart.setText(point,QString("%1").arg(x));
            //step 6
            str = QString("Step 6: 乘以特性阻抗%1，则该位置输入阻抗:%2+j%3").arg(z0).arg(resistor*z0).arg(x*z0);
            resList1.append(str);
            step4Base = 0;
        }
        ui->label_BasicSmithChart->setPixmap(*pImg);
        //显示输出结果
        model1.setStringList(resList1);
    }
}

void MainWindow::on_label_quanterLamdaSmithChart_doubleClicked()
{
    if(ui->le_z0->text().isEmpty())
    {
        return;
    }
    if(ui->le_Freq->text().isEmpty())
    {
        return;
    }
    if(ui->le_zlR->text().isEmpty())
    {
        return;
    }
    if(ui->le_zlim->text().isEmpty())
    {
        return;
    }
    double z0=ui->le_z0->text().toDouble();
    Complex zl(ui->le_zlR->text().toDouble(),ui->le_zlim->text().toDouble());
    double freq = ui->le_Freq->text().toDouble()*1000000;//转换成Hz
    double v = 300000000.0;//相速
    //TransmissionLine tl(z0,zl,v/freq);
    lamdaDiv4 tl(z0,zl,v/freq);
    bool ok;
    ok = tl.calMatchParameter();

    //SmithChart
    //draw Smithchart background
    Complex zin;
    QPointF crossP;
    QPointF point;
    QPointF point2;
    QPointF resPoint;
    QPointF PForLamda;
    struct CrossPoints crossP2;
    qreal resistor;
    qreal x;
    qreal rCoef ;
    qreal swr1,swr2;
    qreal startAng;
    qreal spanAng;
    qreal phiLamda1,phiLamda2;
    qreal startPhi,stopPhi1,stopPhi2;
    qreal spanPhi;
    qreal phiLamda;
    bool isHalfLamda;
    qreal z01;
    qreal Rin;
    QString str;
    SmithChart smithchart(imgHeight-80,imgWidth-80);
    switch(step4QuarnterLamda)
    {
    case 0:
        if(ok)
            str = str = QString("SmithChart做图求解全过程如下:");
        else
        {
            str = QString("已匹配，毋需加入匹配枝节");
            resList2.append(str);
            return;
        }
        resList2.append(str);
        //draw Smithchart background
        smithchart.setPainter(pPainter);
        smithchart.drawBackground();
        //draw Rcircle and XCircle， then draw Cross Point
        resistor = zl.getReal()/z0;
        x = zl.getImag()/z0;
        smithchart.resistorCircle(resistor,Qt::red);
        point = smithchart.getRandXAxisCrossp(resistor);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(resistor));
        smithchart.xCircle(x,Qt::red);
        point = smithchart.getRcoefandXCrossp(1,x);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(x));
        str = QString("Step 1: 计算负载的归一化电阻和电抗值，然后在SmithChart上绘制出对应的电阻圆R=%1和电抗圆X=%2；").arg(resistor).arg(x);
        resList2.append(str);
        ++step4QuarnterLamda;
        break;
    case 1:
        //Step 0
        smithchart.setPainter(pPainter);
        smithchart.drawBackground();
        //draw Rcircle and XCircle， then draw Cross Point
        resistor = zl.getReal()/z0;
        x = zl.getImag()/z0;
        smithchart.resistorCircle(resistor,Qt::red);
        point = smithchart.getRandXAxisCrossp(resistor);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(resistor));
        smithchart.xCircle(x,Qt::red);
        point = smithchart.getRcoefandXCrossp(1,x);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(x));
        //step 1
        crossP = smithchart.getRandXCrossP(resistor,x);
        //draw rcoef circle
        smithchart.drawCrossPoint(crossP);
        smithchart.setText(crossP,QString("(%1,%2)").arg(crossP.x()).arg(crossP.y()));
        rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
        smithchart.rCoefCircle(rCoef,Qt::blue);
        smithchart.setText(QPointF(0,rCoef),QString("%1").arg(rCoef));
        str = QString("Step 2: 找到R=%1的电阻圆和X=%2电抗圆的交点，过交点做反射系数圆，并读取反射系数为%3；").arg(resistor).arg(x).arg(rCoef);
        resList2.append(str);
        ++step4QuarnterLamda;
        break;
   case 2:
        //Step 0
        smithchart.setPainter(pPainter);
        smithchart.drawBackground();
        //draw Rcircle and XCircle， then draw Cross Point
        resistor = zl.getReal()/z0;
        x = zl.getImag()/z0;
        smithchart.resistorCircle(resistor,Qt::red);
        point = smithchart.getRandXAxisCrossp(resistor);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(resistor));
        smithchart.xCircle(x,Qt::red);
        point = smithchart.getRcoefandXCrossp(1,x);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(x));
        //step 1
        crossP = smithchart.getRandXCrossP(resistor,x);
        //draw rcoef circle
        smithchart.drawCrossPoint(crossP);
        smithchart.setText(crossP,QString("(%1,%2)").arg(crossP.x()).arg(crossP.y()));
        rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
        smithchart.rCoefCircle(rCoef,Qt::blue);
        smithchart.setText(QPointF(0,rCoef),QString("%1").arg(rCoef));
        //Step 2
        crossP2 = smithchart.getRcoefandXAxisCrossp(rCoef);
        smithchart.drawCrossPoint(crossP2.crossP[0]);
        smithchart.drawCrossPoint(crossP2.crossP[1]);
        str = QString("Step 3: 找到反射系数为%1的反射系数圆和X轴的两个交点");
        resList2.append(str);
        ++step4QuarnterLamda;
        break;
    case 3:
        //Step 0
        smithchart.setPainter(pPainter);
        smithchart.drawBackground();
        //draw Rcircle and XCircle， then draw Cross Point
        resistor = zl.getReal()/z0;
        x = zl.getImag()/z0;
        smithchart.resistorCircle(resistor,Qt::red);
        point = smithchart.getRandXAxisCrossp(resistor);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(resistor));
        smithchart.xCircle(x,Qt::red);
        point = smithchart.getRcoefandXCrossp(1,x);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(x));
        //step 1
        crossP = smithchart.getRandXCrossP(resistor,x);
        //draw rcoef circle
        smithchart.drawCrossPoint(crossP);
        smithchart.setText(crossP,QString("(%1,%2)").arg(crossP.x()).arg(crossP.y()));
        rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
        smithchart.rCoefCircle(rCoef,Qt::blue);
        smithchart.setText(QPointF(0,rCoef),QString("%1").arg(rCoef));
        //Step 2
        crossP2 = smithchart.getRcoefandXAxisCrossp(rCoef);
        smithchart.drawCrossPoint(crossP2.crossP[0]);
        smithchart.drawCrossPoint(crossP2.crossP[1]);
        //Step 3
        stopPhi1 = 0;
        stopPhi2 = M_PI;
        //swr1 = smithchart.CalAndDrawSwrCircle(rCoef,Qt::darkMagenta);
        //swr2 = smithchart.CalAndDrawSwrCircle(-rCoef,Qt::darkMagenta);
        startPhi =smithchart.getRcoefPhiFromCrossp(crossP);
        if(startPhi<=M_PI)
        {
            spanPhi = stopPhi1-startPhi;
            isHalfLamda = false;
        }
        else
        {
            spanPhi = stopPhi2-startPhi;
            isHalfLamda= true;
        }
        smithchart.calAndDrawArc(rCoef,startPhi/M_PI*180,spanPhi/M_PI*180,Qt::black);
        str = QString("Step 4: 从终端负载位置沿反射系数圆图向电源方向逆时针旋转到x轴（纯电阻轴）");
        resList2.append(str);
        ++step4QuarnterLamda;
        break;
    case 4:
        //Step 0
        smithchart.setPainter(pPainter);
        smithchart.drawBackground();
        //draw Rcircle and XCircle， then draw Cross Point
        resistor = zl.getReal()/z0;
        x = zl.getImag()/z0;
        smithchart.resistorCircle(resistor,Qt::red);
        point = smithchart.getRandXAxisCrossp(resistor);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(resistor));
        smithchart.xCircle(x,Qt::red);
        point = smithchart.getRcoefandXCrossp(1,x);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(x));
        //step 1
        crossP = smithchart.getRandXCrossP(resistor,x);
        //draw rcoef circle
        smithchart.drawCrossPoint(crossP);
        smithchart.setText(crossP,QString("(%1,%2)").arg(crossP.x()).arg(crossP.y()));
        rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
        smithchart.rCoefCircle(rCoef,Qt::blue);
        smithchart.setText(QPointF(0,rCoef),QString("%1").arg(rCoef));
        //Step 2
        crossP2 = smithchart.getRcoefandXAxisCrossp(rCoef);
        smithchart.drawCrossPoint(crossP2.crossP[0]);
        smithchart.drawCrossPoint(crossP2.crossP[1]);
        //Step 3
        stopPhi1 = 0;
        stopPhi2 = M_PI;
        startPhi =smithchart.getRcoefPhiFromCrossp(crossP);
        if(startPhi<=M_PI)
        {
            swr1 = smithchart.CalAndDrawSwrCircle(rCoef,Qt::darkMagenta);
            point = crossP2.crossP[0].x()>crossP2.crossP[1].x()?crossP2.crossP[0]:crossP2.crossP[1];
            smithchart.setText(point,QString("%1").arg(swr1));
            spanPhi = stopPhi1-startPhi;
            isHalfLamda = false;
        }
        else
        {
            swr2 = smithchart.CalAndDrawSwrCircle(-rCoef,Qt::darkMagenta);
            point = crossP2.crossP[0].x()<crossP2.crossP[1].x()?crossP2.crossP[0]:crossP2.crossP[1];
            smithchart.setText(point,QString("%1").arg(swr2));
            spanPhi = stopPhi2-startPhi;
            isHalfLamda= true;
        }
        smithchart.calAndDrawArc(rCoef,startPhi/M_PI*180,spanPhi/M_PI*180,Qt::black);
        smithchart.calAndDrawArc(1,startPhi/M_PI*180,spanPhi/M_PI*180,Qt::black);
        PForLamda = smithchart.calAndDrawLineFromOrign(crossP,Qt::darkBlue);
        smithchart.drawCrossPoint(PForLamda);
        phiLamda = smithchart.calLamdaFromCrossp(PForLamda);
        smithchart.setText(PForLamda,QString("%1λ").arg(phiLamda));
        if(isHalfLamda)
        {
            resList2.append(QString("Step 5: 读取从终端负载旋转到0.5λ：0.5λ-%1λ=%2λ，所以1/4λ阻抗变换器加在距离终端负载ZL %3m处").arg(phiLamda).arg(0.5-phiLamda).arg((0.5-phiLamda)*tl.getLamda()));
        }
        else
        {
            resList2.append(QString("Step 5: 读取从终端负载旋转到0.25λ：0.25λ-%1λ=%1λ，所以1/4λ阻抗变换器加在距离终端负载ZL %3m处").arg(phiLamda).arg(0.25-phiLamda).arg((0.25-phiLamda)*tl.getLamda()));
        }
        ++step4QuarnterLamda;
        break;
    case 5:
        //Step 0
        smithchart.setPainter(pPainter);
        smithchart.drawBackground();
        //draw Rcircle and XCircle， then draw Cross Point
        resistor = zl.getReal()/z0;
        x = zl.getImag()/z0;
        smithchart.resistorCircle(resistor,Qt::red);
        point = smithchart.getRandXAxisCrossp(resistor);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(resistor));
        smithchart.xCircle(x,Qt::red);
        point = smithchart.getRcoefandXCrossp(1,x);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(x));
        //step 1
        crossP = smithchart.getRandXCrossP(resistor,x);
        //draw rcoef circle
        smithchart.drawCrossPoint(crossP);
        smithchart.setText(crossP,QString("(%1,%2)").arg(crossP.x()).arg(crossP.y()));
        rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
        smithchart.rCoefCircle(rCoef,Qt::blue);
        smithchart.setText(QPointF(0,rCoef),QString("%1").arg(rCoef));
        //Step 2
        crossP2 = smithchart.getRcoefandXAxisCrossp(rCoef);
        smithchart.drawCrossPoint(crossP2.crossP[0]);
        smithchart.drawCrossPoint(crossP2.crossP[1]);
        //Step 3
        stopPhi1 = 0;
        stopPhi2 = M_PI;
        startPhi =smithchart.getRcoefPhiFromCrossp(crossP);
        if(startPhi<=M_PI)
        {
            swr1 = smithchart.CalAndDrawSwrCircle(rCoef,Qt::darkMagenta);
            point = crossP2.crossP[0].x()>crossP2.crossP[1].x()?crossP2.crossP[0]:crossP2.crossP[1];
            smithchart.setText(point,QString("%1").arg(swr1));
            Rin = swr1;
            spanPhi = stopPhi1-startPhi;
            isHalfLamda = false;
        }
        else
        {
            swr2 = smithchart.CalAndDrawSwrCircle(-rCoef,Qt::darkMagenta);
            point = crossP2.crossP[0].x()<crossP2.crossP[1].x()?crossP2.crossP[0]:crossP2.crossP[1];
            smithchart.setText(point,QString("%1").arg(swr2));
            Rin = swr2;
            spanPhi = stopPhi2-startPhi;
            isHalfLamda= true;
        }
        smithchart.calAndDrawArc(rCoef,startPhi/M_PI*180,spanPhi/M_PI*180,Qt::black);
        smithchart.calAndDrawArc(1,startPhi/M_PI*180,spanPhi/M_PI*180,Qt::black);
        PForLamda = smithchart.calAndDrawLineFromOrign(crossP,Qt::darkBlue);
        smithchart.drawCrossPoint(PForLamda);
        phiLamda = smithchart.calLamdaFromCrossp(PForLamda);
        smithchart.setText(PForLamda,QString("%1λ").arg(phiLamda));
        //step 5
        z01 = sqrt(Rin)*z0;
        str = QString("Step 6: 1/4λ阻抗变换器的特性阻抗为%1Ω").arg(z01);
        resList2.append(str);
        step4QuarnterLamda=0;
    }
    ui->label_quanterLamdaSmithChart->setPixmap(*pImg);
    //显示输出结果
    model2.setStringList(resList2);
}

void MainWindow::on_label_SingleStubSsmithChart_doubleClicked()
{
    if(ui->leZ0_Series->text().isEmpty())
    {
        return;
    }
    if(ui->leFreq_Series->text().isEmpty())
    {
        return;
    }
    if(ui->leZl_real_Series->text().isEmpty())
    {
        return;
    }
    if(ui->leZl_img_Series->text().isEmpty())
    {
        return;
    }

    double z0=ui->leZ0_Series->text().toDouble();
    Complex zl(ui->leZl_real_Series->text().toDouble(),ui->leZl_img_Series->text().toDouble());
    double freq = ui->leFreq_Series->text().toDouble()*1000000;//转换成Hz
    double v = 300000000.0;//相速
    QString str;

    //TransmissionLine tl(z0,zl,v/freq);
    SingleStubSeriesMatching tl(z0,zl,v/freq);
    if(ui->radio_shortStub->isChecked())
        tl.setStubType(shortStub);
    if(ui->radio_openStub->isChecked())
        tl.setStubType(openStub);

    struct StubMatchResult res[2];
    bool ok;
    ok = tl.calMatchParameter();
    //SmithChart
    //draw Smithchart background
    Complex zin;
    QPointF crossP;
    QPointF point;
    QPointF point2;
    QPointF resPoint;
    QPointF PForLamda;
    struct CrossPoints crossP2;
    qreal resistor;
    qreal x;
    qreal rCoef ;
    qreal swr1,swr2;
    qreal startAng;
    qreal spanAng;
    qreal phiLamda1,phiLamda2;
    qreal startPhi,stopPhi1,stopPhi2;
    qreal spanPhi;
    qreal phiLamda;
    bool isHalfLamda;
    qreal z01;
    qreal Rin;
    SmithChart smithchart(imgHeight-80,imgWidth-80);
    switch(step4SeriesStub[0])
    {
    case 0:
        if(ok)
        {
                        resList3.clear();
            str = QString("单枝节串联匹配：工作频率:%1MHz\t特性阻抗:%2Ω\t终端负载阻抗：%3+j%4(Ω)\t").arg(freq/1000000).arg(z0).arg(zl.getReal()).arg(zl.getImag());
            resList3.append(str);
            str = QString("/*****************************************************************************************/");
            resList3.append(str);
            str = QString("SmithChart做图进行单枝节串联匹配求解全过程如下:");
            resList3.append(str);
        }
        else //ok is false when the Tline has been matched
        {
            str = QString("已匹配，毋需加入匹配枝节");
            resList3.append(str);
            return;
        }
        //draw Smithchart background
        smithchart.setPainter(pPainter);
        smithchart.drawBackground();
        //draw Rcircle and XCircle， then draw Cross Point
        resistor = zl.getReal()/z0;
        x = zl.getImag()/z0;
        smithchart.resistorCircle(resistor,Qt::red);
        point = smithchart.getRandXAxisCrossp(resistor);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(resistor));
        smithchart.xCircle(x,Qt::red);
        point = smithchart.getRcoefandXCrossp(1,x);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(x));
        str = QString("Step 1: 计算负载的归一化电阻和电抗值，然后在SmithChart上绘制出对应的电阻圆R=%1和电抗圆X=%2；").arg(resistor).arg(x);
        resList3.append(str);
        ++step4SeriesStub[0];
        break;
    case 1:
        //Step 0
        smithchart.setPainter(pPainter);
        smithchart.drawBackground();
        //draw Rcircle and XCircle， then draw Cross Point
        resistor = zl.getReal()/z0;
        x = zl.getImag()/z0;
        smithchart.resistorCircle(resistor,Qt::red);
        point = smithchart.getRandXAxisCrossp(resistor);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(resistor));
        smithchart.xCircle(x,Qt::red);
        point = smithchart.getRcoefandXCrossp(1,x);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(x));
        //step 1
        crossP = smithchart.getRandXCrossP(resistor,x);
        //draw rcoef circle
        smithchart.drawCrossPoint(crossP);
        smithchart.setText(crossP,QString("(%1,%2)").arg(crossP.x()).arg(crossP.y()));
        rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
        smithchart.rCoefCircle(rCoef,Qt::blue);
        smithchart.setText(QPointF(0,rCoef),QString("%1").arg(rCoef));
        str = QString("Step 2: 找到R=%1的电阻圆和X=%2电抗圆的交点，过交点做反射系数圆，并读取反射系数为%3；").arg(resistor).arg(x).arg(rCoef);
        resList3.append(str);
        ++step4SeriesStub[0];
        break;
   case 2:
        //Step 0
        smithchart.setPainter(pPainter);
        smithchart.drawBackground();
        //draw Rcircle and XCircle， then draw Cross Point
        resistor = zl.getReal()/z0;
        x = zl.getImag()/z0;
        smithchart.resistorCircle(resistor,Qt::red);
        point = smithchart.getRandXAxisCrossp(resistor);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(resistor));
        smithchart.xCircle(x,Qt::red);
        point = smithchart.getRcoefandXCrossp(1,x);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(x));
        //step 1
        crossP = smithchart.getRandXCrossP(resistor,x);
        //draw rcoef circle
        smithchart.drawCrossPoint(crossP);
        smithchart.setText(crossP,QString("(%1,%2)").arg(crossP.x()).arg(crossP.y()));
        rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
        smithchart.rCoefCircle(rCoef,Qt::blue);
        smithchart.setText(QPointF(0,rCoef),QString("%1").arg(rCoef));
        //Step 2
        crossP2 = smithchart.getRcoefandXAxisCrossp(rCoef);
        smithchart.drawCrossPoint(crossP2.crossP[0]);
        smithchart.drawCrossPoint(crossP2.crossP[1]);
        str = QString("Step 3: 找到反射系数为%1的反射系数圆和X轴的两个交点");
        resList3.append(str);
        ++step4SeriesStub[0];
        break;
    case 3:
        //Step 0
        smithchart.setPainter(pPainter);
        smithchart.drawBackground();
        //draw Rcircle and XCircle， then draw Cross Point
        resistor = zl.getReal()/z0;
        x = zl.getImag()/z0;
        smithchart.resistorCircle(resistor,Qt::red);
        point = smithchart.getRandXAxisCrossp(resistor);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(resistor));
        smithchart.xCircle(x,Qt::red);
        point = smithchart.getRcoefandXCrossp(1,x);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(x));
        //step 1
        crossP = smithchart.getRandXCrossP(resistor,x);
        //draw rcoef circle
        smithchart.drawCrossPoint(crossP);
        smithchart.setText(crossP,QString("(%1,%2)").arg(crossP.x()).arg(crossP.y()));
        rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
        smithchart.rCoefCircle(rCoef,Qt::blue);
        smithchart.setText(QPointF(0,rCoef),QString("%1").arg(rCoef));
        //Step 2
        crossP2 = smithchart.getRcoefandXAxisCrossp(rCoef);
        smithchart.drawCrossPoint(crossP2.crossP[0]);
        smithchart.drawCrossPoint(crossP2.crossP[1]);
        //Step 3
        stopPhi1 = 0;
        stopPhi2 = M_PI;
        //swr1 = smithchart.CalAndDrawSwrCircle(rCoef,Qt::darkMagenta);
        //swr2 = smithchart.CalAndDrawSwrCircle(-rCoef,Qt::darkMagenta);
        startPhi =smithchart.getRcoefPhiFromCrossp(crossP);
        if(startPhi<=M_PI)
        {
            spanPhi = stopPhi1-startPhi;
            isHalfLamda = false;
        }
        else
        {
            spanPhi = stopPhi2-startPhi;
            isHalfLamda= true;
        }
        smithchart.calAndDrawArc(rCoef,startPhi/M_PI*180,spanPhi/M_PI*180,Qt::black);
        str = QString("Step 4: 从终端负载位置沿反射系数圆图向电源方向逆时针旋转到x轴（纯电阻轴）");
        resList3.append(str);
        ++step4SeriesStub[0];
        break;
    case 4:
        //Step 0
        smithchart.setPainter(pPainter);
        smithchart.drawBackground();
        //draw Rcircle and XCircle， then draw Cross Point
        resistor = zl.getReal()/z0;
        x = zl.getImag()/z0;
        smithchart.resistorCircle(resistor,Qt::red);
        point = smithchart.getRandXAxisCrossp(resistor);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(resistor));
        smithchart.xCircle(x,Qt::red);
        point = smithchart.getRcoefandXCrossp(1,x);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(x));
        //step 1
        crossP = smithchart.getRandXCrossP(resistor,x);
        //draw rcoef circle
        smithchart.drawCrossPoint(crossP);
        smithchart.setText(crossP,QString("(%1,%2)").arg(crossP.x()).arg(crossP.y()));
        rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
        smithchart.rCoefCircle(rCoef,Qt::blue);
        smithchart.setText(QPointF(0,rCoef),QString("%1").arg(rCoef));
        //Step 2
        crossP2 = smithchart.getRcoefandXAxisCrossp(rCoef);
        smithchart.drawCrossPoint(crossP2.crossP[0]);
        smithchart.drawCrossPoint(crossP2.crossP[1]);
        //Step 3
        stopPhi1 = 0;
        stopPhi2 = M_PI;
        startPhi =smithchart.getRcoefPhiFromCrossp(crossP);
        if(startPhi<=M_PI)
        {
            swr1 = smithchart.CalAndDrawSwrCircle(rCoef,Qt::darkMagenta);
            point = crossP2.crossP[0].x()>crossP2.crossP[1].x()?crossP2.crossP[0]:crossP2.crossP[1];
            smithchart.setText(point,QString("%1").arg(swr1));
            spanPhi = stopPhi1-startPhi;
            isHalfLamda = false;
        }
        else
        {
            swr2 = smithchart.CalAndDrawSwrCircle(-rCoef,Qt::darkMagenta);
            point = crossP2.crossP[0].x()<crossP2.crossP[1].x()?crossP2.crossP[0]:crossP2.crossP[1];
            smithchart.setText(point,QString("%1").arg(swr2));
            spanPhi = stopPhi2-startPhi;
            isHalfLamda= true;
        }
        smithchart.calAndDrawArc(rCoef,startPhi/M_PI*180,spanPhi/M_PI*180,Qt::black);
        smithchart.calAndDrawArc(1,startPhi/M_PI*180,spanPhi/M_PI*180,Qt::black);
        PForLamda = smithchart.calAndDrawLineFromOrign(crossP,Qt::darkBlue);
        smithchart.drawCrossPoint(PForLamda);
        phiLamda = smithchart.calLamdaFromCrossp(PForLamda);
        smithchart.setText(PForLamda,QString("%1λ").arg(phiLamda));
        if(isHalfLamda)
        {
            resList3.append(QString("Step 5: 读取从终端负载旋转到0.5λ：0.5λ-%1λ=%2λ，所以串联单枝节加在距离终端负载ZL %3m处").arg(phiLamda).arg(0.5-phiLamda).arg((0.5-phiLamda)*tl.getLamda()));
        }
        else
        {
            resList3.append(QString("Step 5: 读取从终端负载旋转到0.25λ：0.25λ-%1λ=%1λ，所以串联单枝节加在距离终端负载ZL %3m处").arg(phiLamda).arg(0.25-phiLamda).arg((0.25-phiLamda)*tl.getLamda()));
        }
        ++step4SeriesStub[0];
        break;
    case 5:
        //Step 0
        smithchart.setPainter(pPainter);
        smithchart.drawBackground();
        //draw Rcircle and XCircle， then draw Cross Point
        resistor = zl.getReal()/z0;
        x = zl.getImag()/z0;
        smithchart.resistorCircle(resistor,Qt::red);
        point = smithchart.getRandXAxisCrossp(resistor);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(resistor));
        smithchart.xCircle(x,Qt::red);
        point = smithchart.getRcoefandXCrossp(1,x);
        smithchart.drawCrossPoint(point);
        smithchart.setText(point,QString("%1").arg(x));
        //step 1
        crossP = smithchart.getRandXCrossP(resistor,x);
        //draw rcoef circle
        smithchart.drawCrossPoint(crossP);
        smithchart.setText(crossP,QString("(%1,%2)").arg(crossP.x()).arg(crossP.y()));
        rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
        smithchart.rCoefCircle(rCoef,Qt::blue);
        smithchart.setText(QPointF(0,rCoef),QString("%1").arg(rCoef));
        //Step 2
        crossP2 = smithchart.getRcoefandXAxisCrossp(rCoef);
        smithchart.drawCrossPoint(crossP2.crossP[0]);
        smithchart.drawCrossPoint(crossP2.crossP[1]);
        //Step 3
        stopPhi1 = 0;
        stopPhi2 = M_PI;
        startPhi =smithchart.getRcoefPhiFromCrossp(crossP);
        if(startPhi<=M_PI)
        {
            swr1 = smithchart.CalAndDrawSwrCircle(rCoef,Qt::darkMagenta);
            point = crossP2.crossP[0].x()>crossP2.crossP[1].x()?crossP2.crossP[0]:crossP2.crossP[1];
            smithchart.setText(point,QString("%1").arg(swr1));
            Rin = swr1;
            spanPhi = stopPhi1-startPhi;
            isHalfLamda = false;
        }
        else
        {
            swr2 = smithchart.CalAndDrawSwrCircle(-rCoef,Qt::darkMagenta);
            point = crossP2.crossP[0].x()<crossP2.crossP[1].x()?crossP2.crossP[0]:crossP2.crossP[1];
            smithchart.setText(point,QString("%1").arg(swr2));
            Rin = swr2;
            spanPhi = stopPhi2-startPhi;
            isHalfLamda= true;
        }
        smithchart.calAndDrawArc(rCoef,startPhi/M_PI*180,spanPhi/M_PI*180,Qt::black);
        smithchart.calAndDrawArc(1,startPhi/M_PI*180,spanPhi/M_PI*180,Qt::black);
        PForLamda = smithchart.calAndDrawLineFromOrign(crossP,Qt::darkBlue);
        smithchart.drawCrossPoint(PForLamda);
        phiLamda = smithchart.calLamdaFromCrossp(PForLamda);
        smithchart.setText(PForLamda,QString("%1λ").arg(phiLamda));
        //step 5

        resList3.append(str);
        ++step4SeriesStub[0];
    default:
        step4SeriesStub[0]=0;
    }
//    if(!ok)//ok is false when the Tline has been matched
//    {
//        str = QString("已匹配，毋需利用smith圆图进行串联单枝节匹配");
//        resList3.append(str);
//        return;
//    }
//    if(ok)
//    {

//        //draw Smithchart background
//        SmithChart smithchart(imgHeight-80,imgWidth-80);
//        smithchart.setPainter(pPainter);
//        smithchart.drawBackground();
//        //draw Rcircle and XCircle， then draw Cross Point
//        qreal resistor = zl.getReal()/z0;
//        qreal x = zl.getImag()/z0;
//        QPointF crossP = smithchart.getRandXCrossP(resistor,x);
//        QPointF point;
//        smithchart.resistorCircle(resistor,Qt::red);
//        point = smithchart.getRandXAxisCrossp(resistor);
//        smithchart.drawCrossPoint(point);
//        smithchart.xCircle(x,Qt::red);
//        point = smithchart.getRcoefandXCrossp(1,x);
//        smithchart.drawCrossPoint(point);
//        //draw rcoef circle
//        smithchart.drawCrossPoint(crossP);
//        qreal rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
//        smithchart.rCoefCircle(rCoef,Qt::blue);
//        //draw match Rcircle and Count crossPoint;
//        struct CrossPoints crossP2;
//        smithchart.resistorCircle(1,Qt::cyan);
//        crossP2 = smithchart.getRcoefandRCrossP(rCoef,1);
//        for(int i=0;i<crossP2.nPoints;++i)
//        {
//            smithchart.drawCrossPoint(crossP2.crossP[i]);
//        }
//        // from CrossP to crossP2.crossP[0]
//        qreal startAng = atan2(crossP.y(),crossP.x());
//        startAng = startAng/M_PI*180;
//        if(startAng<0)
//            startAng = startAng + 360;
//        qreal stopAng = atan2(crossP2.crossP[0].y(),crossP2.crossP[0].x());
//        stopAng = stopAng/M_PI*180;
//        if(stopAng<0)
//            stopAng = stopAng +360;
//        qreal spanAng = stopAng-startAng;
//        if(spanAng<0)
//            spanAng = -spanAng;
//        // -spanAngle is toward Power
//        QPointF resPoint;
//        resPoint = smithchart.calAndDrawArc(rCoef,startAng,-spanAng,Qt::green);
//        //cal and draw the position of stub
//        QPointF PForLamda1 = smithchart.calAndDrawLineFromOrign(crossP,Qt::darkBlue);
//        smithchart.drawCrossPoint(PForLamda1);
//        qreal phiLamda1 = smithchart.calLamdaFromCrossp(PForLamda1);
//        smithchart.setText(PForLamda1,QString("%1λ").arg(phiLamda1));
//        QPointF PForLamda2 = smithchart.calAndDrawLineFromOrign(crossP2.crossP[0],Qt::darkBlue);
//        smithchart.drawCrossPoint(PForLamda2);
//        qreal phiLamda2 = smithchart.calLamdaFromCrossp(PForLamda2);
//        smithchart.setText(PForLamda2,QString("%1λ").arg(phiLamda2));
//        //cal and draw x circle
//        qreal x1 = smithchart.calXfromCrossp(crossP2.crossP[0]);
//        smithchart.xCircle(x1,Qt::blue);
//        QPointF x1Point = smithchart.getRcoefandXCrossp(1,x1);
//        smithchart.drawCrossPoint(x1Point);
//        smithchart.setText(x1Point,QString("%1").arg(x1));
//        qreal x2 = -x1;
//        smithchart.xCircle(x2,Qt::blue);
//        QPointF x2Point = smithchart.getRcoefandXCrossp(1,x2);
//        smithchart.calAndDrawLineFromOrign(x2Point,Qt::darkBlue);
//        smithchart.drawCrossPoint(x2Point);
//        //smithchart.setText(x2Point,QString("%1").arg(x2));
//        qreal posLamda1 = smithchart.calLamdaFromCrossp(x2Point);
//        smithchart.setText(x2Point,QString("%1λ").arg(posLamda1));
//        qreal stubPos1,stubPos2;
//        qreal stubLen1=0,stubLen2=0;

//        if(ui->radio_shortStub->isChecked())
//        {
//            smithchart.drawShortPoint();
//            startAng = 180;
//            stopAng = atan2(x2Point.y(),x2Point.x());
//            stopAng = stopAng/M_PI*180;
//            qreal spanAng;
//            if(stopAng<0)
//                spanAng = 180-stopAng;
//            else
//                spanAng = stopAng-startAng;
//            if(spanAng<0)
//                spanAng = -spanAng;
//            resPoint = smithchart.calAndDrawArc(1,startAng,-spanAng,Qt::green);
//            if(phiLamda1>phiLamda2)
//            {
//                stubPos1 =(0.5- (phiLamda1-phiLamda2))*v/freq;
//            }
//            else
//            {
//                stubPos1 = (phiLamda2-phiLamda1)*v/freq;
//            }
//            stubLen1 = posLamda1*v/freq;
//        }
//        else
//        {
//            smithchart.drawOpenPoint();
//            startAng = 0;
//            stopAng = atan2(x2Point.y(),x2Point.x());
//            stopAng = stopAng/M_PI*180;
//            qreal spanAng;
//            if(stopAng<0)
//                spanAng = -stopAng;
//            else
//                spanAng = 360-stopAng;
//            if(spanAng<0)
//                spanAng = -spanAng;
//            resPoint = smithchart.calAndDrawArc(1,startAng,-spanAng,Qt::green);
//            if(phiLamda1>phiLamda2)
//            {
//                stubPos1 =(0.5- (phiLamda1-phiLamda2))*v/freq;
//            }
//            else
//            {
//                stubPos1 = (phiLamda2-phiLamda1)*v/freq;
//            }
//            if(posLamda1>0.25)
//                stubLen1 = (posLamda1-0.25)*v/freq;
//            else
//                stubLen1 = (posLamda1+0.25)*v/freq;
//        }
//        str = QString("Result 1: Pos l1=%1m\t stub l2=%2m").arg(stubPos1).arg(stubLen1);
//        resList3.append(str);

//        ui->label_SingleStubSsmithChart->setPixmap(*pImg);
//        /* ******************************
//         * the second resolution        *
//         * ******************************/
//        smithchart.drawBackground();
//        //draw Rcircle and XCircle， then draw Cross Point
//        resistor = zl.getReal()/z0;
//        x = zl.getImag()/z0;
//        crossP = smithchart.getRandXCrossP(resistor,x);
//        smithchart.resistorCircle(resistor,Qt::red);
//        point = smithchart.getRandXAxisCrossp(resistor);
//        smithchart.drawCrossPoint(point);
//        smithchart.xCircle(x,Qt::red);
//        point = smithchart.getRcoefandXCrossp(1,x);
//        smithchart.drawCrossPoint(point);
//        //draw rcoef circle
//        smithchart.drawCrossPoint(crossP);
//        rCoef = sqrt(crossP.x()*crossP.x()+crossP.y()*crossP.y());
//        smithchart.rCoefCircle(rCoef,Qt::blue);
//        //draw match Rcircle and Count crossPoint;
//        smithchart.resistorCircle(1,Qt::cyan);
//        crossP2 = smithchart.getRcoefandRCrossP(rCoef,1);
//        for(int i=0;i<crossP2.nPoints;++i)
//        {
//            smithchart.drawCrossPoint(crossP2.crossP[i]);
//        }
//        // from CrossP to crossP2.crossP[1]
//        startAng = atan2(crossP.y(),crossP.x());
//        startAng = startAng/M_PI*180;
//        if(startAng<0)
//            startAng = startAng + 360;
//        stopAng = atan2(crossP2.crossP[1].y(),crossP2.crossP[1].x());
//        stopAng = stopAng/M_PI*180;
//        if(stopAng<0)
//            stopAng = stopAng +360;
//        spanAng = stopAng-startAng;
//        if(spanAng<0)
//            spanAng = -spanAng;
//        else
//            spanAng = 360-spanAng;
//        // -spanAngle is toward Power
//        resPoint = smithchart.calAndDrawArc(rCoef,startAng,-spanAng,Qt::green);
//        //cal and draw the position of stub
//        PForLamda1 = smithchart.calAndDrawLineFromOrign(crossP,Qt::darkBlue);
//        smithchart.drawCrossPoint(PForLamda1);
//        phiLamda1 = smithchart.calLamdaFromCrossp(PForLamda1);
//        smithchart.setText(PForLamda1,QString("%1λ").arg(phiLamda1));
//        PForLamda2 = smithchart.calAndDrawLineFromOrign(crossP2.crossP[1],Qt::darkBlue);
//        smithchart.drawCrossPoint(PForLamda2);
//        phiLamda2 = smithchart.calLamdaFromCrossp(PForLamda2);
//        smithchart.setText(PForLamda2,QString("%1λ").arg(phiLamda2));
//        //cal and draw x circle
//        x1 = smithchart.calXfromCrossp(crossP2.crossP[1]);
//        smithchart.xCircle(x1,Qt::blue);
//        x1Point = smithchart.getRcoefandXCrossp(1,x1);
//        smithchart.drawCrossPoint(x1Point);
//        smithchart.setText(x1Point,QString("%1").arg(x1));
//        x2 = -x1;
//        smithchart.xCircle(x2,Qt::blue);
//        x2Point = smithchart.getRcoefandXCrossp(1,x2);
//        smithchart.calAndDrawLineFromOrign(x2Point,Qt::darkBlue);
//        smithchart.drawCrossPoint(x2Point);
//        //smithchart.setText(x2Point,QString("%1").arg(x2));
//        posLamda1 = smithchart.calLamdaFromCrossp(x2Point);
//        smithchart.setText(x2Point,QString("%1λ").arg(posLamda1));
//        if(ui->radio_shortStub->isChecked())
//        {
//            smithchart.drawShortPoint();
//            startAng = 180;
//            stopAng = atan2(x2Point.y(),x2Point.x());
//            stopAng = stopAng/M_PI*180;
//            qreal spanAng;
//            if(stopAng<0)
//                spanAng = 180-stopAng;
//            else
//                spanAng = stopAng-startAng;
//            if(spanAng<0)
//                spanAng = -spanAng;
//            else
//                spanAng = 360-spanAng;
//            resPoint = smithchart.calAndDrawArc(1,startAng,-spanAng,Qt::green);
//            if(phiLamda1>phiLamda2)
//            {
//                stubPos1 =(0.5- (phiLamda1-phiLamda2))*v/freq;
//            }
//            else
//            {
//                stubPos1 = (phiLamda2-phiLamda1)*v/freq;
//            }
//            stubLen1 = posLamda1*v/freq;
//        }
//        else
//        {
//            smithchart.drawOpenPoint();
//            startAng = 0;
//            stopAng = atan2(x2Point.y(),x2Point.x());
//            stopAng = stopAng/M_PI*180;
//            qreal spanAng;
//            if(stopAng<0)
//                spanAng = -stopAng;
//            else
//                spanAng = 360-stopAng;
//            if(spanAng<0)
//                spanAng = -spanAng;
//            resPoint = smithchart.calAndDrawArc(1,startAng,-spanAng,Qt::green);
//            if(phiLamda1>phiLamda2)
//            {
//                stubPos1 =(0.5- (phiLamda1-phiLamda2))*v/freq;
//            }
//            else
//            {
//                stubPos1 = (phiLamda2-phiLamda1)*v/freq;
//            }
//            if(posLamda1>0.25)
//                stubLen1 = (posLamda1-0.25)*v/freq;
//            else
//                stubLen1 = (posLamda1+0.25)*v/freq;
//        }
//        str = QString("Result 2: Pos l1=%1m\t stub l2=%2m").arg(stubPos1).arg(stubLen1);
//        resList3.append(str);
//        ui->label_SingleStubSsmithChart_2->setPixmap(*pImg);
//    }
    ui->label_SingleStubSsmithChart->setPixmap(*pImg);
    model3.setStringList(resList3);
}
