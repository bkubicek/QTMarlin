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
class QPushButton;

class TabEEPROM : public QWidget
{
Q_OBJECT

public:
  TabEEPROM(QWidget *parent = 0);


  float stepsperunit[4];
  float vmax[4];
  float amax[4];
  float acceleration, accelerationRetract;
  float vxyjerk,vzjerk;
  float vmin,vmin_travel;
  float tmin_segment;
  


public slots:
  //void setWasRead();
  //void quit();


private: 
  QPushButton *test;
};

