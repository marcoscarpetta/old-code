#-------------------------------------------------
#
# Project created by QtCreator 2015-04-03T19:18:05
#
#-------------------------------------------------

QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt-simple-plot
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dataseries.cpp \
    dataarray.cpp \
    drawingarea.cpp \
    configdialog.cpp \
    plotconfiguredialog.cpp

HEADERS  += mainwindow.h \
    dataseries.h \
    dataarray.h \
    drawingarea.h \
    configdialog.h \
    plotconfiguredialog.h

FORMS    += configdialog.ui \
    plotconfiguredialog.ui
