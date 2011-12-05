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


class QTextEdit;
class QLineEdit;

class TabRaw : public QWidget
{
Q_OBJECT

public:
  TabRaw(QWidget *parent = 0);
  void displayText(const QString &text);
  void displayTextHtml(const QString& text);

  QTextEdit *edit;
  QLineEdit *sendText;
  QPushButton *btClear;
  


public slots:
  //void setWasRead();
  //void quit();


private: 
  QPushButton *test;
};

