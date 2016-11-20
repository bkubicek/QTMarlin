TEMPLATE = app
TARGET = QTMarlin

QT += widgets
QT += serialport

CONFIG += silent

HEADERS += mainwindow.h tab_pid.h tab_raw.h tab_eeprom.h tab_veltest.h
SOURCES += main.cpp mainwindow.cpp tab_pid.cpp  tab_raw.cpp tab_eeprom.cpp tab_veltest.cpp

# QWT library: Adapt path to your QWT installation path:
include ( /usr/local/qwt-6.1.3/features/qwt.prf )
