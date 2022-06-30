#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QFile>
#include<QString>
#include<QTableWidget>
#include<QAbstractItemView>
#include<QDebug>
#include<malloc.h>
#include<QDirIterator>

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //page 1
    //PorcessShow();
    //page 2

    setSysInfo();
    initGraph();
    timerConnect();

}

//void MainWindow::test()
//{


//}

void MainWindow::setSysInfo()
{
    auto a = getCPUInfo();

    for(int i=0; i<a.size();i++)
    {
           ui->cpuInfoListWidget->addItem(a[i]);
    }

    ui->sysInfoListWidget->addItem("OS Type:                Linux");
    ui->sysInfoListWidget->addItem("OS VerSion:             3.10.0-1062.el7.x86_64");
    ui->sysInfoListWidget->addItem("GCC Version:            4.8.5 20150623 (Red Hat 4.8.5-36");

    ui->cpuInfoListWidget->setMouseTracking(false);


}

QVector<QString> MainWindow::getCPUInfo()
{
    QString cpuName,cpuType,cpuHz,cpuCache;

    QFile file("/proc/cpuinfo");
    file.open(QIODevice::ReadOnly);
    file.readLine();
    cpuType = file.readLine();
     file.readLine();
    file.readLine();
    cpuName = file.readLine();
    file.readLine();
    cpuHz = file.readLine();
    cpuCache = file.readLine();


    file.close();

    QVector<QString> tmp;
    tmp.push_back(cpuName);
    tmp.push_back(cpuType);
    tmp.push_back(cpuHz);
    tmp.push_back(cpuCache);

    return tmp;

}



Item MainWindow::findProcessItem(QString path)
{
    Item tmp;

    QFile file(path+"/stat");
    QString str;
    QStringList list;
    file.open(QIODevice::ReadOnly);
    str = file.readLine();
    list = str.split(" ");
    file.close();

    tmp.ID = list[0];
    tmp.name = list[1].mid(1,list[1].size()-2);
    tmp.state = list[2];
    tmp.ppid = list[3];
    tmp.pgid = list[4];
    tmp.priority = list[17];
    tmp.nice = list[18];


    int i = list[23].toInt();

    if(i>1024)
    {
        qreal j= i/1024.0;
        tmp.mem = QString::number(j,10,1)+" MiB";
    }else {
        tmp.mem = list[23]+" KiB";
    }

    return tmp;

}

//get the single Item of the process
Item MainWindow::findItem(QString path)
{
    int i;
    QVector<Item> res;

    Item tmpIem;


    QFile file(path+"/status");
    file.open(QIODevice::ReadOnly);
    QString str;

    for(i=0; i<19;i++)
    {
        str = file.readLine();
        if(i==0||i==2||i==5||i==18)
        {
            str = pickData(str);

            switch (i)
            {
            case 0:
                tmpIem.name = str;
            case 2:
                tmpIem.state = str;
            case 5:
                tmpIem.ID = str;
            case 18:
                if(str.endsWith("kB"))
                {
                    tmpIem.mem = str;
                }
                else
                {
                    tmpIem.mem = "not applicable";
                }
            }

        }


    }
    file.close();

    QString state = tmpIem.state;
    tmpIem.state = state[1];//just need the second char

    return tmpIem;
}


void MainWindow::ChangeSortID(int type)
{

   sortID = type;
   PorcessShow();

}

void MainWindow::PorcessShow()
{

    ui->processTableWidget->setColumnCount(8);
    ui->processTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//yi ci xuanqu yihang
    ui->processTableWidget->setHorizontalHeaderLabels(QStringList() \
       <<"Name"<<"ID"<<"State"<<"Ppid"<<"Pgid"<<"Memory"<<"Priority"<<"nice");


    items = traverseDir();
    int size = items.size();

    ui->processTableWidget->setRowCount(size);
    int i;
    for(i=0;i<size;i++)
    {
        ui->processTableWidget->setItem(i,0,new QTableWidgetItem(QString(items[i].name)));
        ui->processTableWidget->setItem(i,1,new QTableWidgetItem(QString(items[i].ID)));
        ui->processTableWidget->setItem(i,2,new QTableWidgetItem(QString(items[i].ppid)));
        ui->processTableWidget->setItem(i,4,new QTableWidgetItem(QString(items[i].pgid)));
        ui->processTableWidget->setItem(i,5,new QTableWidgetItem(QString(items[i].mem)));
        ui->processTableWidget->setItem(i,6,new QTableWidgetItem(QString(items[i].priority)));
        ui->processTableWidget->setItem(i,7,new QTableWidgetItem(QString(items[i].nice)));
    }
}

//ID :1   priority:2  nice:3
QVector<Item> MainWindow::sort(QVector<Item> a,int type)
{
    int n = a.size();
    Item buf;


    for (int i=0; i<n-1; ++i)
        {
            for (int j=0; j<n-1-i; ++j)
            {
               if(type==1)
               {
                   if(a[j].ID.toInt()>a[j+1].ID.toInt())
                   {
                       buf = a[j];
                       a[j] = a[j+1];
                       a[j+1] = buf;
                   }
               }
               if(type==7)
               {
                   if(a[j].priority.toInt()<a[j+1].priority.toInt())
                   {
                       buf = a[j];
                       a[j] = a[j+1];
                       a[j+1] = buf;
                   }
               }
               if(type==3)
               {
                   if(a[j].nice.toInt()<a[j+1].nice.toInt())
                   {
                       buf = a[j];
                       a[j] = a[j+1];
                       a[j+1] = buf;
                   }
               }
               if(type==6)
               {
                   if(a[j].priority.toInt()<a[j+1].priority.toInt())
                   {
                       buf = a[j];
                       a[j] = a[j+1];
                       a[j+1] = buf;
                   }
               }
            }
        }
    return a;
}

//get the all the Item as a array
QVector<Item> MainWindow::traverseDir()
{
    QString DIR = "/proc";
    QDirIterator it(DIR);
    QVector<Item> data;
    while(it.hasNext())
    {
        bool isPidDir = it.fileName().toInt();
        if(isPidDir)
        {
            auto a = findProcessItem(DIR+"/"+it.fileName());
            data.push_back(a);
        }
        it.next();
    }

    data = sort(data,sortID);
    return data;
}
//simplefiy the readline
QString MainWindow::pickData(QString str)
{
    str = str.simplified();
    QChar ch = ':';
    int pos = str.indexOf(ch);
    str = str.remove(0,pos+1);
    return str;
}

QChart* MainWindow:: getChart(int type)
{
    QChart* tmp = new QChart();


    switch (type) {
    case 1:
        tmp->addSeries(CPUSeries);
        tmp->addSeries(CPUSeries1);
        break;
    case 2:
        tmp->addSeries(NetSeries1);
        tmp->addSeries(NetSeries2);
    case 3:
        tmp->addSeries(IOSeries1);
        tmp->addSeries(IOSeries2);
        break;
    default:
        break;
    }

    tmp->createDefaultAxes();
    tmp->legend()->hide();


    QValueAxis* X = new QValueAxis();
    X->setRange(0,60);
    X->setLabelFormat("%ds");
    X->setLabelsFont(QFont("song",8));

    QValueAxis* Y = new QValueAxis();
    switch (type) {
    case 1:

        Y->setRange(0,100);
        Y->setLabelFormat("%d%");
        Y->setLabelsFont(QFont("song",8));
        tmp->addAxis(Y,Qt::AlignRight);
        tmp->axisY()->setRange(0,100);
        break;
    case 2:
        Y->setRange(0,1000);
        Y->setLabelFormat("%db/s");
        Y->setLabelsFont(QFont("song",8));
        tmp->addAxis(Y,Qt::AlignRight);
        tmp->axisY()->setRange(0,1000);
        break;
    case 3:
        Y->setRange(0,100);
        Y->setLabelFormat("%d%");
        Y->setLabelsFont(QFont("song",8));
        tmp->addAxis(Y,Qt::AlignRight);
        tmp->axisY()->setRange(0,100);
        break;
    default:
        break;
    }

    tmp->addAxis(X,Qt::AlignBottom);
    tmp->setMargins(QMargins(0,0,0,0));

    tmp->axisX()->setRange(0,60);
    tmp->axisX()->setVisible(false);
    tmp->axisY()->setVisible(false);

    return tmp;
}

void MainWindow::updateIOLine()
{
    int TRANS = 1024*1024;
    QVector<qreal> tmp = readMemFile();

    MemDots.insert(0,tmp[0]/tmp[1]);
    SwapDots.insert(0,tmp[2]/tmp[3]);


    QString memstr = QString::number(tmp[0]/TRANS,10,1)+" GiB"
            +"("+QString::number(tmp[0]/tmp[1]*100,10,1)+"%"+")"
            + ",共 "+QString::number(tmp[1]/TRANS,10,1)+" GiB";
    ui->Io1->setText(memstr);

    QString swapstr = QString::number(tmp[3]-tmp[2],10,1)+"KiB"+
            " ("+QString::number((tmp[3]-tmp[2])/tmp[3],10,1)+") "+
        ",共 "+QString::number(tmp[3]/TRANS,10,1)+" GiB";


    ui->Io2->setText(swapstr);



    int dotMax = MemDots.size()<61?MemDots.size():61;
    IOSeries1->clear();
    for(int i=0; i<dotMax;i++)
    {
        IOSeries1->append(i,MemDots[i]*100);
    }

    dotMax = SwapDots.size()<61?SwapDots.size():61;
    IOSeries2->clear();
    for(int i=0; i<dotMax;i++)
    {
        IOSeries2->append(i,100-SwapDots[i]*100);
    }


}

QVector<qreal> MainWindow::readMemFile()
{
    QVector<qreal> tmp;
    QString str;


    QFile file("/proc/meminfo");
    file.open(QIODevice::ReadOnly);
    str = file.readAll();
    QStringList listAll = str.split("\n");


    file.readAll();
    file.close();

    QString MemTotal,MemFree,Buffers,Cached,Slab,SwapTotal,SwapFree;

    MemTotal = listAll[0];
    MemFree = listAll[1];
    Buffers = listAll[3];
    Cached = listAll[4];
    Slab = listAll[21];

    SwapTotal = listAll[14];
    SwapFree = listAll[15];


    qreal memTotal =pickMemData(MemTotal).toInt();
    qreal memUsed =  memTotal-pickMemData(MemFree).toInt()-pickMemData(Buffers).toInt()\
                                 -pickMemData(Cached).toInt()-pickMemData(Slab).toInt();

    qreal swapTotal =pickMemData(SwapTotal).toInt();
    qreal swapUsed = pickMemData(SwapFree).toInt();

    tmp.push_back(memUsed);
    tmp.push_back(memTotal);
    tmp.push_back(swapUsed);
    tmp.push_back(swapTotal);

    return tmp;
}

QString MainWindow::pickMemData(QString str)
{
    str = str.simplified();
    QChar ch = ':';
    int pos = str.indexOf(ch);
    str = str.remove(0,pos+2);
    str = str.left(str.size()-3);
    return str;
}

void MainWindow::timerConnect()
{
    timer = new QTimer();

    timer->setInterval(1000);
    connect(ui->ResourcesBut,&QPushButton::clicked,this,&MainWindow::updateCPULine);
    connect(timer,&QTimer::timeout,this,&MainWindow::updateCPULine);

    connect(ui->ResourcesBut,&QPushButton::clicked,this,&MainWindow::updateNetLine);
    connect(timer,&QTimer::timeout,this,&MainWindow::updateNetLine);

    connect(ui->ResourcesBut,&QPushButton::clicked,this,&MainWindow::updateIOLine);
    connect(timer,&QTimer::timeout,this,&MainWindow::updateIOLine);

    connect(ui->ProcessBut,&QPushButton::clicked,this,&MainWindow::PorcessShow);
    connect(timer,&QTimer::timeout,this,&MainWindow::PorcessShow);

    connect(ui->processTableWidget->horizontalHeader(),&QHeaderView::sectionClicked,this,&MainWindow::ChangeSortID);

}

void MainWindow::updateNetLine()
{
      NetSecDots.push_back(0);
      NetSecDots.push_back(0);
      getNetUsePercent();
 }

void MainWindow::getNetUsePercent()
{
    auto point = getNetSec();

    qreal recev = point[0]-NetSecDots[0];
    qreal trans = point[1]- NetSecDots[1];



    NetDots1.insert(0,recev);
    NetDots2.insert(0,trans);

    char tmp[20];
    sprintf(tmp,"rec:%.2f",recev);
    strcat(tmp," b/s");
    ui->net1->setText(tmp);


    char tmp1[20];
    sprintf(tmp1,"trans:%.2f",trans);
    strcat(tmp1," b/s");
    ui->net2->setText(tmp1);


    int readMax = NetDots1.size()<61?NetDots1.size():61;
    NetSeries1->clear();

    for(int i=0; i<readMax-1;i++)
    {
        NetSeries1->append(i-1,NetDots1[i]);
    }


    readMax = NetDots2.size()<61?NetDots2.size():61;
    NetSeries2->clear();
    for(int i=0; i<readMax-1;i++)
    {
        NetSeries2->append(i-1,NetDots2[i]);
    }
    NetSecDots[0] = point[0];
    NetSecDots[1] = point[1];

}

QVector<qreal> MainWindow::getNetSec()
{
    qreal tmp1,tmp2;
    QString str = readNetFile();
    QStringList list = str.split(" ");

    str = list[3];
    tmp1 = str.toInt();
    str = list[48];

    tmp2 = str.toInt();

    QVector<qreal> tmp;
    tmp.push_back(tmp1);
    tmp.push_back(tmp2);

    return tmp;
}

QString MainWindow::readNetFile()
{
    QString tmp;
    QFile file("/proc/net/dev");
    file.open(QIODevice::ReadOnly);
    file.readLine();
    file.readLine();
    tmp = file.readLine();
    file.close();
    return tmp;
}

void MainWindow::updateCPULine()
{
    CPUsecDot_1.push_back(0);
    CPUsecDot_1.push_back(0);

    CPUsecDot_2.push_back(0);
    CPUsecDot_2.push_back(0);
    getCPUUsePercent();


}

void  MainWindow:: getCPUUsePercent()
{
    QVector<qreal> data = getCPUSec();

    qreal finalUse = ( (data[0]-CPUsecDot_1[0]) - (data[1]-CPUsecDot_1[1]) )  / (data[0]-CPUsecDot_1[0]);
    qreal finalUse1 = ( (data[2]-CPUsecDot_2[0]) - (data[3]-CPUsecDot_2[1]) )  / (data[2]-CPUsecDot_2[0]);

    CPUDots1.insert(0,finalUse);
    char tmp[5];

    sprintf(tmp,"CPU1:%.2f",finalUse*100);
    strcat(tmp,"%");
    ui->cpu1->setText(tmp);

    CPUSeries->clear();
    int readMax = CPUDots1.size()<61?CPUDots1.size():61;
    for(int i=0;i<readMax-1;i++)
    {
        CPUSeries->append(i,CPUDots1[i]*100);
    }
    CPUDots2.insert(0,finalUse1);
    char tmp1[5];

    sprintf(tmp1,"CPU2:%.2f",finalUse1*100);
    strcat(tmp1,"%");
    ui->cpu2->setText(tmp1);
    CPUSeries1->clear();

    readMax = CPUDots2.size()<61?CPUDots2.size():61;
    for(int i=0;i<readMax-1;i++)
    {
        CPUSeries1->append(i,CPUDots2[i]*100);
    }

    CPUsecDot_1[0] = data[0];
    CPUsecDot_1[1]=data[1];

    CPUsecDot_2[0]=data[2];
    CPUsecDot_2[1]=data[3];

    CPUsecDot_2[0] = data[2];
    CPUsecDot_2[1] = data[3];

}

QVector<qreal> MainWindow::getCPUSec()
{
    QVector<QString> data = readCPUFile();
    qreal total1,rest1,total2,rest2;
    QString tmpStr;

    QString cpustr0 = data[0];
    QString cpustr1 = data[1];

    QStringList list1 = cpustr0.split(" ");
    QStringList list2 = cpustr1.split(" ");
    for(int i = 1;i<8;i++)
    {
        tmpStr = list1[i];
        total1+=tmpStr.toInt();
    }


    tmpStr = list1[4];
    rest1 += tmpStr.toInt();
    tmpStr = list1[5];
    rest1 += tmpStr.toInt();

    for(int j = 1;j<8;j++)
    {
        tmpStr=list2[j];
        total2+=tmpStr.toInt();
    }
    tmpStr = list2[4];
    rest2+=tmpStr.toInt();
    tmpStr = list2[5];
    rest2+=tmpStr.toInt();


    QVector <qreal> tmp;
    tmp.push_back(total1);
    tmp.push_back(rest1);
    tmp.push_back(total2);
    tmp.push_back(rest2);
    return tmp;
}

QVector<QString> MainWindow::readCPUFile()
{
    QString cpustr0;
    QString  cpustr1;
    QFile file("/proc/stat");
    file.open(QIODevice::ReadOnly);

    file.readLine();
    cpustr0 = file.readLine();
    cpustr1 = file.readLine();

    file.close();
    QVector<QString> tmp;
    tmp.push_back(cpustr0);
    tmp.push_back(cpustr1);
    return tmp;
}

void MainWindow::initGraph()
{
    CPUChart = getChart(1);
    ui->cpuWidget->setChart(CPUChart);
    ui->cpuWidget->setRenderHint(QPainter::Antialiasing);
    ui->cpuLineLabel1->setStyleSheet("QLabel{background-color:rgb(125,120,224);}");
    ui->cpuLineLabel2->setStyleSheet("QLabel{background-color:rgb(63,193,30);}");



    NetChart = getChart(2);
    ui->netWidget->setChart(NetChart);
    ui->netWidget->setRenderHint(QPainter::Antialiasing);
    ui->netLable1->setStyleSheet("QLabel{background-color:rgb(125,120,224);}");
    ui->netLable2->setStyleSheet("QLabel{background-color:rgb(63,193,30);}");


    IOChart = getChart(3);
    ui->IOWidget->setChart(IOChart);
    ui->IOWidget->setRenderHint(QPainter::Antialiasing);
    ui->IoLable1->setStyleSheet("QLabel{background-color:rgb(125,120,224);}");
    ui->IoLable2->setStyleSheet("QLabel{background-color:rgb(63,193,30);}");
}


void MainWindow::KillProcess()
{
    QList<QTableWidgetItem*>  its = ui->processTableWidget->selectedItems();
    if(!its.empty())
    {
        QTableWidgetItem* it = its.at(1);
        QString str = "kill -9 "+ it->text();

        char*  ch;
        QByteArray ba = str.toLatin1();

        ch=ba.data();
        system(ch);
    }

}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ProcessBut_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    timer->start(5000);
}

void MainWindow::on_ResourcesBut_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    timer->start(1000);

}

void MainWindow::on_FileSysBUt_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

//void MainWindow::on_testButton_clicked()
//{

//}

void MainWindow::on_KillBut_clicked()
{
    KillProcess();
    timer->setInterval(1);
}
