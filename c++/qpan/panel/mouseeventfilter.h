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

#ifndef MOUSEEVENTFILTER_H
#define MOUSEEVENTFILTER_H

#include <QObject>
#include <QEvent>
#include <QMouseEvent>

/*  */
class AppletEventFilter : public QObject
{
    Q_OBJECT

signals:
    void mousePress(QObject *obj, QMouseEvent *event);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MOUSEEVENTFILTER_H
