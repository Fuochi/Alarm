#-------------------------------------------------
#
# Project created by QtCreator 2015-07-18T17:36:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AmbientTemperature
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

#-------------------------------------------------
# This section will include QextSerialPort in
# your project:

include(C:\Users\Filippo\workspace\qextserialport-master\src\qextserialport.pri)

# Before running the project, run qmake first:
# In Qt Creator, right-click on the project
# and choose "run qmake".
# The qextserialport folder should appear in the
# directory tree.
# Now your project is ready to run.
#-------------------------------------------------
