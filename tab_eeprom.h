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
class QLineEdit;
class QLabel;

class TabEEPROM : public QWidget
{
Q_OBJECT

public:
  TabEEPROM(QWidget *parent = 0);


//   float stepsperunit[4];
//   float vmax[4];
//   float amax[4];
//   float acceleration, accelerationRetract;
//   float vxyjerk,vzjerk;
//   float vmin,vmin_travel;
//   float tmin_segment;
  
  QLineEdit *lEstepsperunit[4];
  QLineEdit *lEvmax[4];
  QLineEdit *lEamax[4];
  QLineEdit *lEacceleration;
  QLineEdit *lEaccelerationRetract;
  QLineEdit *lEvxyjerk;
  QLineEdit *lEvzjerk;
  QLineEdit *lEvmin;
  QLineEdit *lEvmin_travel;
  QLineEdit *lEtmin_segment;
  QLineEdit *lEPIDp,*lEPIDd,*lEPIDi;
  
  QPushButton *pbRead,*pbWrite,*pbStore,*pbReset,*pbFactorReset;
  
  QLabel *lbAccDis,*lbAccTime;
  QLabel *lbAccDis100,*lbAccTime100;
  QLabel *lbStepRate;
  void EEPROM_recalculate();
public slots:
  //void setWasRead();
  //void quit();
  void changed();


private: 
  QPushButton *test;
};

