#ifndef TAB_EEPROM_H
#define TAB_EEPROM_H

#include <QWidget>

class QPushButton;
class QLineEdit;
class QLabel;

class TabEEPROM : public QWidget
{
    Q_OBJECT

public:
    TabEEPROM(QWidget *parent = 0);

    QLineEdit *lEstepsPerUnit[4];
    QLineEdit *lEvmax[4];
    QLineEdit *lEamax[4];
    QLineEdit *lEacceleration;
    QLineEdit *lEaccelerationRetract;
    QLineEdit *lEvxyJerk;
    QLineEdit *lEvzJerk;
    QLineEdit *lEvmin;
    QLineEdit *lEvminTravel;
    QLineEdit *lEtminSegment;
    QLineEdit *lEPIDp;
    QLineEdit *lEPIDd;
    QLineEdit *lEPIDi;

    QPushButton *pbRead;
    QPushButton *pbWrite;
    QPushButton *pbStore;
    QPushButton *pbReset;
    QPushButton *pbFactorReset;

    QLabel *lbAccDis;
    QLabel *lbAccTime;
    QLabel *lbAccDis100;
    QLabel *lbAccTime100;
    QLabel *lbStepRate;

    void EEPROM_recalculate();

public slots:
    void changed();
};

#endif
