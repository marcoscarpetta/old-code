#ifndef CLOCK_H
#define CLOCK_H

#include "panel/appletinterface.h"
#include "libpanel/applethelper.h"

#include <QSettings>
#include <QLabel>
#include <QCalendarWidget>
#include <QWidgetAction>
#include <QTimer>
#include <QMouseEvent>
#include <QMenu>

class Clock : public QObject,
                public AppletInterface

{
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "Marco.Scarpetta.Clock")
        Q_INTERFACES(AppletInterface)

public:
    Clock();
    ~Clock();

    QSettings *data();
    QWidget *widget();
    QObject *object();
    AppletInterface *create(AppletHelper *helper, QMenu *rightMenu);

    bool click(QMouseEvent *event);
    bool dragClick();
    void deactivate();

signals:
    void deactivated();

private slots:
    void onMenuAboutToHide();
    void onTimeOut();

private:
    QSettings *info;
    AppletHelper *helper;
    QMenu *rightMenu;
    QMenu menu;
    QLabel *clockLabel;
    QCalendarWidget calendar;
    QTimer timer;
    bool deactivatedByPanel;
};

#endif // CLOCK_H
