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

#ifndef PANELMANAGER_H
#define PANELMANAGER_H

#include <QList>
#include <QSignalMapper>

#include "panel.h"

/*  */
class PanelManager : public QObject
{
    Q_OBJECT

public:
    PanelManager(PanelSettings* settings);
    ~PanelManager();

public slots:
    void showPanelSettingsDialog(QString panel);
    void newPanel();
    void removePanel();
    void panelHeightChanged(int height);
    void panelPositionChanged(int index);

private:
    void createPanel(QString panelName);
    void loadApplets();

    PanelSettings* settings;
    QHash<QString, Panel *> panels;
    PanelSettingsDialog* dialog;
    QSignalMapper* signalMapper;
    QDir appletsDir;
    QObjectList *appletsList;

};

#endif // PANELMANAGER_H
