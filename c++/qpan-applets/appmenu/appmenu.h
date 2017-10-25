#ifndef APPMENU_H
#define APPMENU_H

#include "panel/appletinterface.h"
#include "libpanel/applethelper.h"

#include <QSettings>
#include <QLabel>
#include <QObject>
#include <QMenu>
#include <QString>
#include <QAction>
#include <QPoint>
#include <QMouseEvent>
#include <QPushButton>
#include <QSignalMapper>

class AppMenu : public QObject,
                public AppletInterface

{
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "Marco.Scarpetta.AppMenu")
        Q_INTERFACES(AppletInterface)

public:
    AppMenu();
    ~AppMenu();

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
    void onMenuActionTriggered(QString command);

private:
    QSettings *info;
    AppletHelper *helper;
    QMenu *menu;
    QMenu *rMenu;
    QPushButton *menuButton;
    QSignalMapper *signalMapper;
    bool deactivatedByPanel;

    void loadMenu();
};
#endif // APPMENU_H
