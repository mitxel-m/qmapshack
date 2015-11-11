/**********************************************************************************************
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

#include "CMainWindow.h"
#include "widgets/CColorLegend.h"

#include <QtWidgets>

CColorLegend::CColorLegend(QWidget *parent)
    : QWidget(parent)
{
    xOffset = 1;
    colorRect = QRect(0, 0, colorWidth, colorHeight);
    colorRect.moveCenter(QPoint(xOffset + colorWidth / 2, height() / 2));
}

CColorLegend::CColorLegend(QWidget *parent, CGisItemTrk *trk)
    : QWidget(parent), trk(trk)
{
    background = true;
    xOffset = 5;

    colorRect = QRect(0, 0, colorWidth, colorHeight);
    colorRect.moveCenter(QPoint(xOffset + colorWidth / 2, height() / 2));

    trk->registerNotification(this);

    // read data from trk
    notify();
}

CColorLegend::~CColorLegend()
{
    if(trk)
    {
        trk->unregisterNotification(this);
    }
}

void CColorLegend::notify()
{
    if(!trk->getColorizeSource().isEmpty())
    {
        unit    = trk->getColorizeUnit();
        minimum = trk->getColorizeLimitLow();
        maximum = trk->getColorizeLimitHigh();

        update();
        show();
    } else {
        hide();
    }
}

void CColorLegend::setMinimum(qreal min)
{
    minimum = min;
    update();
}

void CColorLegend::setMaximum(qreal max)
{
    maximum = max;
    update();
}

void CColorLegend::setUnit(const QString &unit)
{
    this->unit = unit;
    update();
}

int CColorLegend::paintLabel(QPainter &p, qreal value)
{
    const int fontHeight = QFontMetrics(p.font()).ascent() + 1;
    const int posY = colorRect.bottom() + fontHeight / 2 - colorHeight * (value - minimum) / (maximum - minimum);

    int posX = 0;

    if(value == minimum || value == maximum
     || (posY > colorRect.top() + 3*fontHeight / 2 && posY < colorRect.bottom() - fontHeight / 2))
    {
        p.setPen( QPen(QBrush(Qt::black), 3.) );
        p.drawText(xOffset + colorWidth + 8, posY, QString("%1%2").arg(value).arg(unit));
        posX = xOffset + colorWidth + 8 + QFontMetrics(p.font()).width(QString("%1%2").arg(value).arg(unit));
    }

    p.setPen( QPen(QBrush(Qt::black), 2.) );
    p.drawLine(xOffset + colorWidth + 3, posY - fontHeight / 2 + 1, xOffset + colorWidth + 5, posY - fontHeight / 2 + 1);

    return posX;
}

void CColorLegend::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    colorRect.moveCenter(QPoint(xOffset + colorWidth / 2, height() / 2));
    updateGeometry();
}

void CColorLegend::paintEvent(QPaintEvent *event)
{
    const QFont &font = CMainWindow::self().getMapFont();
    if(isEnabled())
    {
        QPainter p(this);
        p.setFont(font);

        if(background)
        {
            p.setRenderHint(QPainter::Antialiasing);
            p.setOpacity(0.6);

            p.setPen( QPen(QBrush(Qt::darkGray), 2.) );
            p.setBrush(Qt::white);
            p.drawRoundedRect(1, 1, width() - 2, height() - 2, 5.f, 5.f);

            p.setOpacity(1.f);
            p.setRenderHint(QPainter::Antialiasing, false);
        }

        // draw the black frame
        QRect borderRect(colorRect);
        borderRect += QMargins(1, 1, 1, 1);
        p.setPen( QPen(QBrush(Qt::SolidPattern), 2., Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin) );
        p.drawRect(borderRect);

        // draw the gradient
        QLinearGradient grad(colorRect.topLeft(), colorRect.bottomLeft());
        grad.setColorAt(1.00, QColor(  0,   0, 255)); // blue
        grad.setColorAt(0.60, QColor(  0, 255,   0)); // green
        grad.setColorAt(0.40, QColor(255, 255,   0)); // yellow
        grad.setColorAt(0.00, QColor(255,   0,   0)); // red
        p.fillRect(colorRect, grad);

        int reqWidth =  paintLabel(p, minimum);
        reqWidth = qMax(paintLabel(p, maximum), reqWidth);

        // draw values inbetween min/max
        if(minimum < 0.f && maximum > 0.f)
        {
            reqWidth = qMax(paintLabel(p, 0.), reqWidth);
        }

        if(reqWidth + 5 != width())
        {
            setMinimumWidth(reqWidth + 5);
            resize(reqWidth + 5, height());
        }

        p.end();
    }
}
