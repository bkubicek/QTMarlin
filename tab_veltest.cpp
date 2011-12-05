#include "tab_veltest.h"

#include <QPushButton>
#include <QFont>
#include <QPushButton>
#include <QWidget>
#include <QCheckBox>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_legend.h>
#include <qwt_symbol.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QThread>
#include <QStatusBar>
#include <QTreeWidget>
#include <QTabWidget>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>

TabVeltest::TabVeltest(MainWindow* _mw,QWidget *parent): QWidget(parent)
{
  mw=_mw;
  testing=false;
  QVBoxLayout *layout = new QVBoxLayout;
 
  QHBoxLayout *h1box=new QHBoxLayout(this);
  
  QGridLayout *g1=new QGridLayout(this);
  g1->addWidget(new QLabel(""),0,0);
  g1->addWidget(new QLabel("Vmin"),0,1);
  g1->addWidget(new QLabel("Vmax"),0,2);
  g1->addWidget(new QLabel("Amax"),0,3);
  
 
  g1->addWidget(doTest[0]=new QPushButton("X"),  1,0);
  g1->addWidget(vmin[0]=new QLineEdit("100"),    1,1);
  g1->addWidget(vmax[0]=new QLineEdit("500"),    1,2);
  g1->addWidget(amax[0]=new QLineEdit("10000"),  1,3);
  
  g1->addWidget(doTest[1]=new QPushButton("Y"),  2,0);
  g1->addWidget(vmin[1]=new QLineEdit("100"),    2,1);
  g1->addWidget(vmax[1]=new QLineEdit("500"),    2,2);
  g1->addWidget(amax[1]=new QLineEdit("10000"),  2,3);
  
  g1->addWidget(doTest[2]=new QPushButton("Z"),  3,0);
  g1->addWidget(vmin[2]=new QLineEdit("5"),    3,1);
  g1->addWidget(vmax[2]=new QLineEdit("30"),    3,2);
  g1->addWidget(amax[2]=new QLineEdit("10000"),  3,3);
  
  g1->addWidget(new QLabel("Teststeps"),  4,0);
  g1->addWidget(vsteps=new QLineEdit("5"),4,2);
  g1->addWidget(asteps=new QLineEdit("5"),4,3);
  g1->addWidget(new QLabel(""),5,0);
  g1->setColumnStretch(5,1);
  g1->setColumnStretch(0,0);
  
  connect(doTest[0],SIGNAL(clicked(bool)), this,SLOT(testX()));
  connect(doTest[1],SIGNAL(clicked(bool)), this,SLOT(testY()));
  connect(doTest[2],SIGNAL(clicked(bool)), this,SLOT(testZ()));
  
  h1box->addLayout(g1);  
  
  
  plot= new QwtPlot( this);
  
  //myPlot->setAxisTitle(QwtPlot::xBottom,"");
  plot->setAxisTitle(QwtPlot::yLeft,"a  mm/sec^2");
  plot->setAxisTitle(QwtPlot::xBottom,"v mm/sec");
  
  QwtPlotGrid *myGrid=new QwtPlotGrid();
  myGrid->attach(plot);
  plot->insertLegend(new QwtLegend());
  //double x[2]={0,0};
  //double y[2]={0,0};
  
  curveGood= new QwtPlotCurve("Good");
  curveFail = new QwtPlotCurve("Fail");
  
  //curveGood->setSamples(x,y,2);
  curveGood->setPen(QPen(Qt::blue,2));
  //curveFail->setSamples(x,y,2);
 curveFail->setPen(QPen(Qt::red,2));
  curveGood->setStyle(QwtPlotCurve::Dots);
  curveFail->setStyle(QwtPlotCurve::Dots);

  
  curveGood->attach(plot);
  curveFail->attach(plot);
  
  QwtSymbol *s=new QwtSymbol(QwtSymbol::Rect);
  s->setSize(10,10);
  s->setColor(Qt::blue);
  curveGood->setSymbol(s);
  
  QwtSymbol *s2=new QwtSymbol(QwtSymbol::Rect);
  s2->setSize(10,10);
  s2->setColor(Qt::red);
  curveFail->setSymbol(s2);
  plot->replot();
  

  h1box->addWidget(plot);
  layout->addLayout(h1box);

  //setLayout(layout);

}

void TabVeltest::testX()
{
  starttest(0);
}

void TabVeltest::testY()
{
  starttest(1);
}

void TabVeltest::testZ()
{
  starttest(2);
}

void TabVeltest::starttest(uint8_t axis)
{
  if(axis==2)
    return;
  
  float v,a;
  int nrV=vsteps->text().toInt();
  int nrA=asteps->text().toInt()+1;
  for(int i=0;i<=nrV;i++)
  {
      v=vmin[axis]->text().toFloat()+(vmax[axis]->text().toFloat()-vmin[axis]->text().toFloat())*i/float(nrV);
      for(int j=1;j<=nrA;j++)
      {
        a=amax[axis]->text().toFloat()*j/float(nrA);
        bool result=testSetting(axis,v,a,30,10);
        
      }
    
  }
  
  
  
}

void TabVeltest::startTestCase(TestCase& tc)
{
  float t=tc.v/tc.a;
  
  //float x=tc.a*t*t/2.*2; //10mm + full acceleration distance.
  float x=200;
  
  qDebug()<<"Test:"<<tc.a<<" "<<tc.v<<endl;
  mw->endstopfound=false;
  mw->sendGcode(QString("M205 S0 T0"));
  mw->sendGcode(QString("M205 S0 T0 %0%1 Z%2").arg(AxisChars[tc.axis]).arg(tc.xyjerk).arg(tc.zjerk));
  mw->sendGcode(QString("M202 %00%1 Y%2 ").arg(AxisChars[tc.axis]).arg(tc.a).arg(tc.a));
  mw->sendGcode(QString("M204 S%0 T%1 ").arg(tc.a).arg(tc.a));
  mw->sendGcode(QString("M203 %00%1 Y%2 ").arg(AxisChars[tc.axis]).arg(tc.v).arg(tc.v));
  mw->sendGcode(QString("G28 %000 ").arg(AxisChars[tc.axis]));


  mw->sendGcode(QString("G1 %00%1 F%2").arg(AxisChars[tc.axis]).arg(x).arg(tc.v*60));
  mw->sendGcode(QString("M400"));
  mw->sendGcode(QString("G1 %001 F%1").arg(AxisChars[tc.axis]).arg(tc.v*0.5*60));
  mw->sendGcode(QString("M400"));
  testing=true;
}

#include <QWaitCondition>
bool TabVeltest::testSetting(uint8_t axis, float v, float a, float xyjerk, float zjerk)
{
  struct TestCase tc;
  tc.axis=axis;
  tc.v=v;
  tc.a=a;
  tc.xyjerk=xyjerk;
  tc.zjerk=zjerk;
  float t=tc.v/tc.a;
  if(tc.a*t*t/2.*2>200) return false; 
  bufferedCases.push_back(tc);
  qDebug()<<"Adding testcase; "<<(testing?" already testing":"not testing until now")<<" a="<<a<<" v="<<v;
  if(!testing)
  {
    mw->sendGcode(QString("G28 X0 Y0 Z0"));
    mw->sendGcode(QString("G1 Z10 F400"));
    startTestCase(bufferedCases[0]);
    testing=true;
  }

  
  
}



void TabVeltest::checkDone()
{
  if(!testing)
    return;
  //qDebug()<<"checking"<<endl;
  if(mw->sendcodes.size()!=0)  //if mainwindow is still sending, not finished
    return;
  qDebug()<<"no more sending";
  if(bufferedCases.size()==0) //if cue is empty, we are finished
  {
    testing=false;
    return;
  }
  //qDebug()<<"still stuff do do."<<endl;
  qDebug()<<"Testcase finished";
  //we here know we have at least one buffered case
  bool badresult=(mw->endstopfound);
  if(!badresult)
  {
    sucess_x.push_back(bufferedCases[0].v);
    sucess_y.push_back(bufferedCases[0].a);
    curveGood->setSamples(sucess_x,sucess_y);
  }
  else
  {
    fail_x.push_back(bufferedCases[0].v);
    fail_y.push_back(bufferedCases[0].a);
    curveFail->setSamples(fail_x,fail_y);
  }
  plot->replot();
  
  
  
  float lasta,lastv;
  lasta=bufferedCases[0].a;
  lastv=bufferedCases[0].v;
  bufferedCases.removeFirst();
  qDebug()<<"still buffered"<<bufferedCases.size()<<endl;
  if(bufferedCases.size())
  {
    if(!badresult)
      startTestCase(bufferedCases[0]);
    else
    {
     while(bufferedCases[0].a>=lasta && bufferedCases[0].v>=lastv)
     {
        bufferedCases.removeFirst();
        if(bufferedCases.size()==0)
        {
            qDebug()<<"Testing done"<<endl;;
            testing=false;
            break;
        }
     }
       
    }
  }
  else
  {
    qDebug()<<"Testing done"<<endl;;
    testing=false;
  }   
  
  
}
