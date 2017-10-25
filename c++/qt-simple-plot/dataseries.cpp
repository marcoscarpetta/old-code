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

#include "dataseries.h"

DataSeries::DataSeries()
{

}

DataSeries::~DataSeries()
{

}

int DataSeries::loadFromFile(QString filename)
{
    while (size() > 0)
        removeFirst();

    QFile file(filename);
    if (!file.open(QFile::ReadOnly))
        return 1;

    QString stringLine = QString(file.readLine()).remove("\n");
    QStringList line = stringLine.split(" ", QString::SkipEmptyParts);

    if ((line.size() % 2) == 0)
    {
        for (int i=0; i<line.size(); i=i+2)
        {
            DataArray* newDataArray = new DataArray();
            this->append(newDataArray);
            newDataArray->append(QPointF(line[i].toDouble(), line[i+1].toDouble()));

            newDataArray->name = QString("Series ").append(QString().setNum(i/2));
        }

        int index = 0;
        while (!file.atEnd())
        {
            QStringList line = QString(file.readLine()).split(" ", QString::SkipEmptyParts);

            for (int i=0; i<size(); i++)
            {
                int arrayIndex = 0;
                if (index != 0) arrayIndex = index;
                else arrayIndex = i;
                if (2*i+1 < line.size())
                {
                    if (line[2*i].toDouble() < at(arrayIndex)->last().x())
                    {
                        index += 1;
                        DataArray* newDataArray = new DataArray();
                        newDataArray->name = QString("Series ").append(QString().setNum(index));
                        this->append(newDataArray);
                        arrayIndex = index;
                    }
                    at(arrayIndex)->append(QPointF(line[2*i].toDouble(), line[2*i+1].toDouble()));
                }
            }
        }

        return 0;
    }
    return 1;
}

void DataSeries::xMinMax(qreal& min, qreal& max)
{
    at(0)->xMinMax(min, max);

    qreal tmpMin, tmpMax;

    for (int i=1; i<size(); i++)
    {
        at(i)->xMinMax(tmpMin, tmpMax);

        if (tmpMax > max) max = tmpMax;
        if (tmpMin < min) min = tmpMin;
    }
}

void DataSeries::yMinMax(qreal& min, qreal& max)
{
    at(0)->yMinMax(min, max);

    qreal tmpMin, tmpMax;

    for (int i=1; i<size(); i++)
    {
        at(i)->yMinMax(tmpMin, tmpMax);

        if (tmpMax > max) max = tmpMax;
        if (tmpMin < min) min = tmpMin;
    }
}
