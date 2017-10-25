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

#ifndef PANELSETTINGS_H
#define PANELSETTINGS_H

#include <QSettings>
#include <QStringList>

/*  */
class PanelSettings : public QSettings
{
public:
    enum Position{
            PositionBottom = 0,
            PositionTop    = 1,
            PositionLeft   = 2,
            PositionRight  = 3
        };
    static PanelSettings::Position stringToPosition(QString position);
    static QString positionToString(PanelSettings::Position position);

    PanelSettings();

    void addPanel(QString panel);
    void removePanel(QString panel);
    void setHeight(QString panel, int height);
    void setPosition(QString panel, Position position);
    QString addApplet(QString panel, QString position, QString appletType);
    void removeApplet(QString panel, QString appletId);
    void setAppletPosition(QString panel, QString appletId, QString position);
    void setAppletsList(QString panel, QStringList appletsList);

    QString appletType(QString panel, QString appletId);
    QString appletPosition(QString panel, QString appletId);
    QStringList panelsList();
    QStringList appletsList(QString panel);
    int height(QString panel);
    Position position(QString panel);
};

#endif // PANELSETTINGS_H
