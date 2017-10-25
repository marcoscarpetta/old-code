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

#include "panelsettings.h"

#include <QDebug>

PanelSettings::Position PanelSettings::stringToPosition(QString position)
{
    if (position == "Top")
        return PanelSettings::PositionTop;
    if (position == "Bottom")
        return PanelSettings::PositionBottom;
    if (position == "Left")
        return PanelSettings::PositionLeft;
    if (position ==  "Right")
        return PanelSettings::PositionRight;
    return PanelSettings::PositionTop;
}

QString PanelSettings::positionToString(PanelSettings::Position position)
{
    if (position == PanelSettings::PositionTop)
        return "Top";
    if (position == PanelSettings::PositionBottom)
        return "Bottom";
    if (position == PanelSettings::PositionLeft)
        return "Left";
    if (position ==  PanelSettings::PositionRight)
        return "Right";
    return "Top";
}

PanelSettings::PanelSettings() :
    QSettings("qpan", "panel")
{
}

void PanelSettings::addPanel(QString panel)
{
    QStringList panelsList = this->value("panels", QStringList()).toStringList();
    panelsList.append(panel);
    this->setValue("panels", panelsList);
}

void PanelSettings::removePanel(QString panel)
{
    QStringList panelsList = this->value("panels", QStringList()).toStringList();
    if (panelsList.contains(panel))
    {
        panelsList.removeAll(panel);
        //FIXME remove other settings
    }
    this->setValue("panels", panelsList);
}

void PanelSettings::setHeight(QString panel, int height)
{
    beginGroup(panel);
    this->setValue("height", height);
    endGroup();
}

void PanelSettings::setPosition(QString panel, Position position)
{
    beginGroup(panel);
    this->setValue("position", PanelSettings::positionToString(position));
    endGroup();
}

QString PanelSettings::addApplet(QString panel, QString position, QString appletType)
{
    beginGroup(panel);
    QStringList appletsList = this->value("applets", QStringList()).toStringList();
    int i=0;
    QString appletId;
    while (appletsList.contains(appletId.setNum(i++)));
    appletsList.append(appletId);
    setValue("applets", appletsList);
    beginGroup(appletId);
    setValue("type", appletType);
    setValue("position", position);
    endGroup();
    endGroup();
    return appletId;
}

void PanelSettings::removeApplet(QString panel, QString appletId)
{
    beginGroup(panel);
    QStringList appletsList = this->value("applets", QStringList()).toStringList();
    if (appletsList.contains(appletId))
        appletsList.removeAll(appletId);
    setValue("applets", appletsList);
    endGroup();
}

void PanelSettings::setAppletPosition(QString panel, QString appletId, QString position)
{
    beginGroup(panel);
    beginGroup(appletId);
    setValue("position", position);
    endGroup();
    endGroup();
}

void PanelSettings::setAppletsList(QString panel, QStringList appletsList)
{
    beginGroup(panel);
    setValue("applets", appletsList);
    endGroup();
}

QString PanelSettings::appletType(QString panel, QString appletId)
{
    beginGroup(panel);
    beginGroup(appletId);
    QString type = value("type").toString();
    endGroup();
    endGroup();
    return type;
}

QString PanelSettings::appletPosition(QString panel, QString appletId)
{
    beginGroup(panel);
    beginGroup(appletId);
    QString position = value("position").toString();
    endGroup();
    endGroup();
    return position;
}

int PanelSettings::height(QString panel)
{
    beginGroup(panel);
    int h;
    h = this->value("height", 30).toInt();
    endGroup();
    return h;
}

PanelSettings::Position PanelSettings::position(QString panel)
{
    beginGroup(panel);
    PanelSettings::Position pos;
    QString val = this->value("position", "Top").toString();
    pos = PanelSettings::stringToPosition(val);
    endGroup();
    return pos;
}

QStringList PanelSettings::panelsList()
{
    return this->value("panels", QStringList()).toStringList();
}

QStringList PanelSettings::appletsList(QString panel)
{
    beginGroup(panel);
    QStringList applets = this->value("applets", QStringList()).toStringList();
    endGroup();
    return applets;
}
