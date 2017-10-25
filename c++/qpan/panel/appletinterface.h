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

#ifndef APPLETINTERFACE_H
#define APPLETINTERFACE_H

#include <QtPlugin>

QT_BEGIN_NAMESPACE
class QSettings;
class QWidget;
class AppletHelper;
class QMouseEvent;
class QMenu;
QT_END_NAMESPACE

/* abstract class */
class AppletInterface
{
public:
    virtual ~AppletInterface() {}

    /*  */
    virtual QSettings *data() = 0;

    /* returns the widget to add to the panel */
    virtual QWidget *widget() = 0;
    virtual QObject *object() = 0;

    /*  */
    virtual AppletInterface *create(AppletHelper *helper, QMenu *rightMenu) = 0;

    /*  */
    virtual bool click(QMouseEvent *) = 0;

    /*  */
    virtual bool dragClick() = 0;

    /*  */
    virtual void deactivate() = 0;

signals:
    /*  */
    virtual void deactivated() = 0;

};

QT_BEGIN_NAMESPACE

#define AppletInterface_iid "Panel.AppletInterface"

Q_DECLARE_INTERFACE(AppletInterface, AppletInterface_iid)

QT_END_NAMESPACE

#endif // APPLETINTERFACE_H
