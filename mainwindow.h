#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QValueAxis>
#include <QTimer>
#include "QDateTime"
#include<QChart>
#include <QTimer>
QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE



typedef struct info
{
   QString name;
   QString ppid;
   QString pgid;
   QString mem;
   QString ID;
   QString state;
   QString nice;
   QString priority;
}Item;


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    QTimer* timer;
    //process

    int sortID;
    Item findProcessItem(QString);
    Item findItem(QString num);

    QVector<Item> sort(QVector<Item>,int type);

    QVector<Item> items;
    QVector<Item> traverseDir( );
    QString pickData(QString str);


    //graph
    void initGraph();
    QChart* getChart(int);
    QChart* CPUChart;
    QChart* NetChart;
    QChart* IOChart;
    QVector<qreal> CPUsecDot_1;
    QVector<qreal> CPUsecDot_2;
    QVector<qreal> CPUDots1;
    QVector<qreal> CPUDots2;
    QVector<qreal> NetSecDots;
    QVector<qreal> NetDots1;
    QVector<qreal> NetDots2;



    QVector<qreal> MemDots;
    QVector<qreal> SwapDots;

    QSplineSeries* CPUSeries = new QSplineSeries();
    QSplineSeries* CPUSeries1 = new QSplineSeries();
    QSplineSeries* NetSeries1 = new QSplineSeries();
    QSplineSeries* NetSeries2 = new QSplineSeries();
    QSplineSeries* IOSeries1 = new QSplineSeries();
    QSplineSeries* IOSeries2 = new QSplineSeries();

    //CPUgraph
    QVector<qreal> getCPUSec();
    QVector<QString> readCPUFile();
    void timerConnect();
    void getCPUUsePercent();


    //NetGraph
    QString readNetFile();
    QVector<qreal> getNetSec();
    void getNetUsePercent();


    //IO and Swap

    QVector<qreal> readMemFile();
    QString pickMemData(QString);


    //sysInfo
    void setSysInfo();
    QVector<QString> getCPUInfo();
    QVector<QString> getOSInfo();


    void test();

    void KillProcess();
    ~MainWindow();
private slots:

    void ChangeSortID(int);
    void PorcessShow();
    void updateIOLine();
    void updateCPULine();
    void updateNetLine();

    void on_ProcessBut_clicked();

    void on_ResourcesBut_clicked();

    void on_FileSysBUt_clicked();


//    void on_testButton_clicked();

    void on_KillBut_clicked();

private:
    Ui::MainWindow *ui;


};

#endif // MAINWINDOW_H
