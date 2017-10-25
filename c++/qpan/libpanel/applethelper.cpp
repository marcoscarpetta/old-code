#include "applethelper.h"
#include <QDebug>

AppletHelper::AppletHelper(PanelSettings *settings, QFrame *panel, QString panelName, QString appletId) :
    settings(settings),
    panel(panel),
    panelName(panelName),
    appletId(appletId)
{
}

/* returns the point where to popup the applet menu */
QPoint AppletHelper::getPopupPoint(QPoint clickPoint, QSize menuSize)
{
    if (settings->position(panelName) == PanelSettings::PositionTop)
        clickPoint.setY(panel->geometry().bottom());
    if (settings->position(panelName) == PanelSettings::PositionLeft)
        clickPoint.setX(panel->geometry().right());
    if (settings->position(panelName) == PanelSettings::PositionBottom)
        clickPoint.setY(panel->geometry().top()-menuSize.height());
    if (settings->position(panelName) == PanelSettings::PositionRight)
        clickPoint.setX(panel->geometry().left()-menuSize.width());
    return clickPoint;
}

/* saves the given value into tha application settings */
void AppletHelper::setValue(QString key, QVariant value)
{
    settings->beginGroup(panelName);
    settings->beginGroup(appletId);
    settings->setValue(key, value);
    settings->endGroup();
    settings->endGroup();
}

/* returns the value associated with the given key*/
QVariant AppletHelper::value(QString key, QVariant defaultValue)
{
    settings->beginGroup(panelName);
    settings->beginGroup(appletId);
    QVariant value = settings->value(key, defaultValue);
    settings->endGroup();
    settings->endGroup();
    return value;
}
