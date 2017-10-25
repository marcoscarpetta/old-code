/* Copyright (C) 2013 Marco Scarpetta
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "panel.h"
#include "libpanel/applethelper.h"

#include <QApplication>
#include <QDebug>
#include <X11/Xlib.h>
#include <QtX11Extras/QX11Info>
#include <X11/Xatom.h>

Panel::Panel(PanelSettings *settings, QObjectList *appletsList, QString name) :
    settings(settings),
    appletsList(appletsList),
    name(name),
    activeApplet(NULL)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_X11NetWmWindowTypeDock, true); //not working!
    setAttribute(Qt::WA_AlwaysShowToolTips, true);
    setAttribute(Qt::WA_AcceptDrops);

    connect(this, SIGNAL(mousePressEvent(QMouseEvent*)), this, SLOT(onMousePress(QMouseEvent*)));
    connect(this, SIGNAL(showEvent(QShowEvent *)), this, SLOT(onShowEvent(QShowEvent*)));
    connect(&filter, SIGNAL(mousePress(QObject*,QMouseEvent*)), this, SLOT(mousePressed(QObject*,QMouseEvent*)));

    panelLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    panelLayout->setContentsMargins(0,0,0,0);
    this->setLayout(panelLayout);

    firstSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    secondSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

    panelMenu = new QMenu(this);
    QAction *action;
    action = panelMenu->addAction("Add Applet...", &signalMapper, SLOT(map()));
    signalMapper.setMapping(action, "add:void");
    panelMenu->addSeparator();
    panelMenu->addAction(tr("Panel Settings"), this, SLOT(settingsTriggered()));
    action = panelMenu->addAction("Exit", &signalMapper, SLOT(map()));
    signalMapper.setMapping(action, "exit:void");

    addAppletDialog = new QDialog(this);
    addAppletDialog->setWindowTitle(tr("Add applet..."));
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight, addAppletDialog);
    addAppletDialog->setLayout(layout);
    appletsListWidget = new QListWidget(addAppletDialog);
    layout->addWidget(appletsListWidget);

    for (int i=0; i<appletsList->length(); i++) {
        QListWidgetItem *item = new QListWidgetItem(qobject_cast<AppletInterface *>(appletsList->at(i))->data()->value("Name").toString());
        item->setData(Qt::UserRole, qobject_cast<AppletInterface *>(appletsList->at(i))->data()->value("IID").toString());
        appletsListWidget->addItem(item);
    }

    connect(appletsListWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onAddAppletDoubleClick(QModelIndex)));
    connect(&signalMapper, SIGNAL(mapped(QString)),
                this, SLOT(onRightMenuActionTriggered(QString)));
    createApplets();

    connect(&timer, SIGNAL(timeout()), this, SLOT(checkEnter()));
}

Panel::~Panel()
{
    QHashIterator<QString, AppletInterface *> iter(applets);
    while (iter.hasNext()) {
        iter.next();
        delete iter.value();
    }
}

/*  */
void Panel::onShowEvent(QShowEvent *event)
{
    reserveXSpace();
    //setAttribute(Qt::WA_X11NetWmWindowTypeDock, true); //not working!
    //Using Xlib
    Atom atom = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE", false);
    Atom dock = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE_DOCK", false);
    XChangeProperty(QX11Info::display(), effectiveWinId(), atom,
                    XA_ATOM, 32, PropModeReplace,  (unsigned char*)&dock, 1);
}

/*  */
void Panel::mousePressed(QObject *obj, QMouseEvent *event)
{
    foreach (AppletInterface *appletInterface, applets)
    {
        if (obj == appletInterface->widget())
        {
            if (appletInterface == activeApplet) {
                activeApplet->deactivate();
                activeApplet = NULL;
            }
            else {
                bool accepted = appletInterface->click(event);
                if (event->button() == Qt::LeftButton && accepted) {
                    activeApplet = appletInterface;
                    timer.start(10);
                }
            }
            break;
        }
    }
}

/*  */
void Panel::checkEnter()
{
    QPoint p = qApp->desktop()->cursor().pos()-pos();
    foreach (AppletInterface *appletInterface, applets) {
        if (appletInterface->widget()==childAt(p) && appletInterface!=activeApplet){
            if (appletInterface->dragClick())
            {
                if (activeApplet)
                    activeApplet->deactivate();
                activeApplet = appletInterface;
            }
            break;
        }
    }
    timer.start(10);
}

/*  */
void Panel::appletDeactivated()
{
    timer.stop();
    activeApplet = NULL;
}

/*  */
void Panel::onMousePress(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        panelMenu->popup(pos()+event->pos());
}

/*  */
void Panel::onRightMenuActionTriggered(QString info)
{
    QStringList actionApplet = info.split(':');
    if (actionApplet.length() == 2)
    {
        if (actionApplet[0] == "exit")
            qApp->quit();
        if (applets.contains(actionApplet[1]))
        {
            if (actionApplet[0] == "move")
                onMoveAppletTriggered(applets.value(actionApplet[1]));
            else if (actionApplet[0] == "add")
                onAddAppletTriggered(panelLayout->indexOf(applets.value(actionApplet[1])->widget()));
            else if (actionApplet[0] == "remove")
                onRemoveAppletTriggered(applets.value(actionApplet[1]));
        }
        else if (actionApplet[1] == "void")
            onAddAppletTriggered(0);
    }
}

/*  */
void Panel::onMoveAppletTriggered(AppletInterface *applet)
{
    movingApplet = applet;
    grabMouse();
    setMouseTracking(true);
    setCursor(Qt::ClosedHandCursor);
    connect(this, SIGNAL(mouseMoveEvent(QMouseEvent*)), this, SLOT(onAppletMove(QMouseEvent*)));
    connect(this, SIGNAL(mousePressEvent(QMouseEvent*)), this, SLOT(onAppletDrop(QMouseEvent*)));
}

/*  */
void Panel::onAddAppletTriggered(int insertIndex)
{
    if (addAppletDialog->exec()) {
        QString appletType = appletsListWidget->currentItem()->data(Qt::UserRole).toString();
        QString appletId = settings->addApplet(name, "left", appletType);
        AppletInterface *newApplet = createApplet(appletId);
        if (newApplet) {
            panelLayout->insertWidget(insertIndex, newApplet->widget());
            saveConfig();
        }
    }
}

/*  */
void Panel::onRemoveAppletTriggered(AppletInterface *applet)
{
    QStringList panelApplets = settings->appletsList(name);
    QString appletId = applets.key(applet);
    panelApplets.removeAll(appletId);
    settings->setAppletsList(name, panelApplets);
    QHashIterator<QString, AppletInterface *> iter(applets);
    while (iter.hasNext()) {
        iter.next();
        if (iter.value() == applet)
            applets.remove(iter.key());
    }
    delete applet;
}

/*  */
void Panel::onAppletMove(QMouseEvent *event)
{
    bool horizontal = position == PanelSettings::PositionTop || position == PanelSettings::PositionBottom;
    QLayoutItem *item = NULL;
    int i;
    for (i=0; i<panelLayout->count(); i++){
        if ((horizontal && panelLayout->itemAt(i)->geometry().x() < event->x() &&
             event->x() < panelLayout->itemAt(i)->geometry().x()+panelLayout->itemAt(i)->geometry().width()) ||
                (!horizontal && panelLayout->itemAt(i)->geometry().y() < event->y() &&
                 event->y() < panelLayout->itemAt(i)->geometry().y()+panelLayout->itemAt(i)->geometry().height())) {
            item = panelLayout->itemAt(i);
            break;
        }}
    if (item)
        if (item->widget() != movingApplet->widget()) {
            if ((horizontal && event->x() > prevX && event->x()-item->geometry().x() > item->geometry().width()/2) ||
                    (!horizontal && event->y() > prevX && event->y()-item->geometry().y() > item->geometry().height()/2)) {
                panelLayout->removeWidget(movingApplet->widget());
                panelLayout->insertWidget(i, movingApplet->widget());
            } else if ((horizontal && event->x() < prevX && event->x()-item->geometry().x() < item->geometry().width()/2) ||
                       (!horizontal && event->y() < prevX && event->y()-item->geometry().y() < item->geometry().height()/2)) {
                panelLayout->removeWidget(movingApplet->widget());
                panelLayout->insertWidget(i, movingApplet->widget());
            }
        }
    prevX = horizontal ? event->x() : event->y();
}

/*  */
void Panel::onAppletDrop(QMouseEvent *)
{
    releaseMouse();
    setMouseTracking(false);
    setCursor(Qt::ArrowCursor);
    disconnect(this, SIGNAL(mouseMoveEvent(QMouseEvent*)), this, SLOT(onAppletMove(QMouseEvent*)));
    disconnect(this, SIGNAL(mousePressEvent(QMouseEvent*)), this, SLOT(onAppletDrop(QMouseEvent*)));
    saveConfig();
}

void Panel::onAddAppletDoubleClick(QModelIndex index)
{
    addAppletDialog->accept();
}

/*  */
void Panel::setPanelHeight(int height)
{
    QRect avGeometry = availableGeometry();
    QRect newGeometry = avGeometry;
    QPoint pos;
    switch (position) {
    case PanelSettings::PositionTop:
        newGeometry.setHeight(height);
        newGeometry.setTopLeft(avGeometry.topLeft());
        break;
    case PanelSettings::PositionBottom:
        newGeometry.setTop(avGeometry.bottom()-height);
        newGeometry.setLeft(avGeometry.left());
        break;
    case PanelSettings::PositionLeft:
        newGeometry.setWidth(height);
        newGeometry.setTopLeft(avGeometry.topLeft());
        break;
    case PanelSettings::PositionRight:
        newGeometry.setTop(avGeometry.top());
        newGeometry.setLeft(avGeometry.right()-height);
        break;
    }
    this->setGeometry(newGeometry);
    this->setFixedSize(newGeometry.width(), newGeometry.height());
    reserveXSpace();
}

/*  */
void Panel::setPanelPosition(PanelSettings::Position position)
{
    this->position = position;
    if (position==PanelSettings::PositionTop || position==PanelSettings::PositionBottom)
    {
        this->panelLayout->setDirection(QBoxLayout::LeftToRight);
    }
    else if (position==PanelSettings::PositionLeft || position==PanelSettings::PositionRight)
    {
        this->panelLayout->setDirection(QBoxLayout::TopToBottom);
    }
}

/*  */
void Panel::settingsTriggered()
{
    emit settingsActionTriggered(name);
}

/*  */
void Panel::createApplets()
{
    int pos = 0;
    foreach (QString appletId, settings->appletsList(name)) {
        if (pos == 0 && settings->appletPosition(name, appletId) == "center") {
            pos=1;
            panelLayout->addSpacerItem(firstSpacer);
        }
        if (pos < 2 && settings->appletPosition(name, appletId) == "right") {
            if (pos == 0)
                panelLayout->addSpacerItem(firstSpacer);
            panelLayout->addSpacerItem(secondSpacer);
            pos=2;
        }
        AppletInterface *applet = createApplet(appletId);
        if (applet)
            panelLayout->addWidget(applet->widget());
    }
    if (pos == 0) {
        panelLayout->addSpacerItem(firstSpacer);
        panelLayout->addSpacerItem(secondSpacer);
    }

    if (pos == 1)
        panelLayout->addSpacerItem(secondSpacer);
}

/*  */
QRect Panel::availableGeometry()
{
    QRect geometry = qApp->desktop()->availableGeometry();

    unsigned char* strut = NULL;
    Atom atom = XInternAtom(QX11Info::display(), "_NET_WM_STRUT_PARTIAL", false);

    int  format;
    unsigned long type, rest, length;
    int r = XGetWindowProperty(QX11Info::display(), effectiveWinId(), atom, 0, 4096, false,
                                  XA_ATOM, &type, &format, &length, &rest, (unsigned char**)&strut);
    if (r == Success) {
        if (strut[0] != 0)
            geometry.setLeft(geometry.left()-strut[5]);
        else if (strut[1] != 0)
            geometry.setWidth(geometry.width()+strut[7]);
        else if (strut[2] != 0)
            geometry.setTop(geometry.top()-strut[9]);
        else if (strut[3] != 0)
            geometry.setHeight(geometry.height()+strut[11]);
    }

    return geometry;
}

/*  */
void Panel::reserveXSpace()
{
    unsigned long strut[12];

    for (int i=0; i<12; i++)
        strut[i] = 0;

    QRect geometry = qApp->desktop()->geometry();
    switch (position) {
    case PanelSettings::PositionLeft:
        strut[0] = this->width();
        strut[5] = geometry.height();
        break;
    case PanelSettings::PositionRight:
        strut[1] = this->width();
        strut[7] = geometry.height();
        break;
    case PanelSettings::PositionTop:
        strut[2] = this->height();
        strut[9] = geometry.width();
        break;
    case PanelSettings::PositionBottom:
        strut[3] = this->height();
        strut[11] = geometry.width();
        break;
    }

    Atom atom = XInternAtom(QX11Info::display(), "_NET_WM_STRUT_PARTIAL", false);

    XChangeProperty(QX11Info::display(), effectiveWinId(), atom,
                    XA_CARDINAL, 32, PropModeReplace,  (unsigned char*) strut, 12);

    atom = XInternAtom(QX11Info::display(), "_NET_WM_STRUT", false);

    XChangeProperty(QX11Info::display(), effectiveWinId(), atom,
                    XA_CARDINAL, 32, PropModeReplace, (unsigned char*) strut, 4);
}

/*  */
QMenu *Panel::createRightMenu(QString appletId)
{
    QMenu *rightMenu = new QMenu(this);
    QAction *action;
    action = rightMenu->addAction("Move", &signalMapper, SLOT(map()));
    signalMapper.setMapping(action, "move:"+appletId);
    action = rightMenu->addAction("Add Applet...", &signalMapper, SLOT(map()));
    signalMapper.setMapping(action, "add:"+appletId);
    action = rightMenu->addAction("Remove Applet", &signalMapper, SLOT(map()));
    signalMapper.setMapping(action, "remove:"+appletId);
    rightMenu->addSeparator();
    rightMenu->addAction(tr("Panel Settings"), this, SLOT(settingsTriggered()));
    action = rightMenu->addAction("Exit", &signalMapper, SLOT(map()));
    signalMapper.setMapping(action, "exit:"+appletId);
    return rightMenu;
}

/*  */
AppletInterface *Panel::createApplet(QString appletId)
{
    QString type = settings->appletType(name, appletId);
    for (int i=0; i<appletsList->length(); i++) {
        AppletInterface *factory = qobject_cast<AppletInterface *>(appletsList->at(i));
        if (factory->data()->value("IID").toString() == type) {
            QMenu *rightMenu = createRightMenu(appletId);
            AppletHelper *helper = new AppletHelper(settings, this, name, appletId);
            AppletInterface *appletInterface = factory->create(helper, rightMenu);
            applets.insert(appletId, appletInterface);
            appletInterface->widget()->installEventFilter(&filter);
            connect(appletInterface->object(), SIGNAL(deactivated()), this, SLOT(appletDeactivated()));
            return appletInterface;
        }
    }
    return NULL;
}

/*  */
void Panel::saveConfig()
{
    QStringList appletsConfig;
    QString appletPos = "left";
    for (int i=0; i<panelLayout->count(); i++) {
        if (panelLayout->itemAt(i) == firstSpacer)
            appletPos = "center";
        else if (panelLayout->itemAt(i) == secondSpacer)
            appletPos = "right";
        else {
            QHashIterator<QString, AppletInterface *> iter(applets);
            while (iter.hasNext()) {
                iter.next();
                if (iter.value()->widget() == panelLayout->itemAt(i)->widget()) break;
            }
            appletsConfig.append(iter.key());
            settings->setAppletPosition(name, iter.key(), appletPos);
        }
    }
    settings->setAppletsList(name, appletsConfig);
}
