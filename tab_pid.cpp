#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_legend.h>
#include "tab_pid.h"

TabPID::TabPID(QWidget* parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;

    tempPlot = new QwtPlot( this);
    tempPlot->setAxisTitle(QwtPlot::yLeft,"Celsius");
    heaterPlot= new QwtPlot(this);
    heaterPlot->setAxisTitle(QwtPlot::yLeft,"Heating Power");
    QwtPlotGrid *myGrid = new QwtPlotGrid();
    myGrid->attach(tempPlot);
    QwtPlotGrid *myhGrid = new QwtPlotGrid();
    myhGrid->attach(heaterPlot);
    tempPlot->insertLegend(new QwtLegend());
    heaterPlot->insertLegend(new QwtLegend());
    tempPlot->setMaximumHeight(150);
    heaterPlot->setMaximumHeight(150);

    double x[2] = {0, 0};
    double y[2] = {0, 0};

    curve[hotend1] = new QwtPlotCurve("Hotend 1");
    curve[bed] = new QwtPlotCurve("Bed");
    curve[hotend2] = new QwtPlotCurve("Hotend 2");
    curve[heater] = new QwtPlotCurve("Heater");

    for(int i = 0; i < 4; i++)
    {
        curve[i]->setSamples(x, y, 2);
        curve[i]->setPen(QPen(QColor::fromHsv((255 * i) / 10, 255, 255), 2));

    }
    curve[0]->attach(tempPlot);
    curve[1]->attach(tempPlot);
    curve[2]->attach(tempPlot);
    curve[3]->attach(heaterPlot);
    tempPlot->replot();
    heaterPlot->replot();

    layout->addWidget(tempPlot);
    layout->addWidget(heaterPlot);

    QHBoxLayout *h1layout = new QHBoxLayout;

    QGroupBox *gb1 = new QGroupBox("Temperature:");
    QGroupBox *gb2 = new QGroupBox("PID values:");
    QGroupBox *gb3 = new QGroupBox("Ziegler-Nichols:");

    QGridLayout *gb1g = new QGridLayout;
    gb1g->addWidget(new QLabel("Hotend 1:"), 0, 0);
    gb1g->addWidget(temp[0] = new QLineEdit("0"), 0, 1);
    gb1g->addWidget(new QLabel("Bed:"), 1, 0);
    gb1g->addWidget(temp[1] = new QLineEdit("0"), 1, 1);
    gb1g->addWidget(new QLabel("Hotend 2:"), 2, 0);
    gb1g->addWidget(temp[2] = new QLineEdit("0"), 2, 1);
    gb1->setLayout(gb1g);
    temp[0]->setMaxLength(3);
    temp[1]->setMaxLength(3);
    temp[2]->setMaxLength(3);

    QGridLayout *gb2g = new QGridLayout;
    gb2g->addWidget(new QLabel("Proportional:"), 0, 0);
    gb2g->addWidget(pids[0] = new QLineEdit("0"), 0, 1);
    gb2g->addWidget(new QLabel("Integral:"), 1, 0);
    gb2g->addWidget(pids[1] = new QLineEdit("0"), 1, 1);
    gb2g->addWidget(new QLabel("Differential:"), 2, 0);
    gb2g->addWidget(pids[2] = new QLineEdit("0"), 2, 1);
    gb2g->addWidget(new QLabel("Extruder speed prop:"), 3, 0);
    gb2g->addWidget(pids[3] = new QLineEdit("0"), 3, 1);
    gb2g->addWidget(btLoad = new QPushButton("Load"), 4, 0);
    gb2->setLayout(gb2g);

    QGridLayout *gb3g = new QGridLayout;
    
    gb3g->addWidget(new QLabel("Critical prop. Gain:"), 0, 0);
    gb3g->addWidget(eCriticalGain = new QLineEdit("0"), 0, 1);
    gb3g->addWidget(btPeriod = new QPushButton("Period [sec]@critical:"), 1, 0);
    gb3g->addWidget(ePeriod = new QLineEdit("0"), 1, 1);
    gb3g->addWidget(cbZieglerDif = new QCheckBox("PI+D?"), 2, 0);
    //gb3g->addWidget(pids[2] = new QLineEdit("0"), 2, 1);
    gb3g->addWidget(btSet = new QPushButton("Set",this), 3, 0);
    //gb3g->addWidget(pids[3] = new QLineEdit("0"), 3, 1);
    gb3->setLayout(gb3g);

    h1layout->addWidget(gb1);
    h1layout->addWidget(gb2);
    h1layout->addWidget(gb3);
    layout->addLayout(h1layout);

    QHBoxLayout *h2layout = new QHBoxLayout;
    monitor = new QCheckBox("monitor temp", this);
    h2layout->addWidget(monitor);
    differential = new QCheckBox("Offset Temperature", this);
    h2layout->addWidget(differential);
    btClear = new QPushButton("Clear");
    h2layout->addWidget(btClear);
    h2layout->addWidget(new QLabel("Period [sec]:"));
    h2layout->addWidget(lPeriod = new QLabel("0"));

    h2layout->addWidget(new QLabel("Amplitude [C]:"));
    h2layout->addWidget(lAmp = new QLabel("0"));
    h2layout->addWidget(new QLabel("last Amplitude [C]:"));
    h2layout->addWidget(lAmpPrevious = new QLabel("0"));

    h2layout->addWidget(new QLabel("ratio:"));
    h2layout->addWidget(lAmpRatio = new QLabel("0"));

    layout->addLayout(h2layout);
    setLayout(layout);

    connect(btClear, SIGNAL(clicked(bool)), this, SLOT(clearClicked()));
    connect(btPeriod, SIGNAL(clicked(bool)), this, SLOT(periodClicked()));
    connect(btSet, SIGNAL(clicked(bool)), this, SLOT(setClicked()));
}


void TabPID::addData(double t1, double b, double t2, double h)
{
    const QTime now = QTime::currentTime();
    const double dt = currentTime.msecsTo(now);

    valueHotEnd1.push_back(t1);
    time.push_back(dt);
    valueBed.push_back(b);
    valueHotEnd2.push_back(t2);
    valueHeater.push_back(h / 255.0);

    targetHotEnd1.push_back(temp[hotend1]->text().toDouble());
    targetHotEnd2.push_back(temp[hotend2]->text().toDouble());
    targetBed.push_back(temp[bed]->text().toDouble());

    if(differential->checkState())
    {
        QVector<double> d[3];

        d[0].resize(valueHotEnd1.size());
        d[1].resize(valueHotEnd1.size());
        d[2].resize(valueHotEnd1.size());
        for(int i = 0; i < valueHotEnd1.size(); i++)
        {
            d[0][i] = valueHotEnd1[i] - targetHotEnd1[i];
            d[1][i] = valueBed[i] - targetBed[i];
            d[2][i] = valueHotEnd2[i] - targetHotEnd2[i];
        }
        curve[hotend1]->setSamples(time, d[0]);
        curve[hotend2]->setSamples(time, d[2]);
        curve[bed]->setSamples(time, d[1]);
    }
    else
    {
        curve[hotend1]->setSamples(time, valueHotEnd1);
        curve[hotend2]->setSamples(time, valueHotEnd2);
        curve[bed]->setSamples(time, valueBed);
    }
    curve[heater]->setSamples(time, valueHeater);
    tempPlot->replot();
    heaterPlot->replot();
    calculatePeriodicity();
}

void TabPID::calculatePeriodicity()
{
    if(valueHotEnd1.size() < 5)
        return;

    const double t = valueHotEnd1.last();
    int period = 0;
    int previouslast = valueHotEnd1.size() - 1;
    bool firstEventFound = false;

    for(int i = valueHotEnd1.size() - 2; i > 0; i--)
    {
        if( ((valueHotEnd1[i] >= t) && (valueHotEnd1[i-1] < t)) ||
            ((valueHotEnd1[i] <= t) && (valueHotEnd1[i-1] > t))  )
        {
            if(previouslast -i > 5)
            {
                if(firstEventFound)
                {

                    period = valueHotEnd1.size() - i;
                    lPeriod->setText(QString("%1").arg(period));
                    break;
                }
                else
                {
                    firstEventFound = true;
                    previouslast = i;
                }

            }
            else
            {
                //in proximity
                previouslast=i;
            }
        }
    }

    double minnow = 100000;
    double maxnow = 0;
    double minlast = 100000;
    double maxlast = 0;

    for(int i = valueHotEnd1.size() - 2; i > valueHotEnd1.size() - period; i--)
    {
        double &f = valueHotEnd1[i];

        if(f < minnow)
            minnow = f;

        if(f > maxnow)
            maxnow = f;
    }
    lAmp->setText(QString("%1").arg(maxnow - minnow));

    if(valueHotEnd1.size() - 2 * period)
    {
        for(int i = valueHotEnd1.size() - period; i > valueHotEnd1.size() - 2 * period; i--)
        {
            double &f = valueHotEnd1[i];

            if(f < minlast)
                minlast = f;

            if(f > maxlast)
                maxlast = f;
        }
        lAmpPrevious->setText(QString("%1").arg(maxlast - minlast));
    }
    lAmpRatio->setText(QString("%1").arg(lAmp->text().toDouble() / lAmpPrevious->text().toDouble()));
}

void TabPID::startTime()
{
    currentTime = QTime::currentTime();
}


void TabPID::clearClicked()
{
    time.resize(0);
    valueHotEnd1.resize(0);
    valueBed.resize(0);
    valueHotEnd2.resize(0);
    valueHeater.resize(0);

    targetHotEnd1.resize(0);
    targetHotEnd2.resize(0);
    targetBed.resize(0);
    startTime();

}

void TabPID::periodClicked()
{
    ePeriod->setText(lPeriod->text());
}

void TabPID::setClicked()
{
    const double ku = eCriticalGain->text().toDouble();
    const double t = ePeriod->text().toDouble();
    double kp = 0;
    double ki = 0;
    double kd = 0;

    if(cbZieglerDif->isChecked())
    {
        //PID
        kp = 0.6 * ku;
        ki = 2.0 * kp / t;
        kd = kp * t / 8.0;
    }
    else
    {
        //PI only
        kp = ku / 2.2;
        ki = 1.2 * kp / t;
        kd = 0;
    }
    pids[0]->setText(QString("%1").arg(kp));
    pids[1]->setText(QString("%1").arg(ki));
    pids[2]->setText(QString("%1").arg(kd));
    emit pidChanged();
}
