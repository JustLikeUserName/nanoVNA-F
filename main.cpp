#include "mainwindow.h"

#include <QApplication>
#include <QSplashScreen>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //author information
    QString str = "开启便携式网络分析仪测试之旅\nAuthor:曹洪龙\nEmail:caohonglong@suda.edu.cn";
    QSplashScreen* splashScreen= new QSplashScreen();
    splashScreen->setFixedSize(500,200);
    splashScreen->showMessage(str,Qt::AlignCenter,Qt::blue);
    splashScreen->show();
    //delay
    for(int i=0;i<25000;++i)
    {
        for(int i=0;i<25000;++i);
    }
    //a.processEvents();
    MainWindow w;
    w.show();
    if(splashScreen)
        delete splashScreen;
    return a.exec();
}
