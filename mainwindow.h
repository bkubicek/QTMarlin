#ifndef __MAINWINDOWH
#define __MAINWINDOWH
#include <QWidget>
#include <fstream>

#include <stdint.h>
#include <QList>






class MyThread;
class QwtPlotCurve;
class QwtPlot;
class QStatusBar;
class QCheckBox;
class QTreeWidget;
class QTreeWidgetItem;
class QTabWidget;
class QComboBox;
class QPushButton;
class QLineEdit;

class TabPID;
class TabRaw;
class TabEEPROM;
class TabVeltest;
//class QextSerialPort;
class SerialDeviceEnumerator;
class AbstractSerial;
class QTimer;

#include <QMap>

//user choices:
//#define USE_QEXTSERIALPORT  //if comment QSerialDevice will be used
//#define DONTUSE_QSERIALDEVICEENUMERATE  //OSX fix for QSerialDevice

#ifndef USE_QEXTSERIALPORT
  #define USE_QSERIALDEVICE
#endif




#ifdef USE_QEXTSERIALPORT
  #include "qextserialport.h"
  #include <qextserialenumerator.h>
#endif

#ifdef USE_QSERIALDEVICE
  #include <abstractserial.h>
#endif

class MainWindow : public QWidget
{
Q_OBJECT

public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();


  void send(QString text);
  void sendGcode(const QString &text); //wait for "ok"

  QStringList sendcodes;
  QMap<QString, float> variables;
  
  bool wait_reply;
  bool endstopfound;
  
  
public slots:
  void clickedConnect();
  void clickedDisconnect();
  void clickedRefresh();
  void slotRead();
  void manualSend();
  void measure();
  void setHotend1Temp();
  
  void sendPID();
  void getPID();
  //void quit();
  
  void processReply();
  
  
  void EEPROM_loadClicked();
  void EEPROM_writeClicked();
  void EEPROM_storeClicked();
  void EEPROM_resetClicked();
  void EEPROM_factorResetClicked();
  
signals:
  void newSerialData();

private: 
  QStatusBar *status;
  QTabWidget *tab;
  TabPID *tabPID;
  TabRaw *tabRaw;
  TabEEPROM *tabEEPROM;
  TabVeltest *tabVeltest;
  
  QComboBox *portSelector;
  QComboBox *baudSelector;
  QPushButton *btConnect;
  QPushButton *btDisconnect;
  QPushButton *btRescan;
  
  #ifdef USE_QEXTSERIALPORT
    QextSerialPort *comport;
  #endif

  #ifdef USE_QSERIALDEVICE  
    AbstractSerial *comport;
  #endif
  QString serialBuffer; //so only full lines are handed to the next stuff.
  QTimer *timer; 

private: //functions
  void initSerial();
  void openSerial();
  void closeSerial();
  
  void getdata(const QString &line,const QString &after, const QString &key,float &target);
  void getdata(const QString &line,const QString &after, const QString &key,QLineEdit *target);
  
  QByteArray serialBinBuffer;
  QString readSinceLastSend;
  bool pidloaded;
};

#endif