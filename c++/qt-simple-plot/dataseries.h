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

#ifndef DATASERIES_H
#define DATASERIES_H

#include <QFile>
#include <QStringList>
#include <dataarray.h>

class DataSeries : public QList<DataArray*>
{
public:
    DataSeries();
    ~DataSeries();

    int loadFromFile(QString filename);

    void xMinMax(qreal& min, qreal& max);
    void yMinMax(qreal& min, qreal& max);

signals:

public slots:

private:

};

#endif // DATASERIES_H
