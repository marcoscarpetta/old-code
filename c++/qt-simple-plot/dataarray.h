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

#ifndef DATAARRAY_H
#define DATAARRAY_H

#include <QObject>
#include <QDebug>
#include <QColor>
#include <QList>
#include <QPointF>
#include <QStandardItemModel>

class DataArray : public QList<QPointF>
{
public:
    DataArray();
    ~DataArray();

    void xMinMax(qreal& min, qreal& max);
    void yMinMax(qreal& min, qreal& max, int from=-1, int to=-2);

    QColor color;
    QString name;
    QStandardItemModel selectedPoints;
};

#endif // DATAARRAY_H
