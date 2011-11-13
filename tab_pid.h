#include <QWidget>
#include <fstream>

#include <stdint.h>
#include <QList>
#include <QTime>
#include <QVector>

class MyThread;
class QwtPlotCurve;
class QwtPlot;
class QStatusBar;
class QCheckBox;
class QTreeWidget;
class QTreeWidgetItem;
class QTabWidget;
class QPushButton;
class QLineEdit;
class QLabel;

enum Curves {hotend1=0,bed=1,hotend2=2,heater=3};

class TabPID : public QWidget
{
Q_OBJECT

public:
  TabPID(QWidget *parent = 0);
  void addData(float t1,float b,float t2,float h);
  void startTime();
  void calculatePeriodicity();


 QwtPlotCurve *curve[4] ;
 QwtPlot *tempPlot,*heaterPlot;
 
 QPushButton *clear;
 QLineEdit *temp[3];
 QLineEdit *pids[4];
 
 QVector<double> time;
 QVector<double> value_hotend1,value_hotend2,value_bed,value_heater;
 QVector<double> target_hotend1,target_hotend2,target_bed;
 QTime starttime;
 QCheckBox *differential,*monitor;
 QPushButton *btClear, *btPeriod, *btSet,*btLoad;
 QLabel *lPeriod,*lAmp,*lAmpPrevious,*lAmpRatio;
 QLineEdit *ePeriod,*eCriticalGain;
 QCheckBox *cbZieglerDif;
 
public slots:
  //void setWasRead();
  //void quit();
  void clearClicked();
  void periodClicked();
  void setClicked();
signals:
  void pidChanged();
private: 
  QPushButton *test;
};

