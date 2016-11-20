#include <QList>
#include <QTimer>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QTabWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QDebug>
#include "tab_pid.h"
#include "tab_raw.h"
#include "tab_eeprom.h"
#include "tab_veltest.h"
#include "mainwindow.h"

#ifdef USE_QSERIALDEVICE
#include <QtSerialPort/QtSerialPort>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    waitReply(false),
    serialBinBuffer(),
    pidLoaded(false)
{      
    initSerial();

    QVBoxLayout *layout = new QVBoxLayout;

    QHBoxLayout *comLayout = new QHBoxLayout;
    portSelector = new QComboBox(this);
    baudSelector = new QComboBox(this);

    baudSelector->addItem("115200");
    baudSelector->addItem("57600");
    baudSelector->addItem("230400");
    baudSelector->addItem("250000");
    btConnect = new QPushButton("Connect", this);
    btDisconnect = new QPushButton("Disconnect", this);
    btRescan = new QPushButton("Rescan", this);
    comLayout->addWidget(portSelector);
    comLayout->addWidget(baudSelector);
    comLayout->addWidget(btConnect);
    comLayout->addWidget(btDisconnect);
    comLayout->addWidget(btRescan);
    layout->addLayout(comLayout);
    tab = new QTabWidget();
    layout->addWidget(tab);

    tabPID = new TabPID(tab);
    tabRaw = new TabRaw(tab);
    tabEEPROM = new TabEEPROM(tab);
    tabVelTest = new TabVelTest(this, 0 );

    tab->addTab(tabRaw, "Raw");
    tab->addTab(tabPID, "PID");
    tab->addTab(tabEEPROM, "EEPROM");
    tab->addTab(tabVelTest, "VelTest");

    status = new QStatusBar(this);
    layout->addWidget(status);
    setLayout(layout);
    clickedRefresh();

    connect(btConnect, SIGNAL(clicked(bool)), this, SLOT(clickedConnect()));
    connect(btDisconnect, SIGNAL(clicked(bool)), this, SLOT(clickedDisconnect()));
    connect(btRescan, SIGNAL(clicked(bool)), this, SLOT(clickedRefresh()));

    connect(tabRaw->sendText, SIGNAL(returnPressed()), this, SLOT(manualSend()));

    connect(tabPID, SIGNAL(pidChanged()), this, SLOT(sendPID()));
    connect(tabPID->pids[0], SIGNAL(returnPressed()), this, SLOT(sendPID()));
    connect(tabPID->pids[1], SIGNAL(returnPressed()), this, SLOT(sendPID()));
    connect(tabPID->pids[2], SIGNAL(returnPressed()), this, SLOT(sendPID()));
    connect(tabPID->pids[3], SIGNAL(returnPressed()), this, SLOT(sendPID()));
    connect(tabPID->btLoad, SIGNAL(clicked()), this, SLOT(getPID()));

    connect(tabPID->temp[hotend1], SIGNAL(returnPressed()), this, SLOT(setHotend1Temp()));

    connect(this, SIGNAL(newSerialData()), this, SLOT(processReply()));
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), tabVelTest, SLOT(checkDone()));

    connect(tabEEPROM->pbRead,SIGNAL(clicked(bool)),this, SLOT(EEPROM_loadClicked()));
    connect(tabEEPROM->pbWrite,SIGNAL(clicked(bool)),this, SLOT(EEPROM_writeClicked()));
    connect(tabEEPROM->pbReset,SIGNAL(clicked(bool)),this, SLOT(EEPROM_resetClicked()));
    connect(tabEEPROM->pbFactorReset,SIGNAL(clicked(bool)),this, SLOT(EEPROM_factorResetClicked()));
}

MainWindow::~MainWindow()
{
    closeSerial();
}

void MainWindow::initSerial()
{
#ifdef USE_QSERIALDEVICE
    comport = new QSerialPort();
    connect(comport, SIGNAL(readyRead()), this, SLOT(slotRead()));
#endif
}

void MainWindow::closeSerial()
{
#ifdef USE_QSERIALDEVICE
    comport->close();
#endif
}

void MainWindow::openSerial()
{

#ifdef USE_QSERIALDEVICE
    comport->setPortName(portSelector->currentText());

    if (comport->open(QIODevice::ReadWrite))
    {
        qDebug()<<"opened ok"<<endl;
    }
    else
    {
        qDebug()<<"failed opening comport:"<<portSelector->currentText()<<endl;
        return;
    }
    
    const QString baud = baudSelector->currentText();
    if(baud == "115200")
    {
        if (!comport->setBaudRate(QSerialPort::Baud115200))
        {
            qDebug() << "Set baud rate " <<  QSerialPort::Baud115200 << " error.";
            return;
        };
    }
    if(baud == "230400")
    {
        if (!comport->setBaudRate(230400))
        {
            qDebug() << "Set baud rate " <<  230400 << " error.";
            return;
        };
    }
    if(baud == "250000")
    {
        if (!comport->setBaudRate(250000))
        {
            qDebug() << "Set baud rate " <<  250000 << " error.";
            return;
        };
    }
    if(baud == "57600")
    {
        if (!comport->setBaudRate(57600))
        {
            qDebug() << "Set baud rate " <<  57600<< " error.";
            return;
        };
    }
#endif
}

void MainWindow::clickedConnect()
{
    openSerial();

    tabPID->startTime();

    connect(timer, SIGNAL(timeout()), this, SLOT(measure()));
    timer->start(1000);

    serialBinBuffer.clear();
}

void MainWindow::clickedDisconnect()
{
    closeSerial();
}

void MainWindow::clickedRefresh()
{
    QStringList portnames;

#ifdef USE_QSERIALDEVICE
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        portnames << info.portName();
    }
#endif

    portSelector->clear();

    foreach (QString s, portnames)
    {
        portSelector->addItem(s);
    }
}

void MainWindow::getdata(const QString &line, const QString &after, const QString &key, double &target)
{
    const int n = line.indexOf(after);
    if(n == -1)
        return;

    const QString t = line.mid(n, line.size());
    const int m = t.indexOf(key) + key.size();
    if(m == -1)
        return;

    const QString end = t.mid(m, t.size()).split(" ")[0];
    const double f = end.toDouble();
    target = f;
}

void MainWindow::getdata(const QString &line,const QString &after, const QString &key,QLineEdit *target)
{
    const int n = line.indexOf(after);
    if(n == -1)
        return;

    const QString t = line.mid(n, line.size());

    const int m = t.indexOf(key) + key.size();
    if(m == -1)
        return;

    const QString end = t.mid(m, t.size()).split(" ")[0];
    target->setText(end);
    tabEEPROM->EEPROM_recalculate();
}
void MainWindow::slotRead() 
{
    QByteArray ba;

#ifdef USE_QSERIALDEVICE
    ba = comport->readAll();
#endif

    serialBinBuffer.append(ba);
    int lastnewlinepos = serialBinBuffer.lastIndexOf('\n');
    if(lastnewlinepos < 0)
        return; //no newline read yet.

    const QString readlines = QString(serialBinBuffer.mid(0, lastnewlinepos).append(static_cast<char>(0)));
    readSinceLastSend.append(readlines);

    const QStringList lines = readlines.split("\n", QString::SkipEmptyParts);
    serialBinBuffer.remove(0, lastnewlinepos);

    tabRaw->displayText(readlines);

    foreach(QString lineContent, lines)
    {
        if(lineContent.contains("endstop"))
            endStopFound = true;

        if(lineContent.startsWith("ok") || lineContent.startsWith("T"))
        {
            const QStringList junkList(lineContent.remove(0, 3).split(" ", QString::SkipEmptyParts));

            foreach(QString junk, junkList)
            {
                const QStringList ll = junk.split(":", QString::SkipEmptyParts);
                if(ll.size() == 2)
                {
                    bool ok = false;
                    const double f = ll[1].toDouble(&ok);
                    if(ok)
                        variables[ll[0]] = f;

                    if(ll[0] == "p")
                    {
                        tabPID->pids[0]->setText(ll[1]);
                        tabEEPROM->lEPIDp->setText(ll[1]);
                    }
                    if(ll[0] == "i")
                    {
                        tabPID->pids[1]->setText(ll[1]);
                        tabEEPROM->lEPIDi->setText(ll[1]);
                    }
                    if(ll[0] == "d")
                    {
                        tabPID->pids[2]->setText(ll[1]);
                        tabEEPROM->lEPIDd->setText(ll[1]);
                    }
                    if(ll[0] == "c")
                    {
                        tabPID->pids[3]->setText(ll[1]);
                    }
                }
            }
            tabPID->addData(variables["T"], variables["B"], 0, variables["@"]);

        }
        else if(lineContent.startsWith("echo:"))
        {
            getdata(lineContent,"M92","X", tabEEPROM->lEstepsPerUnit[0]);
            getdata(lineContent,"M92","Y", tabEEPROM->lEstepsPerUnit[1]);
            getdata(lineContent,"M92","Z", tabEEPROM->lEstepsPerUnit[2]);
            getdata(lineContent,"M92","E", tabEEPROM->lEstepsPerUnit[3]);

            getdata(lineContent,"M203","X", tabEEPROM->lEvmax[0]);
            getdata(lineContent,"M203","Y", tabEEPROM->lEvmax[1]);
            getdata(lineContent,"M203","Z", tabEEPROM->lEvmax[2]);
            getdata(lineContent,"M203","E", tabEEPROM->lEvmax[3]);

            getdata(lineContent,"M201","X", tabEEPROM->lEamax[0]);
            getdata(lineContent,"M201","Y", tabEEPROM->lEamax[1]);
            getdata(lineContent,"M201","Z", tabEEPROM->lEamax[2]);
            getdata(lineContent,"M201","E", tabEEPROM->lEamax[3]);

            getdata(lineContent,"M204","S", tabEEPROM->lEacceleration);
            getdata(lineContent,"M204","T", tabEEPROM->lEaccelerationRetract);

            getdata(lineContent,"M205","S", tabEEPROM->lEvmin);
            getdata(lineContent,"M205","T", tabEEPROM->lEvminTravel);
            getdata(lineContent,"M205","B", tabEEPROM->lEtminSegment);
            getdata(lineContent,"M205","X", tabEEPROM->lEvxyJerk);
            getdata(lineContent,"M205","Z", tabEEPROM->lEvzJerk);

            getdata(lineContent,"M301","P", tabEEPROM->lEPIDp);
            getdata(lineContent,"M301","I", tabEEPROM->lEPIDi);
            getdata(lineContent,"M301","D", tabEEPROM->lEPIDd);
        }

    }
    emit newSerialData();
}

void MainWindow::manualSend()
{
    send(tabRaw->sendText->text() + "\n");
}

void MainWindow::send(QString text)
{
    tabRaw->displayTextHtml(QString("<br><b><font color=red>Sending:%0</font></b><br>").arg(text));

#ifdef USE_QSERIALDEVICE
    if(comport->isOpen())
    {
        QString text2=text.append("\n");
        QByteArray ba=text2.toLatin1(); //data to send
        comport->write(ba);
    }
#endif
}

void MainWindow::sendGcode(const QString &text)
{
    sendCodes << text;

    if(waitReply)
        return;

    send(sendCodes[0]);
    readSinceLastSend.clear();
    waitReply = true;
}

void MainWindow::processReply()
{
    if(sendCodes.size() == 0)
        return;

    const QStringList lines = readSinceLastSend.split("\n", QString::SkipEmptyParts);
    QString overhang;

    foreach(QString lineContent, lines)
    {
        if(overhang.size())
        {
            overhang.append(lineContent);
        }
        else
        {
            overhang = lineContent;
        }

        if(overhang.startsWith("ok"))
        {
            if(sendCodes.size())
            {
                sendCodes.removeFirst();
                if(sendCodes.size())
                {
                    readSinceLastSend.clear();
                    send(sendCodes[0]);
                }
                else
                {
                    waitReply = false;
                }
            }

        }
        else
        {
            qDebug() << "ack not ok:" << overhang << endl;
        }
        if(lineContent.size() < 2)
        {
            overhang = lineContent;
        }
        else
        {
            overhang.clear();
        }
    }

}

void MainWindow::measure()
{
    if(!pidLoaded)
    {
        getPID();
        pidLoaded = true;
    }
    else
    {
        if(tabPID->monitor->isChecked())
        {
            sendGcode("M105");
        }
    }
}

void MainWindow::setHotend1Temp()
{
    sendGcode(QString("M104 S%1").arg(tabPID->temp[hotend1]->text()));
}

void MainWindow::sendPID()
{
    sendGcode(QString("M301 P%1 I%2 D%3 C%4").arg(tabPID->pids[0]->text()).arg(tabPID->pids[1]->text()).arg(tabPID->pids[2]->text()).arg(tabPID->pids[3]->text()));
}

void MainWindow::getPID()
{
    sendGcode(QString("M301"));
}


void MainWindow::EEPROM_loadClicked()
{
    sendGcode(QString("M503"));
}



void MainWindow::EEPROM_resetClicked()
{
    sendGcode("M501");
    EEPROM_loadClicked();
}


void MainWindow::EEPROM_factorResetClicked()
{
    sendGcode("M502");
    EEPROM_loadClicked();
}

void MainWindow::EEPROM_storeClicked()
{

}

void MainWindow::EEPROM_writeClicked()
{
    /* :  M92 X79.87 Y79.87 Z533.00 E953.00
    echo:Maximum feedrates (mm/s):
    echo:  M203 X200.00 Y200.00 Z19.00 E932.00
    echo:Maximum Acceleration (mm/s2):
    echo:  M201 X10300 Y10500 Z500 E4700
    echo:Acceleration: S=acceleration, T=retract acceleration
    echo:  M204 S4700.00 T7000.00
    echo:Advanced variables: S=Min feedrate (mm/s), T=Min travel feedrate (mm/s), B=minimum segment time (ms), X=maximum xY jerk (mm/s),  Z=maximum Z jerk (mm/s)
    echo:  M205 S0.00 T174.00 B20000 X36.00 Z600.00
    echo:PID settings:
    echo:   M301 P24.00 I1.02 D141.00
    */
    sendGcode(QString("M92 X%1 Y%2 Z%3 E%4").arg(tabEEPROM->lEstepsPerUnit[0]->text()).arg(tabEEPROM->lEstepsPerUnit[1]->text()).arg(tabEEPROM->lEstepsPerUnit[2]->text()).arg(tabEEPROM->lEstepsPerUnit[3]->text()) );
    sendGcode(QString("M203 X%1 Y%2 Z%3 E%4").arg(tabEEPROM->lEvmax[0]->text()).arg(tabEEPROM->lEvmax[1]->text()).arg(tabEEPROM->lEvmax[2]->text()).arg(tabEEPROM->lEvmax[3]->text()) );
    sendGcode(QString("M201 X%1 Y%2 Z%3 E%4").arg(tabEEPROM->lEamax[0]->text()).arg(tabEEPROM->lEamax[1]->text()).arg(tabEEPROM->lEamax[2]->text()).arg(tabEEPROM->lEamax[3]->text()) );
    sendGcode(QString("M204 S%1 T%2").arg(tabEEPROM->lEacceleration->text()).arg(tabEEPROM->lEaccelerationRetract->text()));
    sendGcode(QString("M301 P%1 I%2 D%3").arg(tabEEPROM->lEPIDp->text()).arg(tabEEPROM->lEPIDi->text()).arg(tabEEPROM->lEPIDd->text()));
}
