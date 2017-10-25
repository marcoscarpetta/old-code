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

#include "configdialog.h"
#include "ui_configdialog.h"

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);

    connect(&colorDialog, SIGNAL(accepted()), this, SLOT(on_colorDialogAccepted()));
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::setColor(QColor color)
{
    currentColor = color;
    colorDialog.setCurrentColor(color);

    QPixmap pixmap(20, 20);
    pixmap.fill(color);
    QIcon icon(pixmap);
    ui->pushButton->setIcon(icon);
}

void ConfigDialog::setName(QString name)
{
    ui->lineEdit->setText(name);
}

QColor ConfigDialog::color()
{
    return currentColor;
}

QString ConfigDialog::name()
{
    return ui->lineEdit->text();
}

void ConfigDialog::on_pushButton_clicked()
{
    colorDialog.show();
}

void ConfigDialog::on_colorDialogAccepted()
{
    setColor(colorDialog.currentColor());
}
