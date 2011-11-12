#include <QWidget>
#include <fstream>

#include <stdint.h>
#include <QList>
#include <abstractserial.h>
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

class TabPID;
class TabRaw;
class TabEEPROM;
//class QextSerialPort;
class SerialDeviceEnumerator;
class AbstractSerial;

#include <QMap>


class MainWindow : public QWidget
{
Q_OBJECT

public:
  MainWindow(QWidget *parent = 0);



  void send(const QString &text);

  
  QMap<QString, float> variables;
  
public slots:
  void clickedConnect();
  void clickedDisconnect();
  void clickedRefresh();
  void slotRead();
  void manualSend();
  //void quit();
  

private: 
  QStatusBar *status;
  QTabWidget *tab;
  TabPID *tabPID;
  TabRaw *tabRaw;
  TabEEPROM *tabEEPROM;
  
  QComboBox *portSelector;
  QComboBox *baudSelector;
  QPushButton *btConnect;
  QPushButton *btDisconnect;
  QPushButton *btRescan;
  
  //QextSerialPort *comport;
  SerialDeviceEnumerator *m_sde;
  AbstractSerial *port;
  
};

