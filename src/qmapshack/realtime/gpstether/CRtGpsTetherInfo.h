/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CRTGPSINFO_H
#define CRTGPSINFO_H

#include "ui_IRtGpsTetherInfo.h"

#include <functional>
#include <QTcpSocket>
#include <QWidget>

using fNemaLine = std::function<void(const QStringList&)>;

class CRtGpsTether;
class QSettings;

class CRtGpsTetherInfo : public QWidget, private Ui::IRtGpsTetherInfo
{
    Q_OBJECT
public:
    CRtGpsTetherInfo(CRtGpsTether& source, QWidget *parent);
    virtual ~CRtGpsTetherInfo();

    void loadSettings(QSettings& cfg);
    void saveSettings(QSettings& cfg) const;    

    QPointF getPosition() const;
signals:
    void sigChanged();

private slots:
    void slotHelp() const;
    void slotConnect(bool yes);
    void slotConnected();
    void slotDisconnected();
    void slotError(QAbstractSocket::SocketError socketError);
    void slotReadyRead();
    void slotUpdate();

private:
    bool verifyLine(const QString& line);
    void disconnectFromHost();
    void autoConnect(int msec);

    fNemaLine nmeaDefault = [&](const QStringList& t){qDebug() << t[0] << "unknown";};

    void nmeaGPGSV(const QStringList& tokens);
    void nmeaGPRMC(const QStringList& tokens);
    void nmeaGPGGA(const QStringList& tokens);
    void nmeaGPVTG(const QStringList& tokens);
    void nmeaGPGSA(const QStringList& tokens);

private:
    CRtGpsTether& source;

    QTcpSocket * socket;
    QTimer * timer;

    QHash<QString,fNemaLine> dict;

    QDateTime lastTimestamp;

    struct rmc_t
    {
        bool isValid {false};
        QDateTime datetime;
        qreal lat {0.0};
        qreal lon {0.0};
        qreal groundSpeed {0.0};
        qreal trackMadeGood {0.0};
        qreal magneticVariation {0.0};
    };

    rmc_t rmc;

    struct gga_t
    {
        bool isValid {false};
        QDateTime datetime;
        qreal lat  {0.0};
        qreal lon  {0.0};
        qint32 quality {-1};
        qint32 numSatelites {0};
        qreal horizDilution {0.0};
        qreal altAboveSeaLevel {0.0};
        qreal geodialSeparation {0.0};
        qreal age {0};
        qint32 diffRefStation {0};
    };

    gga_t gga;
};

#endif //CRTGPSINFO_H
