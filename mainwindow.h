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

class TabPID;
class MainWindow : public QWidget
{
//Q_OBJECT

public:
  MainWindow(QWidget *parent = 0);





public slots:
  //void setWasRead();
  //void quit();


private: 
  QStatusBar *status;
  QTabWidget *tab;
  TabPID *tabPID;
  
  QComboBox *portSelector;
  QComboBox *baudSelector;
  QPushButton *btConnect;
  QPushButton *btDisconnect;
  QPushButton *btRescan;
};

