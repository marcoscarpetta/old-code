TEMPLATE      = lib
CONFIG       += plugin
QT           += widgets
INCLUDEPATH  += ../../panel
HEADERS       = clock.h
SOURCES       = clock.cpp
TARGET        = $$qtLibraryTarget(clock)
DESTDIR       = ../build/bin/qpan-applets
LIBS           = -lqpan

RESOURCES = clock.qrc \
    clock.qrc
