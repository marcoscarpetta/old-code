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

#include "panelmanager.h"

#include <QDebug>

PanelManager::PanelManager(PanelSettings *settings) :
    settings(settings)
{
    signalMapper = new QSignalMapper();

    appletsDir = QDir(qApp->applicationDirPath());
    appletsDir.cd("qpan-applets");

    appletsList = new QObjectList();
    loadApplets();

    dialog = new PanelSettingsDialog(settings);

    QStringList panelsStringList = settings->panelsList();
    if (panelsStringList.isEmpty())
    {
        panelsStringList.append("0");
        settings->setHeight("0", 30);
        settings->setPosition("0", PanelSettings::PositionTop);
        settings->addPanel("0");
        dialog->panelsComboBox->addItem("0");
    }
    foreach (QString panelName, panelsStringList) {
        createPanel(panelName);
    }

    connect(dialog->addPanelButton, SIGNAL(clicked()), this, SLOT(newPanel()));
    connect(dialog->removePanelButton, SIGNAL(clicked()), this, SLOT(removePanel()));
    connect(dialog->panelHeight, SIGNAL(valueChanged(int)), this, SLOT(panelHeightChanged(int)));
    connect(dialog->panelPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(panelPositionChanged(int)));
}

PanelManager::~PanelManager()
{
    QHashIterator<QString, Panel *> iter(panels);
    while (iter.hasNext()) {
        iter.next();
        delete iter.value();
    }
    delete dialog;
    delete signalMapper;
    for (int i=0; i<appletsList->length(); i++)
        delete appletsList->at(i);
    delete appletsList;
}

/*  */
void PanelManager::createPanel(QString panelName)
{
    Panel *panel = new Panel(settings, appletsList, panelName);
    panels.insert(panelName, panel);

    connect(panel, SIGNAL(settingsActionTriggered(QString)), this, SLOT(showPanelSettingsDialog(QString)));

    panel->setPanelPosition(settings->position(panelName));
    panel->setPanelHeight(settings->height(panelName));
    panel->show();
}

/*  */
void PanelManager::showPanelSettingsDialog(QString panel)
{
    dialog->setPanelName(panel);
    dialog->show();
}

/*  */
void PanelManager::removePanel()
{
    QString panelName = dialog->panelsComboBox->currentText();
    dialog->panelsComboBox->removeItem(dialog->panelsComboBox->findText(panelName));
    settings->removePanel(panelName);
    delete panels.take(panelName);
}

/*  */
void PanelManager::newPanel()
{
    QStringList panelsList = settings->panelsList();
    int i=0;
    QString panelName;
    while (panelsList.contains(panelName.setNum(i++)));
    settings->addPanel(panelName);
    createPanel(panelName);
    dialog->panelsComboBox->addItem(panelName);
    dialog->setPanelName(panelName);
}

/*  */
void PanelManager::panelHeightChanged(int height)
{
    QString panelName = dialog->panelName;
    Panel *panel = panels.value(panelName);
    if (panel)
    {
        settings->setHeight(panelName, height);
        panel->setPanelHeight(height);
    }
}

/*  */
void PanelManager::panelPositionChanged(int index)
{
    QString panelName = dialog->panelName;
    Panel *panel = panels.value(panelName);
    if (panel)
    {
        PanelSettings::Position pos = (PanelSettings::Position)dialog->panelPosition->itemData(index).toInt();
        settings->setPosition(panelName, pos);
        panel->setPanelPosition(pos);
        panel->setPanelHeight(dialog->panelHeight->value());
    }
}

/*  */
void PanelManager::loadApplets()
{
    foreach (QObject *plugin, QPluginLoader::staticInstances())
        appletsList->append(plugin);

    foreach (QString fileName, appletsDir.entryList(QDir::Files)) {
        QString file = appletsDir.absoluteFilePath(fileName);
        QPluginLoader loader(file);
        QObject *plugin = loader.instance();
        if (plugin) {
            appletsList->append(plugin);
        }
    }
}
