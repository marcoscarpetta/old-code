TEMPLATE      = lib
CONFIG       += plugin
QT           += widgets network
INCLUDEPATH  += ../../panel
HEADERS       = weather.h
SOURCES       = weather.cpp
TARGET        = $$qtLibraryTarget(weather)
DESTDIR       = ../build/bin/qpan-applets
LIBS           = -lqpan

RESOURCES = weather.qrc 
