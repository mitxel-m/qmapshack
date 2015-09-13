/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CPRINTDIALOG_H
#define CPRINTDIALOG_H

#include "ui_IPrintDialog.h"
#include <QDialog>
#include <QPrinter>

class CCanvas;

class CPrintDialog : public QDialog, private Ui::IPrintDialog
{
    Q_OBJECT
public:
    CPrintDialog(const QRectF &area, CCanvas * canvas);
    virtual ~CPrintDialog();

private slots:
    void slotZoom();
    void slotProperties();
    void slot();

private:
    void updateMetrics();

    CCanvas * canvas;
    CCanvas * preview;

    QRectF area;
    QPrinter printer;
};

#endif //CPRINTDIALOG_H
