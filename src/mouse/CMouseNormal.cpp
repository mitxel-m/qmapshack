/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#include "mouse/CMouseNormal.h"
#include "CCanvas.h"

#include <QtGui>

CMouseNormal::CMouseNormal(CCanvas *canvas)
    : IMouse(canvas)
    , mapMove(false)
{
    cursor = QCursor(QPixmap(":/cursors/cursorMoveMap.png"),0,0);
}

CMouseNormal::~CMouseNormal()
{

}

void CMouseNormal::mousePressEvent(QMouseEvent * e)
{
    if(e->button() == Qt::LeftButton)
    {
        lastPos = e->pos();
        mapMove = true;
    }
}

void CMouseNormal::mouseMoveEvent(QMouseEvent * e)
{
    const QPoint pos = e->pos();

    if(mapMove)
    {
        if(pos != lastPos)
        {
            QPoint delta = pos - lastPos;
            canvas->moveMap(delta);
            lastPos = pos;
        }
    }
    else
    {
        canvas->displayInfo(pos);
    }
}

void CMouseNormal::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        mapMove = false;
    }
}
