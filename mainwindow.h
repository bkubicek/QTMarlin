#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QMap>

class QStatusBar;
class QTabWidget;
class QComboBox;
class QPushButton;
class QLineEdit;
class QTimer;
class QSerialPort;
class TabPID;
class TabRaw;
class TabEEPROM;
class TabVelTest;

#define USE_QSERIALDEVICE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void send(QString text);
    void sendGcode(const QString &text); //wait for "ok"

    QStringList sendCodes;
    QMap<QString, double> variables;
    bool waitReply;
    bool endStopFound;

public slots:
    void clickedConnect();
    void clickedDisconnect();
    void clickedRefresh();
    void slotRead();
    void manualSend();
    void measure();
    void setHotend1Temp();

    void sendPID();
    void getPID();

    void processReply();

    void EEPROM_loadClicked();
    void EEPROM_writeClicked();
    void EEPROM_storeClicked();
    void EEPROM_resetClicked();
    void EEPROM_factorResetClicked();

signals:
    void newSerialData();

private: 
    QStatusBar *status;
    QTabWidget *tab;
    TabPID *tabPID;
    TabRaw *tabRaw;
    TabEEPROM *tabEEPROM;
    TabVelTest *tabVelTest;

    QComboBox *portSelector;
    QComboBox *baudSelector;
    QPushButton *btConnect;
    QPushButton *btDisconnect;
    QPushButton *btRescan;

#ifdef USE_QSERIALDEVICE
    QSerialPort *comport;
#endif
    QTimer *timer;

private:
    void initSerial();
    void openSerial();
    void closeSerial();

    void getdata(const QString &line, const QString &after, const QString &key, double &target);
    void getdata(const QString &line,const QString &after, const QString &key,QLineEdit *target);

    QByteArray serialBinBuffer;
    QString readSinceLastSend;
    bool pidLoaded;
};

#endif
