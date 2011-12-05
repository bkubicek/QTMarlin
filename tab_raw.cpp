#include "tab_raw.h"

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
#include <QTextEdit>
#include <QLabel>
#include <QLineEdit>


TabRaw::TabRaw(QWidget* parent): QWidget(parent)
{
  QVBoxLayout *layout = new QVBoxLayout;
  edit=new QTextEdit("",this);
  edit->setReadOnly(true);
  
  layout->addWidget(edit);
  //layout->addWidget(slider);
  
  QHBoxLayout *hlayout = new QHBoxLayout;
  hlayout->addWidget(new QLabel("Send:",this));
  sendText= new QLineEdit("",this);
  hlayout->addWidget(sendText);
  
  btClear=new QPushButton("clear",this);
  hlayout->addWidget(btClear);
  layout->addLayout(hlayout);
  setLayout(layout);

}

void TabRaw::displayText(const QString& text)
{
  edit->moveCursor(QTextCursor::End);
  edit->insertPlainText(text);
  if(!edit->underMouse())
    edit->ensureCursorVisible();
}

void TabRaw::displayTextHtml(const QString& text)
{
  edit->moveCursor(QTextCursor::End);
  edit->insertHtml(text);
  if(!edit->underMouse())
    edit->ensureCursorVisible();
}
