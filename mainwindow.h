#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logging.h"
#include "nanovna.h"
#include "callout.h"
#include <QValidator>
#include <QSplineSeries>
#include <QChart>
#include <QTimer>
#include <callout.h>
using namespace QtCharts;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
enum ReadStatus {None,S11AndS21,S11,S21,OpenCal,shortCal,LoadCal,ThruCal};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_pbConnect_clicked();

    void on_pbDisConnect_clicked();

    void on_pbConfig_clicked();

    void on_pbOpenCal_clicked();

    void on_pbShortCal_clicked();

    void on_pbLoadCal_clicked();

    void on_pbThruCal_clicked();

    void on_pbDoneCal_clicked();

    void on_pbResetCal_clicked();

    void on_pbSingleStep_clicked();

    void on_pbRun_clicked();

    void on_pbStop_clicked();

    void on_pbSaveImage_clicked();

    void onVnaReadyRead();
    void onVnaError(QSerialPort::SerialPortError error);
    void onTimerOut();
    void on_s11SeriesHovered(const QPointF &point, bool state);
    void on_s21SeriesHovered(const QPointF &point, bool state);

    void on_pbSaveData_clicked();

    void on_pbLoadData_clicked();

    void on_pbSaveUIImage_clicked();

private:
    void setSelfValidator();
    void availableSerialDevice();
    void initChart();
    void initTimer();
    bool initVNA(QString comName,QString baudRate);
    void closeEvent(QCloseEvent* e) override;

private:
    Ui::MainWindow *ui;
    //store the debug log
    logging log;
    //chart
    QChart sParameterChart;
    QSplineSeries s11Series,s21Series;
    Callout* pS11Callout;
    Callout* pS21Callout;
    // VNA
    enum ReadStatus vnaStatus;
    NanoVNA *pVna;
    bool isVnaOpened;
    QString retMsg;
    //timer
    QTimer timer;
};
#endif // MAINWINDOW_H
