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

#ifndef PANELSETTINGSDIALOG_H
#define PANELSETTINGSDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QTabWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QListWidget>
#include <QObjectList>

#include "panelsettings.h"
#include "appletinterface.h"

/*  */
class PanelSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PanelSettingsDialog(PanelSettings *settings, QWidget *parent = 0);

    QComboBox *panelsComboBox;
    QPushButton *addPanelButton;
    QPushButton *removePanelButton;
    QSpinBox *panelHeight;
    QComboBox *panelPosition;
    QString panelName;
    
signals:
    
public slots:
    void setPanelName(QString panel);

private:
    QTabWidget *tabWidget;
    PanelSettings *settings;
};

#endif // PANELSETTINGSDIALOG_H
