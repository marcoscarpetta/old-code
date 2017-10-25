QT += widgets x11extras

INCLUDEPATH  += ..

SOURCES = main.cpp\
    panel.cpp \
    panelsettings.cpp \
    panelsettingsdialog.cpp \
    panelmanager.cpp \
    mouseeventfilter.cpp

HEADERS  = \
    panel.h \
    panelsettings.h \
    panelsettingsdialog.h \
    panelmanager.h \
    appletinterface.h \
    mouseeventfilter.h

LIBS           = -L../build/usr/lib -lqpan -lX11

DESTDIR       = ../build/bin

TARGET        = qpan
