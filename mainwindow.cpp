
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
#include "tab_veltest.h"

#ifdef USE_QSERIALDEVICE
  #include <serialdeviceenumerator.h>
#endif

MainWindow::MainWindow(QWidget *parent): QWidget(parent)
{      
  initSerial();
  
  //port = new AbstractSerial();
  serialBinBuffer.resize(0);
  wait_reply=false;
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
  tabVeltest=new TabVeltest(this,0);
  
  tab->addTab(tabRaw,"Raw");
  tab->addTab(tabPID,"PID");
  tab->addTab(tabEEPROM,"EEPROM");
  tab->addTab(tabVeltest,"VelTest");
  
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
  
  connect(this,SIGNAL(newSerialData()),this,SLOT(processReply()));
  timer = new QTimer(this);
  connect(timer,SIGNAL(timeout()),tabVeltest,SLOT(checkDone()));
  pidloaded=false;
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
  //getPID();
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
          portnames<<QString("/dev/%1").arg(s);
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
  
  //qDebug()<<end<<endl;
}
void MainWindow::slotRead() 
{
  QByteArray ba = comport->readAll();
  serialBinBuffer.append(ba);
  int lastnewlinepos=serialBinBuffer.lastIndexOf('\n');
  qDebug()<<"newline@"<<lastnewlinepos;
  if(lastnewlinepos<0) 
    return; //no newline read yet.
  QString readlines=QString(serialBinBuffer.mid(0,lastnewlinepos).append((char)0));
  readSinceLastSend.append(readlines);
  QStringList lines=readlines.split("\n",QString::SkipEmptyParts);
  serialBinBuffer.remove(0,lastnewlinepos);
  tabRaw->edit->insertPlainText(readlines);
  
  foreach(QString s, lines) //s =  linecontent
  {
    qDebug()<<"read line:"<<s;
   if(s.contains("endstop"))
     endstopfound=true;
   if(s.startsWith("ok") ||s.startsWith("T"))
   {
     QStringList junks(s.remove(0,3).split(" ",QString::SkipEmptyParts));
     foreach(QString j, junks)
     {
       //qDebug()<<j<<endl;
       QStringList ll=j.split(":",QString::SkipEmptyParts);
       if(ll.size()==2)
       {
         bool ok=false;
         float f=ll[1].toDouble(&ok);
         if(ok)
            variables[ll[0]]=f ;
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
      
      //qDebug()<<f<<endl;
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
  emit newSerialData();
}

void MainWindow::manualSend()
{
  send(tabRaw->sendText->text()+"\n");
}


void MainWindow::send(QString text)
{
  if(!comport->isOpen())
    return;
  QString text2=text.append("\n");
  QByteArray ba=text2.toAscii(); //data to send
  qint64 bw = 0; //bytes really writed

  bw = comport->write(ba);

}

void MainWindow::sendGcode(const QString &text)
{
  qDebug()<<"gcode cue list:"<<sendcodes.size();
  sendcodes<<text;
  if(wait_reply)
    return;
  
  send(sendcodes[0]);
  readSinceLastSend="";
  wait_reply=true;
}

void MainWindow::processReply()
{
  if(sendcodes.size()==0)
    return;
  
  QStringList lines = readSinceLastSend.split("\n",QString::SkipEmptyParts);
  qDebug()<<"got reply for command "<<sendcodes[0];
  
  QString overhang="";
  foreach(QString s,lines)
  {
    if(overhang.size())
    {
      overhang.append(s);
      //qDebug()<<"Appeneded Overhang:"<<overhang<<endl;
    }
    else
      overhang=s;
    
    if(overhang.startsWith("ok"))
    {
       qDebug()<<"ack ok:"<<overhang<<endl;
      if(sendcodes.size())
      {
        sendcodes.removeFirst();
        if(sendcodes.size())
        {
          readSinceLastSend="";
          send(sendcodes[0]);
        }
        else
            wait_reply=false;
      }
      
    }
    else
    {
     // qDebug()<<"ack not ok:"<<overhang<<endl;
    }
    if(s.size()<2)
      overhang=s;
    else
      overhang="";
    //qDebug()<<"Overhang:"<<overhang<<endl;
  }
  
}

void MainWindow::measure()
{
  //qDebug()<<"measure"
  if(!pidloaded)
  {
     getPID();
      pidloaded=true;
  }
  else
  if(tabPID->monitor->isChecked())
    sendGcode("M105");
  
}

void MainWindow::setHotend1Temp()
{
  sendGcode(QString("M104 S%1").arg(tabPID->temp[hotend1]->text()));
}

void MainWindow::sendPID()
{
  sendGcode(QString("M301 P%1 I%2 D%3 C%4").arg(tabPID->pids[0]->text()).arg(tabPID->pids[1]->text()).arg(tabPID->pids[2]->text()).arg(tabPID->pids[3]->text()));
}

void MainWindow::getPID()
{
  sendGcode(QString("M301"));
}