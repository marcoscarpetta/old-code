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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QXmlStreamWriter>
#include <QtSvg/QSvgGenerator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    //setup ui

    setCentralWidget(new QWidget(this));
    centralWidget()->setLayout(&gridLayout);
    gridLayout.addWidget(&toolbar, 0, 0, 1, 3);
    gridLayout.addWidget(&area, 1, 0, 4, 1);
    gridLayout.addWidget(&seriesCombo, 1, 1, 1, 1);
    gridLayout.addWidget(&selectedPoints, 3, 1, 1, 2);
    gridLayout.addWidget(&measures, 4, 1, 1, 2);

    toolbar.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    measures.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    area.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QPushButton* button = new QPushButton(QIcon::fromTheme("configure"), "Configure", this);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(on_configureButton_clicked()));
    gridLayout.addWidget(button, 1, 2, 1, 1);

    button = new QPushButton(QIcon::fromTheme("edit-delete"), "Delete", this);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(on_deleteButton_clicked()));
    gridLayout.addWidget(button, 2, 2, 1, 1);

    //toolbar actions
    QAction* action = new QAction(QIcon::fromTheme("zoom-in-x"), "Zoom X", this);
    action->setCheckable(true);
    action->setChecked(settings.value("zoomX", true).toBool());
    connect(action, SIGNAL(toggled(bool)), this, SLOT(on_actionX_zoom_toggled(bool)));
    toolbar.addAction(action);

    action = new QAction(QIcon::fromTheme("zoom-in-y"), "Zoom Y", this);
    action->setCheckable(true);
    action->setChecked(settings.value("zoomY", true).toBool());
    connect(action, SIGNAL(toggled(bool)), this, SLOT(on_actionY_zoom_toggled(bool)));
    toolbar.addAction(action);

    panAction = new QAction(QIcon::fromTheme("transform-move"), "Pan", this);
    panAction->setCheckable(true);
    panAction->setChecked(settings.value("pan", true).toBool());
    connect(panAction, SIGNAL(toggled(bool)), this, SLOT(on_actionPan_toggled(bool)));
    toolbar.addAction(panAction);

    measureAction = new QAction(QIcon::fromTheme("measure"), "Measure", this);
    measureAction->setCheckable(true);
    measureAction->setChecked(settings.value("measure", true).toBool());
    on_actionMeasure_toggled(measureAction->isChecked());
    connect(measureAction, SIGNAL(toggled(bool)), this, SLOT(on_actionMeasure_toggled(bool)));
    toolbar.addAction(measureAction);

    toolbar.addSeparator();

    action = new QAction(QIcon::fromTheme("configure"), "Configure", this);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(on_actionConfigure_triggered()));
    toolbar.addAction(action);

    toolbar.addSeparator();

    action = new QAction(QIcon::fromTheme("document-open"), "Open", this);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(on_actionOpen_triggered()));
    toolbar.addAction(action);

    action = new QAction(QIcon::fromTheme("document-save"), "Save", this);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(on_actionSave_triggered()));
    toolbar.addAction(action);

    action = new QAction(QIcon::fromTheme("document-export"), "Export", this);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(on_actionExport_triggered()));
    toolbar.addAction(action);

    connect(&area, SIGNAL(measurePointAdded(int,int)), this, SLOT(on_measurePointAdded(int,int)));
    connect(&configDialog, SIGNAL(accepted()), this, SLOT(on_configDialogAccepted()));
    connect(&plotConfigureDialog, SIGNAL(accepted()),
            this, SLOT(on_plotConfigureDialogAccepted()));
    connect(&fileDialog, SIGNAL(fileSelected(QString)), this, SLOT(on_fileDialogAccepted(QString)));
    connect(&seriesCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(on_seriesCombo_currentIndexChanged(int)));
    //ui completed

    xLabel = "x axis";
    yLabel = "y axis";

    //area.setFont(QFont("sans", 16));
    area.setXAxisLabel(xLabel);
    area.setYAxisLabel(yLabel);

    if (qApp->arguments().size() > 1)
    {
        loadFile(qApp->arguments().at(1));
    }
    else
        area.setDataSeries(0);

    resize(1024,500);

    exporting = false;
}

MainWindow::~MainWindow()
{
    //delete ui;
}

void MainWindow::loadFile(QString fileName)
{
    while (seriesCombo.count())
        seriesCombo.removeItem(0);

    savedFile = fileName;
    if (!restore(fileName))
    {
        savedFile = "";
        inputFile = fileName;
        dataSeries.loadFromFile(inputFile);

        for (int i=0; i<dataSeries.size(); i++)
        {
            QPixmap pixmap(20, 20);
            pixmap.fill(dataSeries[i]->color);
            QIcon icon(pixmap);
            seriesCombo.addItem(icon, dataSeries[i]->name, i);
        }
        area.setDataSeries(&dataSeries);
    }
}

void MainWindow::on_measurePointAdded(int series, int pointIndex)
{
    if (dataSeries[series]->selectedPoints.rowCount() < 2)
    {
        QStandardItem* point = new QStandardItem();
        point->setData(pointIndex);
        point->setText(QString::number(dataSeries[series]->at(pointIndex).x()) + QString(", ") +
                       QString::number(dataSeries[series]->at(pointIndex).y()));
        dataSeries[series]->selectedPoints.appendRow(point);
        area.update();
    }
    updateMeasures();
}

void MainWindow::on_actionY_zoom_toggled(bool toggled)
{
    settings.setValue("zoomY", toggled);
}

void MainWindow::on_actionX_zoom_toggled(bool toggled)
{
    settings.setValue("zoomX", toggled);
}

void MainWindow::on_actionPan_toggled(bool toggled)
{
    if (toggled)
    {
        measureAction->setChecked(false);
        area.setSelectedPoint(-1);
        area.update();
    }
    settings.setValue("pan", toggled);

    area.setCursor(QCursor(Qt::OpenHandCursor));
}

void MainWindow::on_actionMeasure_toggled(bool toggled)
{
    if (toggled)
        panAction->setChecked(false);
    settings.setValue("measure", toggled);

    area.setMouseTracking(toggled);
    area.setActiveSeries(seriesCombo.currentIndex());

    area.setCursor(QCursor(Qt::ArrowCursor));
}

void MainWindow::on_seriesCombo_currentIndexChanged(int index)
{
    if (index > -1)
    {
        area.setActiveSeries(index);
        selectedPoints.setModel(&dataSeries[index]->selectedPoints);

        connect(selectedPoints.selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                this, SLOT(on_selectedPointsActivated(QModelIndex,QModelIndex)));

        updateMeasures();
    }
}

void MainWindow::on_deleteButton_clicked()
{
    foreach (QModelIndex index, selectedPoints.selectionModel()->selectedIndexes())
    {
        selectedPoints.model()->removeRow(index.row());
    }
    updateMeasures();
    area.setSelectedPoint(-1);
    area.update();
}

void MainWindow::updateMeasures()
{
    if (dataSeries[seriesCombo.currentIndex()]->selectedPoints.rowCount() == 2)
    {
        DataArray* array = dataSeries[seriesCombo.currentIndex()];
        QPointF p1, p2;
        int i1 = array->selectedPoints.item(0,0)->data().toInt(),
                i2 = array->selectedPoints.item(1,0)->data().toInt();

        if (i1 > i2)
        {
            int tmp = i1;
            i1 = i2;
            i2 = tmp;
        }

        p1 = array->at(i1);
        p2 = array->at(i2);

        qreal min, max;
        array->yMinMax(min, max, i1, i2);

        QString text("dy/dx:\n");
        text += QString::number((p2.y()-p1.y())/(p2.x()-p1.x()));
        text += QString("\nMin:\n") += QString::number(min);
        text += QString("\nMax:\n") += QString::number(max);
        text += QString("\nPeak to peak:\n") += QString::number(max-min);
        measures.setText(text);
    }
    else
        measures.setText("");
}

void MainWindow::on_configureButton_clicked()
{
    DataArray* array = dataSeries[seriesCombo.currentIndex()];
    configDialog.setColor(array->color);
    configDialog.setName(array->name);
    configDialog.show();
}

void MainWindow::on_configDialogAccepted()
{
    int index = seriesCombo.currentIndex();
    DataArray* array = dataSeries[index];

    array->name = configDialog.name();
    array->color = configDialog.color();

    seriesCombo.setItemText(index, array->name);

    QPixmap pixmap(20, 20);
    pixmap.fill(array->color);
    QIcon icon(pixmap);
    seriesCombo.setItemIcon(index, icon);
}

void MainWindow::on_selectedPointsActivated(QModelIndex index, QModelIndex previous)
{
    if (index.row() > -1) {
        DataArray* array = dataSeries[seriesCombo.currentIndex()];
        area.setSelectedPoint(array->selectedPoints.item(index.row(), 0)->data().toInt());
        int a = array->selectedPoints.item(index.row(), 0)->data().toInt();
        area.update();
    }
}

void MainWindow::on_actionConfigure_triggered()
{
    plotConfigureDialog.setXLabel(xLabel);
    plotConfigureDialog.setYLabel(yLabel);
    plotConfigureDialog.show();
}

void MainWindow::on_plotConfigureDialogAccepted()
{
    xLabel = plotConfigureDialog.xLabel();
    yLabel = plotConfigureDialog.yLabel();
    area.setXAxisLabel(xLabel);
    area.setYAxisLabel(yLabel);
}

void MainWindow::save(QString savingFile)
{
    QFile file(savingFile);
    file.open(QFile::WriteOnly);

    QJsonObject mainObject;

    mainObject["version"] = "1.0";

    mainObject["inputFile"] = inputFile;
    mainObject["xLabel"] = xLabel;
    mainObject["yLabel"] = yLabel;

    qreal xMin, xMax, yMin, yMax;
    area.getView(xMin, xMax, yMin, yMax);
    mainObject["xMin"] = xMin;
    mainObject["xMax"] = xMax;
    mainObject["yMin"] = yMin;
    mainObject["yMax"] = yMax;

    QJsonArray series;

    for (int i=0; i<dataSeries.size(); i++)
    {
        QJsonObject dataArray;
        dataArray["index"] = i;
        dataArray["name"] = dataSeries[i]->name;
        dataArray["color"] = dataSeries[i]->color.name();

        QJsonArray selectedPoints;
        for (int j=0; j<dataSeries[i]->selectedPoints.rowCount(); j++)
        {
            selectedPoints.append(dataSeries[i]->selectedPoints.item(j, 0)->data().toInt());
        }
        dataArray["selectedPoints"] = selectedPoints;
        series.append(dataArray);
    }
    mainObject["dataSeries"] = series;

    QJsonDocument doc(mainObject);

    file.write(doc.toJson());
}

bool MainWindow::restore(QString savedFile)
{
    QFile file(savedFile);
    file.open(QFile::ReadOnly);

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());

    if (doc.isNull()) return false;

    QJsonObject mainObject = doc.object();

    inputFile = mainObject["inputFile"].toString();
    dataSeries.loadFromFile(inputFile);
    area.setDataSeries(&dataSeries);
    for (int i=0; i<dataSeries.size(); i++)
    {
        QPixmap pixmap(20, 20);
        pixmap.fill(dataSeries[i]->color);
        QIcon icon(pixmap);
        seriesCombo.addItem(icon, dataSeries[i]->name, i);
    }

    xLabel = mainObject["xLabel"].toString();
    yLabel = mainObject["yLabel"].toString();
    area.setXAxisLabel(xLabel);
    area.setYAxisLabel(yLabel);

    qreal xMin, xMax, yMin, yMax;

    xMin = mainObject["xMin"].toDouble();
    xMax = mainObject["xMax"].toDouble();
    yMin = mainObject["yMin"].toDouble();
    yMax = mainObject["yMax"].toDouble();

    area.defineView(xMin, xMax, yMin, yMax);

    QJsonArray series = mainObject["dataSeries"].toArray();
    for (int i=0; i<series.size(); i++)
    {
        QJsonObject dataArray = series[i].toObject();
        int index = dataArray["index"].toInt();
        if (index < dataSeries.size())
        {
            dataSeries[index]->name = dataArray["name"].toString();
            QColor color(dataArray["color"].toString());
            dataSeries[index]->color = color;

            QPixmap pixmap(20, 20);
            pixmap.fill(color);
            QIcon icon(pixmap);
            seriesCombo.setItemText(index, dataSeries[index]->name);
            seriesCombo.setItemIcon(index, icon);

            QJsonArray selectedPoints = dataArray["selectedPoints"].toArray();
            for (int j=0; j<selectedPoints.size(); j++)
            {
                on_measurePointAdded(index, selectedPoints[j].toInt());
            }
            dataArray["selectedPoints"] = selectedPoints;
        }
    }

    area.update();
    return true;
}

void MainWindow::exportToFile(QString fileName)
{
    QFont areaFont(area.fontInfo().family(), area.fontInfo().pixelSize());
    QFontMetrics fm(areaFont);

    int height = area.height();

    foreach (DataArray* array, dataSeries) {
        if (array->selectedPoints.rowCount() == 0)
            height += fm.height() + 5;
        if (array->selectedPoints.rowCount() == 1)
            height += 2*(fm.height() + 5);
        if (array->selectedPoints.rowCount() == 2)
            height += 7*(fm.height() + 5);
    }
    height += 10;

    QSvgGenerator gen;
    gen.setFileName(fileName);
    gen.setSize(QSize(area.width(), height));
    gen.setViewBox(QRect(0, 0, area.width(), height));

    QPainter painter;
    painter.begin(&gen);
    painter.setFont(areaFont);
    area.renderView(painter);

    height = area.height() + fm.height() + 5;

    foreach (DataArray* array, dataSeries) {
        painter.fillRect(10, height+2, 20, -20, array->color);
        painter.drawText(40, height, array->name);
        height += fm.height() + 5;

        if (array->selectedPoints.rowCount() == 1)
        {
            QPointF p;
            int i = array->selectedPoints.item(0,0)->data().toInt();

            p = array->at(i);

            QString text("Point 1: ");
            text += QString::number(p.x()) + QString(", ") + QString::number(p.y());

            painter.drawText(10, height, text);
            height += fm.height() + 5;
        }
        else if (array->selectedPoints.rowCount() == 2)
        {
            QPointF p1, p2;
            int i1 = array->selectedPoints.item(0,0)->data().toInt(),
                    i2 = array->selectedPoints.item(1,0)->data().toInt();

            if (i1 > i2)
            {
                int tmp = i1;
                i1 = i2;
                i2 = tmp;
            }

            p1 = array->at(i1);
            p2 = array->at(i2);

            qreal min, max;
            array->yMinMax(min, max, i1, i2);

            painter.drawText(10, height, QString("Point 1: ") + QString::number(p1.x()) + QString(", ") + QString::number(p1.y()));
            height += fm.height() + 5;
            painter.drawText(10, height, QString("Point 2: ") + QString::number(p2.x()) + QString(", ") + QString::number(p2.y()));
            height += fm.height() + 5;
            painter.drawText(10, height, QString("dy/dx: ") + QString::number((p2.y()-p1.y())/(p2.x()-p1.x())));
            height += fm.height() + 5;
            painter.drawText(10, height, QString("Min: ") + QString::number(min));
            height += fm.height() + 5;
            painter.drawText(10, height, QString("Max: ") + QString::number(max));
            height += fm.height() + 5;
            painter.drawText(10, height, QString("Peak to peak: ") + QString::number(max-min));
            height += fm.height() + 5;
        }
    }
}

void MainWindow::on_actionOpen_triggered()
{
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    QStringList filters;
    filters << "Text files (*.txt *.data *)"
            << "QSP files (*.qsp)";

    fileDialog.setNameFilters(filters);
    fileDialog.show();
}

void MainWindow::on_actionSave_triggered()
{
    if (savedFile.length() == 0)
    {
        exporting = false;
        fileDialog.setAcceptMode(QFileDialog::AcceptSave);
        fileDialog.setNameFilter("QSP files (*.qsp)");
        fileDialog.show();
    }
    else
        save(savedFile);
}

void MainWindow::on_fileDialogAccepted(QString fileName)
{
    if (fileDialog.acceptMode() == QFileDialog::AcceptOpen)
    {
        loadFile(fileName);
    }

    else if (fileDialog.acceptMode() == QFileDialog::AcceptSave)
    {
        if (exporting)
        {
            exportToFile(fileName);
            exporting = false;
        }
        else
            save(fileName);
    }
}

void MainWindow::on_actionExport_triggered()
{
    exporting = true;
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setNameFilter("SVG files (*.svg)");
    fileDialog.show();
}
