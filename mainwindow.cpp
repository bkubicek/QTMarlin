
#include "mainwindow.h"



#include <QList>
#include <QtDebug>



#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <vector>

#include <time.h>
using namespace std;


#include <QTimer>

#include <QFont>
#include <QPushButton>
#include <QWidget>
#include <QCheckBox>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_legend.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QThread>
#include <QStatusBar>
#include <QTreeWidget>
#include <QTabWidget>
#include <QComboBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QDir>

#include "tab_pid.h"
#include "tab_raw.h"
#include "tab_eeprom.h"

#ifdef USE_QSERIALDEVICE
  #include <serialdeviceenumerator.h>
#endif

MainWindow::MainWindow(QWidget *parent): QWidget(parent)
{      
  initSerial();
  
  //port = new AbstractSerial();
  QVBoxLayout *layout = new QVBoxLayout;
  
  QHBoxLayout *comLayout= new QHBoxLayout;
  portSelector=new QComboBox(this);
  baudSelector=new QComboBox(this);
  
  baudSelector->addItem("115200");
  baudSelector->addItem("57600");
  baudSelector->addItem("230400");
  baudSelector->addItem("250000");
  btConnect= new QPushButton("Connect");;
  btDisconnect=new QPushButton("Disconnect",this);
  btRescan= new QPushButton("Rescan",this);
  comLayout->addWidget(portSelector);
  comLayout->addWidget(baudSelector);
  comLayout->addWidget(btConnect);
  comLayout->addWidget(btDisconnect);
  comLayout->addWidget(btRescan);
  layout->addLayout(comLayout);
  tab=new QTabWidget();
  layout->addWidget(tab);
  
  tabPID=new TabPID(tab);
  tabRaw=new TabRaw(tab);
  tabEEPROM=new TabEEPROM(tab);
  
  tab->addTab(tabRaw,"Raw");
  tab->addTab(tabPID,"PID");
  tab->addTab(tabEEPROM,"EEPROM");
  
  status=new QStatusBar(this);
 
  
  layout->addWidget(status);
  //layout->addWidget(slider);
  setLayout(layout);
  clickedRefresh();
  
  connect(btConnect, SIGNAL(clicked(bool)), this, SLOT(clickedConnect()));
  connect(btDisconnect, SIGNAL(clicked(bool)), this, SLOT(clickedDisconnect()));
  connect(btRescan, SIGNAL(clicked(bool)), this, SLOT(clickedRefresh()));
  
  //connect(comport, SIGNAL(readyRead()), this, SLOT(slotRead()));
  connect(tabRaw->sendText,SIGNAL(returnPressed()),this, SLOT(manualSend()));
  
  
   connect(tabPID,SIGNAL(pidChanged()),this, SLOT(sendPID()));
  connect(tabPID->pids[0],SIGNAL(returnPressed()),this, SLOT(sendPID()));
  connect(tabPID->pids[1],SIGNAL(returnPressed()),this, SLOT(sendPID()));
  connect(tabPID->pids[2],SIGNAL(returnPressed()),this, SLOT(sendPID()));
  connect(tabPID->pids[3],SIGNAL(returnPressed()),this, SLOT(sendPID()));
  connect(tabPID->btLoad,SIGNAL(clicked()),this, SLOT(getPID()));
  
  connect(tabPID->temp[hotend1],SIGNAL(returnPressed()),this, SLOT(setHotend1Temp()));
  
  timer = new QTimer(this);
}

MainWindow::~MainWindow()
{
  closeSerial();
}

void MainWindow::initSerial()
{
  #ifdef USE_QEXTSERIALPORT
    comport=0;
  #endif

  #ifdef USE_QSERIALDEVICE
    comport = new AbstractSerial();
    connect(comport, SIGNAL(readyRead()), this, SLOT(slotRead()));
  #endif
}

void MainWindow::closeSerial()
{
  
  #ifdef USE_QEXTSERIALPORT
    comport->close();
    delete comport;
    comport=0;
  #endif

  #ifdef USE_QSERIALDEVICE
    comport->close();
  #endif
}

void MainWindow::openSerial()
{
  #ifdef USE_QEXTSERIALPORT
    if(comport)
      closeSerial();
  
    comport = new QextSerialPort(portSelector->currentText(), QextSerialPort::EventDriven);  
    connect(comport, SIGNAL(readyRead()), this, SLOT(slotRead()));

    QString baud=baudSelector->currentText();
    if(baud=="57600")
      comport->setBaudRate(BAUD57600);
    else if(baud=="115200")
      comport->setBaudRate(BAUD115200);
    else 
      cerr<<"Unsuppored baudrate"<<endl;

    if (comport->open(QIODevice::ReadWrite) != true) 
    {
      cout<<"failed opening comport:"<<portSelector->currentText().toStdString()<<endl;
      exit(1);
    }
  #endif

  #ifdef USE_QSERIALDEVICE
    comport->setDeviceName(portSelector->currentText());
    if (comport->open(AbstractSerial::ReadWrite | AbstractSerial::Unbuffered)) 
    {
       qDebug()<<"opened ok"<<endl;
    }
    else
    {
        qDebug()<<"failed opening comport:"<<portSelector->currentText()<<endl;
        //exit(1);
        return;
    }
    
    QString baud=baudSelector->currentText();
    if(baud=="115200")
    {
       if (!comport->setBaudRate(AbstractSerial::BaudRate115200)) 
       {
          qDebug() << "Set baud rate " <<  AbstractSerial::BaudRate115200 << " error.";
          return ;
      };
    }
    if(baud=="230400")
    {
       if (!comport->setBaudRate(AbstractSerial::BaudRate230400)) 
       {
          qDebug() << "Set baud rate " <<  AbstractSerial::BaudRate230400 << " error.";
          return ;
      };
    }
    if(baud=="250000")
    {
       if (!comport->setBaudRate(AbstractSerial::BaudRate250000)) 
       {
          qDebug() << "Set baud rate " <<  AbstractSerial::BaudRate250000 << " error.";
          return ;
      };
    }
    if(baud=="57600")
    {
       if (!comport->setBaudRate(AbstractSerial::BaudRate57600)) 
       {
          qDebug() << "Set baud rate " <<  AbstractSerial::BaudRate57600<< " error.";
          return ;
      };
    }
  #endif
}



void MainWindow::clickedConnect()
{
  openSerial();
 
  tabPID->startTime();
  
  connect(timer, SIGNAL(timeout()), this, SLOT(measure()));
  timer->start(1000);
  send("M301\n");
  serialBuffer="";
}

void MainWindow::clickedDisconnect()
{
  //comport->close();
  //disconnect(comport, SIGNAL(readyRead()), this, SLOT(slotRead()));
  closeSerial();
}

void MainWindow::clickedRefresh()
{
  QStringList portnames;
  #ifdef USE_QEXTSERIALPORT
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    stringstream ss;
    for (int i = 0; i < ports.size(); i++) 
      portnames<<(ports.at(i).physName);
  #endif

  #ifdef USE_QSERIALDEVICE
      
    #ifdef DONTUSE_QSERIALDEVICEENUMERATE
      QDir mydir("/dev");
      QStringList list = mydir.entryList(QDir::AllEntries|QDir::System);     
      foreach (QString s, list) 
      {
        if(s.startsWith("tty"))
          portnames<<QString("/dev/%1").arg(s));
      }
    #else
      SerialDeviceEnumerator *m_sde = SerialDeviceEnumerator::instance();
  
      QStringList list;
      list=m_sde->devicesAvailable();
      
      foreach (QString s, list) 
      {
        m_sde->setDeviceName(s);
        portnames<<m_sde->shortName();
      }
          
    #endif
  #endif
  
  //always:
  portSelector->clear();
  foreach (QString s, portnames) 
    portSelector->addItem(s);
}

void getdata(const QString &line,const QString &after, const QString &key,float &target)
{
   int n=line.indexOf(after);
   if(n==-1) return;
  QString t=line.mid(n,line.size());
  int m=t.indexOf(key)+key.size();
  if(m==-1)
    return;
  float f;
  QString end=t.mid(m, t.size()).split(" ")[0];
  f=end.toFloat();
  target=f;
  
  qDebug()<<end<<endl;
}
void MainWindow::slotRead() 
{
  QByteArray ba = comport->readAll();
  //qDebug() << "Readed is : " << ba.size() << " bytes";
  //qDebug()<<QString( ba);
  tabRaw->edit->insertPlainText(ba);
  serialBuffer=serialBuffer+QString(ba);
  int n=serialBuffer.lastIndexOf("\n");
  if(n==-1) return; //not even a full line
  QStringList lines = serialBuffer.mid(0,n).split("\n");
  serialBuffer=serialBuffer.mid(n,sizeof(serialBuffer)-n);
  foreach(QString s, lines)
  {
   if(s.startsWith("ok"))
   {
     QStringList junks(s.remove(0,3).split(" ",QString::SkipEmptyParts));
     foreach(QString j, junks)
     {
       //qDebug()<<j<<endl;
       QStringList ll=j.split(":",QString::SkipEmptyParts);
       if(ll.size()==2)
       {
        variables[ll[0]]= ll[1].toDouble();
       // qDebug()<<"Variable read:"<<QString(ll[0])<<"="<<variables[ll[0]]<<endl;
        if(ll[0]=="p")
        {
         tabPID->pids[0]->setText(ll[1]); 
        }
        if(ll[0]=="i")
        {
         tabPID->pids[1]->setText(ll[1]); 
        }
        if(ll[0]=="d")
        {
         tabPID->pids[2]->setText(ll[1]); 
        }
        if(ll[0]=="c")
        {
         tabPID->pids[3]->setText(ll[1]); 
        }
        
       }
     }
     tabPID->addData(variables["T"],variables["B"],0,variables["@"]);
     
   }
   else if(s.startsWith("echo:"))
   {
     
      float f;
      getdata(s,"M92","X",tabEEPROM->stepsperunit[0]);
      getdata(s,"M92","Y",tabEEPROM->stepsperunit[1]);
      getdata(s,"M92","Z",tabEEPROM->stepsperunit[2]);
      getdata(s,"M92","E",tabEEPROM->stepsperunit[3]);
      
      getdata(s,"M203","X",tabEEPROM->vmax[0]);
      getdata(s,"M203","Y",tabEEPROM->vmax[1]);
      getdata(s,"M203","Z",tabEEPROM->vmax[2]);
      getdata(s,"M203","E",tabEEPROM->vmax[3]);
      
      getdata(s,"M201","X",tabEEPROM->amax[0]);
      getdata(s,"M201","Y",tabEEPROM->amax[1]);
      getdata(s,"M201","Z",tabEEPROM->amax[2]);
      getdata(s,"M201","E",tabEEPROM->amax[3]);
      
      getdata(s,"M204","S",tabEEPROM->acceleration);
      getdata(s,"M204","T",tabEEPROM->accelerationRetract);
      
      getdata(s,"M205","S",tabEEPROM->vmin);
      getdata(s,"M205","T",tabEEPROM->vmin_travel);
      getdata(s,"M205","B",tabEEPROM->tmin_segment);
      getdata(s,"M205","X",tabEEPROM->vxyjerk);
      getdata(s,"M205","Z",tabEEPROM->vzjerk);
      
      qDebug()<<f<<endl;
     /*
      * float stepsperunit[4];
  float vmax[4];
  float amax[4];
  float acceleration, accelerationRetract;
  float vxyjerk,vzjerk;
  float vmin,vmin_travel;
  float tmin_segment;*/
   }
   
  }
}

void MainWindow::manualSend()
{
  send(tabRaw->sendText->text()+"\n");
}


void MainWindow::send(const QString &text)
{
  if(!comport->isOpen())
    return;
  QByteArray ba=text.toAscii(); //data to send
  qint64 bw = 0; //bytes really writed

  /* 5. Fifth - you can now read / write device, or further modify its settings, etc.
  */
  
  bw = comport->write(ba);
  //qDebug() << "Writen : " << bw << " bytes:"<<QString(ba);

}

void MainWindow::measure()
{
  send("M105\n");
}

void MainWindow::setHotend1Temp()
{
  send(QString("M104 S%1\n").arg(tabPID->temp[hotend1]->text()));
}

void MainWindow::sendPID()
{
  send(QString("M301 P%1 I%2 D%3 C%4\n").arg(tabPID->pids[0]->text()).arg(tabPID->pids[1]->text()).arg(tabPID->pids[2]->text()).arg(tabPID->pids[3]->text()));
}

void MainWindow::getPID()
{
  send(QString("M301\n"));
}