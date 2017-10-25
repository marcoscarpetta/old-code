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

#include "dataarray.h"
#include <cstdlib>

DataArray::DataArray()
{
    int r = rand() % 256;
    int g = rand() % 256;
    int b = rand() % 256;
    color = QColor(r, g, b);
}

DataArray::~DataArray()
{

}

void DataArray::xMinMax(qreal& min, qreal& max)
{
    min = max = at(0).x();

    for (int i=1; i<size(); i++)
    {
        if (at(i).x() > max) max = at(i).x();
        if (at(i).x() < min) min = at(i).x();
    }
}

void DataArray::yMinMax(qreal& min, qreal& max, int from, int to)
{
    to++;

    if (from < 0)
        from = 0;
    if (to < 0 || to > size())
        to = size();

    min = max = at(from).y();

    for (int i=from; i<to; i++)
    {
        if (at(i).y() > max) max = at(i).y();
        if (at(i).y() < min) min = at(i).y();
    }
}
