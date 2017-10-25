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

#ifndef PANEL_H
#define PANEL_H

#include <QFrame>
#include <QDesktopWidget>
#include <QBoxLayout>
#include <QMouseEvent>
#include <QPluginLoader>
#include <QDir>
#include <QApplication>
#include <QMenu>
#include <QTimer>
#include <QListWidget>
#include <QSignalMapper>

#include "panelsettingsdialog.h"
#include "appletinterface.h"
#include "mouseeventfilter.h"

/*  */
class Panel : public QFrame
{
    Q_OBJECT

public:
    Panel(PanelSettings *settings, QObjectList *appletsList, QString name);
    ~Panel();

    void setPanelHeight(int height);
    void setPanelPosition(PanelSettings::Position position);

public slots:
    void settingsTriggered();

signals:
    void showEvent(QShowEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void settingsActionTriggered(QString);

private slots:
    void onMousePress(QMouseEvent *event);
    void onShowEvent(QShowEvent *event);
    void mousePressed(QObject *obj, QMouseEvent *event);
    void checkEnter();
    void appletDeactivated();
    void onRightMenuActionTriggered(QString info);
    void onMoveAppletTriggered(AppletInterface *applet);
    void onAddAppletTriggered(int insertIndex);
    void onRemoveAppletTriggered(AppletInterface *applet);
    void onAppletMove(QMouseEvent *event);
    void onAppletDrop(QMouseEvent *);
    void onAddAppletDoubleClick(QModelIndex index);

private:
    void showRightClickMenu(QMouseEvent *event, AppletInterface *appletInterface);
    void createApplets();
    QRect availableGeometry();
    void reserveXSpace();
    QMenu *createRightMenu(QString appletId);
    AppletInterface *createApplet(QString appletId);
    void saveConfig();

    PanelSettings *settings;
    QString name;
    QMenu *panelMenu;
    QBoxLayout* panelLayout;
    QSpacerItem *firstSpacer;
    QSpacerItem *secondSpacer;
    PanelSettings::Position position;
    AppletEventFilter filter;
    QObjectList *appletsList;
    QHash<QString, AppletInterface *> applets;
    QTimer timer;
    AppletInterface *activeApplet;
    AppletInterface *movingApplet;
    int prevX;
    QDialog *addAppletDialog;
    QListWidget *appletsListWidget;
    QSignalMapper signalMapper;
};

#endif // PANEL_H
