#ifndef TAB_PID_H
#define TAB_PID_H

#include <QWidget>
#include <QTime>
#include <QVector>

class QCheckBox;
class QPushButton;
class QLineEdit;
class QLabel;
class QwtPlotCurve;
class QwtPlot;

enum Curves {
    hotend1 = 0,
    bed = 1,
    hotend2 = 2,
    heater = 3
};

class TabPID : public QWidget
{
    Q_OBJECT

public:
    TabPID(QWidget *parent = 0);
    void addData(double t1, double b, double t2, double h);
    void startTime();
    void calculatePeriodicity();

    QPushButton *clear;
    QLineEdit *temp[3];
    QLineEdit *pids[4];

    QPushButton *btClear;
    QPushButton *btPeriod;
    QPushButton *btSet;
    QPushButton *btLoad;

    QCheckBox *monitor;

public slots:
    void clearClicked();
    void periodClicked();
    void setClicked();

private:
    QwtPlotCurve *curve[4];
    QwtPlot *tempPlot;
    QwtPlot *heaterPlot;

    QLabel *lPeriod;
    QLabel *lAmp;
    QLabel *lAmpPrevious;
    QLabel *lAmpRatio;
    QLineEdit *ePeriod;
    QLineEdit *eCriticalGain;

    QCheckBox *differential;
    QCheckBox *cbZieglerDif;

    QVector<double> time;
    QVector<double> valueHotEnd1;
    QVector<double> valueHotEnd2;
    QVector<double> valueBed;
    QVector<double> valueHeater;
    QVector<double> targetHotEnd1;
    QVector<double> targetHotEnd2;
    QVector<double> targetBed;
    QTime currentTime;

signals:
    void pidChanged();
};

#endif
