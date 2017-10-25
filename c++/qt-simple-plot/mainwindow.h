/*  Copyright (C) 2015 Marco Scarpetta
 *
 *     This file is part of qt-simple-plot.
 *
 *  qt-simple-plot is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  qt-simple-plot is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with qt-simple-plot.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QGridLayout>
#include <QToolBar>
#include <QComboBox>
#include <QListView>
#include <QLabel>
#include <QFileDialog>
#include <drawingarea.h>
#include <configdialog.h>
#include <plotconfiguredialog.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_measurePointAdded(int series, int pointIndex);

    void on_actionY_zoom_toggled(bool toggled);

    void on_actionX_zoom_toggled(bool toggled);

    void on_actionPan_toggled(bool toggled);

    void on_actionMeasure_toggled(bool toggled);

    void on_seriesCombo_currentIndexChanged(int index);

    void on_deleteButton_clicked();

    void on_configureButton_clicked();

    void on_configDialogAccepted();

    void on_plotConfigureDialogAccepted();

    void on_selectedPointsActivated(QModelIndex index, QModelIndex previous);

    void on_actionConfigure_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_fileDialogAccepted(QString fileName);

    void on_actionExport_triggered();

private:
    //ui
    QGridLayout gridLayout;
    QToolBar toolbar;
    QComboBox seriesCombo;
    QListView selectedPoints;
    QLabel measures;

    QAction* measureAction;
    QAction* panAction;

    QString inputFile;
    QSettings settings;
    DataSeries dataSeries;
    DrawingArea area;
    ConfigDialog configDialog;
    PlotConfigureDialog plotConfigureDialog;
    QFileDialog fileDialog;
    QString savedFile;
    bool exporting;

    QString xLabel;
    QString yLabel;

    void updateMeasures();

    void loadFile(QString fileName);
    void save(QString savingFile);
    bool restore(QString savedFile);
    void exportToFile(QString fileName);
};

#endif // MAINWINDOW_H
