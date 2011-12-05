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

#include "mainwindow.h"

const char AxisChars[]={'X','Y','Z','E'};
struct TestCase
{
  uint8_t axis;
  float v,a,xyjerk,zjerk;
};

class TabVeltest : public QWidget
{
Q_OBJECT

public:
  TabVeltest(MainWindow* _mw,QWidget *parent);

  void calculatePeriodicity();
  void starttest(uint8_t axis);
  bool testSetting(uint8_t axis, float v, float a, float xyjerk, float zjerk);
  void startTestCase(TestCase &tc);


 QwtPlotCurve *curveGood,*curveFail ;
 QwtPlot *plot;

 
 QLineEdit *vmax[4],*vmin[4],*vsteps;
 QLineEdit *amax[4],*asteps;
 QPushButton *doTest[3];
 
 QVector<double> sucess_x,sucess_y;
 QVector<double> fail_x,fail_y;
 
 MainWindow *mw;
 QList<TestCase> bufferedCases;

 
public slots:
  void testX();
  void testY();
  void testZ();
  void checkDone();

signals:

private: 
  bool testing;
};

