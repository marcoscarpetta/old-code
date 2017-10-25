#ifndef APPLETHELPER_H
#define APPLETHELPER_H

#include <QPoint>
#include <QFrame>

#include "panel/panelsettings.h"

/* contains some useful functions for applets */
class AppletHelper
{
public:
    AppletHelper(PanelSettings *settings, QFrame *panel, QString panelName, QString appletId);

    QPoint getPopupPoint(QPoint clickPoint, QSize menuSize);
    void setValue(QString key, QVariant value);
    QVariant value(QString key, QVariant defaultValue = QVariant());

private:
    PanelSettings *settings;
    QFrame *panel;
    QString panelName;
    QString appletId;
};

#endif // APPLETHELPER_H
