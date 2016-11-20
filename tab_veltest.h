#ifndef TAB_VELTEST_H
#define TAB_VELTEST_H

#include <QWidget>
#include <QList>
#include <QVector>
#include "mainwindow.h"

class QPushButton;
class QLineEdit;
class QLabel;
class QwtPlotCurve;
class QwtPlot;

struct TestCase
{
    uint8_t axis;
    double v;
    double a;
    double xyjerk;
    double zjerk;
};

class TabVelTest : public QWidget
{
    Q_OBJECT

public:
    TabVelTest(MainWindow* _mw, QWidget *parent);

    void calculatePeriodicity();
    void starttest(uint8_t axis);
    bool testSetting(uint8_t axis, double v, double a, double xyjerk, double zjerk);
    void startTestCase(TestCase &tc);

public slots:
    void testX();
    void testY();
    void testZ();
    void checkDone();

private:
    QwtPlotCurve *curveGood;
    QwtPlotCurve *curveFail;
    QwtPlot *plot;

    QLineEdit *vmax[4];
    QLineEdit *vmin[4];
    QLineEdit *vsteps;
    QLineEdit *amax[4];
    QLineEdit *asteps;
    QPushButton *doTest[3];

    QVector<double> sucess_x;
    QVector<double> sucess_y;
    QVector<double> fail_x;
    QVector<double> fail_y;

    MainWindow *mw;
    QList<TestCase> bufferedCases;
    bool testing;
};

#endif
