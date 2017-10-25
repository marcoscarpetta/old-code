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

#include "plotconfiguredialog.h"
#include "ui_plotconfiguredialog.h"

PlotConfigureDialog::PlotConfigureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotConfigureDialog)
{
    ui->setupUi(this);
}

PlotConfigureDialog::~PlotConfigureDialog()
{
    delete ui;
}

void PlotConfigureDialog::setXLabel(QString label)
{
    ui->xAxisLine->setText(label);
}

void PlotConfigureDialog::setYLabel(QString label)
{
    ui->yAxisLine->setText(label);
}

QString PlotConfigureDialog::xLabel()
{
    return ui->xAxisLine->text();
}

QString PlotConfigureDialog::yLabel()
{
    return ui->yAxisLine->text();
}
