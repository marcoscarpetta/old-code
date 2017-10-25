#ifndef WEATHER_H
#define WEATHER_H

#include "panel/appletinterface.h"
#include "libpanel/applethelper.h"

#include <QSettings>
#include <QLabel>
#include <QCalendarWidget>
#include <QWidgetAction>
#include <QTimer>
#include <QMouseEvent>
#include <QMenu>
#include <QtNetwork/QNetworkAccessManager>
#include <QDialog>

class Weather : public QObject,
                public AppletInterface

{
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "Marco.Scarpetta.Clock")
        Q_INTERFACES(AppletInterface)

public:
    Weather();
    ~Weather();

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
    void onNetworkReply(QNetworkReply *reply);
    void onSettingsTriggered();

private:
    void updateWeather(QString xml);

    QSettings *info;
    AppletHelper *helper;
    QMenu *rightMenu;
    QMenu menu;
    QLabel *weatherLabel;
    QLabel weatherInfo;
    QTimer timer;
    QNetworkAccessManager* networkManager;
    bool deactivatedByPanel;
};

#endif // WEATHER_H
