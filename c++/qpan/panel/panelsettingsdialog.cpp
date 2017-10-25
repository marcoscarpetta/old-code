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

#include "panelsettingsdialog.h"

#include <QDebug>

PanelSettingsDialog::PanelSettingsDialog(PanelSettings *settings, QWidget *parent) :
    QDialog(parent)
{
    this->settings = settings;

    QGridLayout *mainGridLayout = new QGridLayout(this);

    panelsComboBox = new QComboBox(this);
    removePanelButton = new QPushButton(tr("Remove panel"), this);
    addPanelButton = new QPushButton(tr("New panel"), this);
    tabWidget = new QTabWidget(this);
    panelHeight = new QSpinBox(this);
    panelPosition = new QComboBox(this);

    QWidget *panelSettings = new QWidget(tabWidget);
    tabWidget->addTab(panelSettings, "Panel settings");

    panelsComboBox->addItems(settings->panelsList());
    panelPosition->addItem(tr("Top"), PanelSettings::PositionTop);
    panelPosition->addItem(tr("Bottom"), PanelSettings::PositionBottom);
    panelPosition->addItem(tr("Left"), PanelSettings::PositionLeft);
    panelPosition->addItem(tr("Right"), PanelSettings::PositionRight);

    mainGridLayout->addWidget(panelsComboBox, 0, 0, 1, 1);
    mainGridLayout->addWidget(removePanelButton, 0, 1, 1, 1);
    mainGridLayout->addWidget(addPanelButton, 0, 2, 1, 1);
    mainGridLayout->addWidget(tabWidget, 1, 0, 1, 3);

    QGridLayout *settingsGrid = new QGridLayout(panelSettings);
    settingsGrid->addWidget(new QLabel(tr("Height:"), this), 0, 0, 1, 1);
    settingsGrid->addWidget(panelHeight, 0, 1, 1, 1);
    settingsGrid->addWidget(new QLabel(tr("Position:"), this), 1, 0, 1, 1);
    settingsGrid->addWidget(panelPosition, 1, 1, 1, 1);

    panelSettings->setLayout(settingsGrid);    

    setWindowTitle(tr("Panels Settings"));

    connect(panelsComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(setPanelName(QString)));
}

/*  */
void PanelSettingsDialog::setPanelName(QString panel)
{
    panelName = panel;
    panelsComboBox->setCurrentText(panel);
    panelHeight->setValue(settings->height(panel));
    panelPosition->setCurrentIndex(panelPosition->findData(settings->position(panel)));
}
