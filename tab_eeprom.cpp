#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include "tab_eeprom.h"

TabEEPROM::TabEEPROM(QWidget* parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;

    QGridLayout *g1 = new QGridLayout;
    QGridLayout *g2 = new QGridLayout;
    QGridLayout *g3 = new QGridLayout;
    QGridLayout *g4 = new QGridLayout;
    QWidget *w1 = new QWidget;
    QWidget *w2 = new QWidget;
    QWidget *w3 = new QWidget;
    QWidget *w4 = new QWidget;
    w1->setLayout(g1);
    w2->setLayout(g2);
    w3->setLayout(g3);
    w4->setLayout(g4);

    uint8_t curline = 0;

    g1->addWidget(new QLabel(""), curline, 0);
    g1->addWidget(new QLabel("X"), curline, 1);
    g1->addWidget(new QLabel("Y"), curline, 2);
    g1->addWidget(new QLabel("Z"), curline, 3);
    g1->addWidget(new QLabel("E"), curline, 4);

    curline = 1;
    g1->addWidget(new QLabel("steps per mm"), curline, 0);
    g1->addWidget(lEstepsPerUnit[0] = new QLineEdit("0"), curline, 1);
    g1->addWidget(lEstepsPerUnit[1] = new QLineEdit("0"), curline, 2);
    g1->addWidget(lEstepsPerUnit[2] = new QLineEdit("0"), curline, 3);
    g1->addWidget(lEstepsPerUnit[3] = new QLineEdit("0"), curline, 4);
    connect(lEstepsPerUnit[0], SIGNAL(editingFinished()), this, SLOT(changed()));
    connect(lEstepsPerUnit[1], SIGNAL(editingFinished()), this, SLOT(changed()));
    connect(lEstepsPerUnit[2], SIGNAL(editingFinished()), this, SLOT(changed()));
    connect(lEstepsPerUnit[3], SIGNAL(editingFinished()), this, SLOT(changed()));

    curline = 2;
    g1->addWidget(new QLabel("v max [mm/sec]"), curline, 0);
    g1->addWidget(lEvmax[0] = new QLineEdit("0"), curline, 1);
    g1->addWidget(lEvmax[1] = new QLineEdit("0"), curline, 2);
    g1->addWidget(lEvmax[2] = new QLineEdit("0"), curline, 3);
    g1->addWidget(lEvmax[3] = new QLineEdit("0"), curline, 4);
    connect(lEvmax[1], SIGNAL(editingFinished()), this, SLOT(changed()));
    connect(lEvmax[2], SIGNAL(editingFinished()), this, SLOT(changed()));
    connect(lEvmax[0], SIGNAL(editingFinished()), this, SLOT(changed()));
    connect(lEvmax[3], SIGNAL(editingFinished()), this, SLOT(changed()));

    curline = 3;
    g1->addWidget(new QLabel("a max [mm/sec^2]"), curline, 0);
    g1->addWidget(lEamax[0] = new QLineEdit("0"), curline, 1);
    g1->addWidget(lEamax[1] = new QLineEdit("0"), curline, 2);
    g1->addWidget(lEamax[2] = new QLineEdit("0"), curline, 3);
    g1->addWidget(lEamax[3] = new QLineEdit("0"), curline, 4);
    connect(lEamax[0], SIGNAL(editingFinished()), this, SLOT(changed()));
    connect(lEamax[1], SIGNAL(editingFinished()), this, SLOT(changed()));
    connect(lEamax[2], SIGNAL(editingFinished()), this, SLOT(changed()));
    connect(lEamax[3], SIGNAL(editingFinished()), this, SLOT(changed()));


    curline = 0;
    g2->addWidget(new QLabel(""), curline, 0);
    g2->addWidget(new QLabel(""), curline, 1);

    curline++;
    g2->addWidget(new QLabel("acceleration [mm/sec^2]"), curline, 0);
    g2->addWidget(lEacceleration = new QLineEdit("0"), curline, 1);
    g2->addWidget(new QLabel("retract acceleration [mm/sec^2]"), curline, 2);
    g2->addWidget(lEaccelerationRetract = new QLineEdit("0"), curline, 3);
    connect(lEacceleration, SIGNAL(editingFinished()), this, SLOT(changed()));
    connect(lEaccelerationRetract, SIGNAL(editingFinished()), this, SLOT(changed()));

    curline++;
    g2->addWidget(new QLabel("Jerk velocity xy [mm/sec]"), curline, 0);
    g2->addWidget(lEvxyJerk = new QLineEdit("0"), curline, 1);
    g2->addWidget(new QLabel("Jerk velocity z [mm/sec]"), curline, 2);
    g2->addWidget(lEvzJerk = new QLineEdit("0"), curline, 3);
    connect(lEvxyJerk, SIGNAL(editingFinished()), this, SLOT(changed()));
    connect(lEvzJerk, SIGNAL(editingFinished()), this, SLOT(changed()));

    curline++;
    g2->addWidget(new QLabel("Minimum velocity [mm/sec]"), curline, 0);
    g2->addWidget(lEvmin = new QLineEdit("0"), curline, 1);
    g2->addWidget(new QLabel("Minimum travel velocity [mm/sec]"), curline, 2);
    g2->addWidget(lEvminTravel = new QLineEdit("0"), curline, 3);
    connect(lEvmin, SIGNAL(editingFinished()), this, SLOT(changed()));
    connect(lEvminTravel, SIGNAL(editingFinished()), this, SLOT(changed()));

    curline++;
    g2->addWidget(new QLabel("Minimum segment time [usec]"), curline, 0);
    g2->addWidget(lEtminSegment = new QLineEdit("0"), curline, 1);
    connect(lEtminSegment, SIGNAL(editingFinished()), this, SLOT(changed()));

    curline++;
    g2->addWidget(new QLabel("PID"),curline,0);
    g2->addWidget(lEPIDp = new QLineEdit("0"), curline, 1);
    g2->addWidget(lEPIDi = new QLineEdit("0"), curline, 2);
    g2->addWidget(lEPIDd = new QLineEdit("0"), curline, 3);
    connect(lEPIDp, SIGNAL(editingFinished()), this, SLOT(changed()));
    connect(lEPIDi, SIGNAL(editingFinished()), this, SLOT(changed()));
    connect(lEPIDd, SIGNAL(editingFinished()), this, SLOT(changed()));


    g3->addWidget(pbRead = new QPushButton("Read Mem"), curline, 0);
    g3->addWidget(pbWrite = new QPushButton("Write Mem"), curline, 1);
    g3->addWidget(pbStore = new QPushButton("Store EEPROM"), curline, 2);
    g3->addWidget(pbReset = new QPushButton("Read EEPROM "), curline, 3);
    g3->addWidget(pbFactorReset = new QPushButton("Read Factory "), curline, 4);
    pbStore->setStyleSheet("background-color: rgb(255, 0, 0); color: rgb(255, 255, 255)");
    pbReset->setStyleSheet("background-color: rgb(255, 0, 0); color: rgb(255, 255, 255)");
    pbFactorReset->setStyleSheet("background-color: rgb(255, 0, 0); color: rgb(255, 255, 255)");

    curline=0;
    g4->addWidget(new QLabel("Vmax xy:"), curline, 0);
    g4->addWidget(new QLabel("Acc. Distance:"), curline, 1);
    g4->addWidget(lbAccDis = new QLabel("0"), curline, 2);
    g4->addWidget(new QLabel("Acceleration time:"), curline, 3);
    g4->addWidget(lbAccTime = new QLabel("0"), curline, 4);

    curline=1;
    g4->addWidget(new QLabel("100 mm/s:"), curline, 0);
    g4->addWidget(new QLabel("Acc. Distance:"), curline, 1);
    g4->addWidget(lbAccDis100 = new QLabel("0"), curline, 2);
    g4->addWidget(new QLabel("Acceleration time:"), curline, 3);
    g4->addWidget(lbAccTime100 = new QLabel("0"), curline, 4);
    layout->addWidget(w1);
    layout->addWidget(w2);
    layout->addWidget(w4);
    layout->addWidget(w3);
    setLayout(layout);

}


void TabEEPROM::EEPROM_recalculate()
{
    const double amax = lEacceleration->text().toDouble();
    const double vx = lEvmax[0]->text().toDouble();
    const double vy = lEvmax[1]->text().toDouble();
    const double vz = lEvmax[2]->text().toDouble();
    const double vxyjerk = lEvxyJerk->text().toDouble();

    double vmin = vx;
    if(vy < vmin)
        vmin = vy;
    if(vz < vmin)
        vmin = vz;

    double vmax = vx;
    if(vy > vmax)
        vmax = vy;
    if(vz > vmax)
        vmax = vz;

    double t = (vmax - vxyjerk) / amax;
    double adis = amax / 2.0 * t * t;
    lbAccDis->setText(QString("%1 mm").arg(adis));
    lbAccTime->setText(QString("%1 ms").arg(t * 1000));

    t = (vmin - vxyjerk) / amax;
    adis = amax / 2.0 * t * t;
    lbAccDis100->setText(QString("%1 mm").arg(adis));
    lbAccTime100->setText(QString("%1 ms").arg(t * 1000));
}

void TabEEPROM::changed()
{
    EEPROM_recalculate();
}

