TEMPLATE      = lib
CONFIG       += plugin
QT           += widgets
INCLUDEPATH  += ../../panel
HEADERS       = appmenu.h
SOURCES       = appmenu.cpp
TARGET        = $$qtLibraryTarget(appmenu)
DESTDIR       = ../build/bin/qpan-applets
LIBS           = -lqpan

RESOURCES = appmenu.qrc
