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





public slots:
  //void setWasRead();
  //void quit();


private: 
  QPushButton *test;
};

