/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de
    Copyright (C) 2015 Christian Eichler code@christian-eichler.de

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/

#include "CDraw.h"
#include "../canvas/CCanvas.h"

#include <QPointF>
#include <QImage>
#include <QtMath>
#include <QDebug>

QImage CDraw::createBasicArrow(const QBrush &brush)
{
    QImage arrow(21, 16, QImage::Format_ARGB32);
    arrow.fill(qRgba(0, 0, 0, 0));

    QPainter painter(&arrow);
    USE_ANTI_ALIASING(painter, true);

    // white background, same foreground as p
    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(brush);

    QPointF arrowPoints[4] =
    {
        QPointF(20.0,  7.0), // front
        QPointF( 0.0,  0.0), // upper tail
        QPointF( 5.0,  7.0), // mid   tail
        QPointF( 0.0, 15.0)  // lower tail
    };
    painter.drawPolygon(arrowPoints, 4);
    painter.end();

    return arrow;
}

/**
   @brief   Calculates the distance between two points
   @return  (int) ( (x2 - x1)^2 + (y2 - y1)^2 )^0.5
 */
template<typename T>
static inline int pointDistance(T &p1, T &p2)
{
    return qSqrt(qPow(p2.x() - p1.x(), 2) + qPow(p2.y() - p1.y(), 2));
}

void CDraw::arrows(const QPolygonF &line, const QRectF &viewport, QPainter &p, int minPointDist, int minArrowDist)
{
    QImage arrow = createBasicArrow(p.brush());

    QPoint prevArrow;
    bool firstArrow = true;
    for(int i = 1; i < line.size(); i++)
    {
        const QPointF &pt     = line[i    ];
        const QPointF &prevPt = line[i - 1];

        // ensure there is enough space between two linepts
	if( pointDistance(pt, prevPt) >= minPointDist )
        {
            QPoint arrowPos((pt.x() + prevPt.x()) / 2, (pt.y() + prevPt.y()) / 2);

            if( (viewport.contains(pt) || 0 == viewport.height()) // ensure the point is visible
            && (firstArrow || pointDistance(prevArrow, arrowPos) >= minArrowDist) )
            {
                p.save();

                // rotate and draw the arrow (between bullets)
                p.translate(arrowPos);
                qreal direction = ( qAtan2((qreal)(pt.y() - prevPt.y()), (qreal)(pt.x() - prevPt.x())) * 180.) / M_PI;
                p.rotate(direction);
                p.drawImage(-11, -7, arrow);

                p.restore();

                prevArrow  = arrowPos;
                firstArrow = false;
            }
        }
    }
}

void CDraw::text(const QString &str, QPainter &p, const QPoint &center, const QColor &color, const QFont &font)
{
    QFontMetrics fm(font);
    QRect r = fm.boundingRect(str);

    r.moveCenter(center);
    p.setFont(font);

    // draw the white `shadow`
    p.setPen(Qt::white);
    p.drawText(r.topLeft() - QPoint(-1, -1), str);
    p.drawText(r.topLeft() - QPoint( 0, -1), str);
    p.drawText(r.topLeft() - QPoint(+1, -1), str);

    p.drawText(r.topLeft() - QPoint(-1,  0), str);
    p.drawText(r.topLeft() - QPoint(+1,  0), str);

    p.drawText(r.topLeft() - QPoint(-1, +1), str);
    p.drawText(r.topLeft() - QPoint( 0, +1), str);
    p.drawText(r.topLeft() - QPoint(+1, +1), str);

    p.setPen(color);
    p.drawText(r.topLeft(), str);
}

void CDraw::text(const QString &str, QPainter &p, const QRect &r, const QColor &color)
{
    p.setPen(Qt::white);
    p.setFont(CMainWindow::self().getMapFont());

    // draw the white `shadow`
    p.drawText(r.adjusted(-1, -1, -1, -1), Qt::AlignCenter, str);
    p.drawText(r.adjusted( 0, -1,  0, -1), Qt::AlignCenter, str);
    p.drawText(r.adjusted(+1, -1, +1, -1), Qt::AlignCenter, str);

    p.drawText(r.adjusted(-1,  0, -1,  0), Qt::AlignCenter, str);
    p.drawText(r.adjusted(+1,  0, +1,  0), Qt::AlignCenter, str);

    p.drawText(r.adjusted(-1, +1, -1, +1), Qt::AlignCenter, str);
    p.drawText(r.adjusted( 0, +1,  0, +1), Qt::AlignCenter, str);
    p.drawText(r.adjusted(+1, +1, +1, +1), Qt::AlignCenter, str);

    p.setPen(color);
    p.drawText(r, Qt::AlignCenter, str);
}

QPoint CDraw::bubble(QPainter &p, const QRect &contentRect, const QPoint &pointerPos, int pointerBaseWidth, int pointerBaseProc)
{
    QPainterPath bubblePath;
    bubblePath.addRoundedRect(contentRect, 5, 5);

    // draw the arrow
    int pointerBaseCenterX = contentRect.left() + (pointerBaseProc * contentRect.width()) / 100;
    int pointerHeight = 0;
    if(pointerPos.y() < contentRect.top())         pointerHeight = contentRect.top() - pointerPos.y()    + 1;
    else if(pointerPos.y() > contentRect.bottom()) pointerHeight = contentRect.bottom() - pointerPos.y() - 1;
    else {
        qDebug() << "cannot calculate pointerHeight/pointerBaseCenterX due to invalid parameters";
        return QPoint(0, 0);
    }

    QPolygonF pointerPoly;
    pointerPoly << pointerPos
                << QPointF(pointerBaseCenterX - pointerBaseWidth / 2, pointerPos.y() + pointerHeight)
                << QPointF(pointerBaseCenterX + pointerBaseWidth / 2, pointerPos.y() + pointerHeight)
                << pointerPos;

    QPainterPath pointerPath;
    pointerPath.addPolygon(pointerPoly);

    p.setPen  (CCanvas::penBorderGray);
    p.setBrush(CCanvas::brushBackWhite);

    p.drawPolygon(bubblePath.united(pointerPath).toFillPolygon());

    return contentRect.topLeft();
}
