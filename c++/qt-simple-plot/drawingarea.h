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

#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QWidget>
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QSettings>
#include <dataseries.h>

class DrawingArea : public QWidget
{
    Q_OBJECT
public:
    explicit DrawingArea(QWidget *parent = 0);
    ~DrawingArea();

    void setDataSeries(DataSeries* series);
    void setActiveSeries(int index);
    void setSelectedPoint(int index);
    void setXAxisLabel(QString label);
    void setYAxisLabel(QString label);

    void defineView(qreal xMin, qreal xMax, qreal yMin, qreal yMax);
    void getView(qreal& xMin, qreal& xMax, qreal& yMin, qreal& yMax);
    void renderView(QPainter &painter);



protected:
    void wheelEvent(QWheelEvent* event);
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    void calcAxis(qreal min, qreal max, qreal& intMin, qreal& intMax,
                  qreal& delta, qreal& subMin, qreal& subMax, qreal& subDelta);

    QPointF pointToLocal(QPointF point);
    QPointF localToPoint(QPointF local);

signals:
    void measurePointAdded(int series, int pointIndex);

private:
    DataSeries* dataSeries;
    int selectedPoint, selectedArray;
    qreal xMin, xMax, yMin, yMax;
    QString xLabel, yLabel;
    int reservedHeight, reservedWidth;
    QPointF lastPosition;
    QSettings settings;

signals:

public slots:
};

#endif // DRAWINGAREA_H
