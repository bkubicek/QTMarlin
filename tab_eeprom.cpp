#include "tab_eeprom.h"

#include <QPushButton>
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

TabEEPROM::TabEEPROM(QWidget* parent): QWidget(parent)
{
  QVBoxLayout *layout = new QVBoxLayout;
  test=new QPushButton("test",this);
  
  layout->addWidget(test);
  //layout->addWidget(slider);
  setLayout(layout);

}