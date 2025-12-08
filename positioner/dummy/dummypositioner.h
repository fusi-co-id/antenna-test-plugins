/****************************************************************************
**
** Copyright (C) 2025 PT Fusi Global Teknologi. All rights reserved.
** Coded by: Yan Syafri Hidayat
**
** This file is part of the Antenna Tester GUI plugin interface.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
****************************************************************************/

#ifndef DUMMYPOSITIONER_H
#define DUMMYPOSITIONER_H

#include "iplugininterface.h"
#include <QObject>
#include <QString>
#include <QTimer>
#include <QRandomGenerator>

class DummyPositioner : public IPositionerPlugin
{
    Q_OBJECT
    Q_INTERFACES(IPositionerPlugin)
    
public:
    DummyPositioner();
    virtual ~DummyPositioner();
    
    // Device discovery
    QVector<DeviceInfo> scanDevices() override;
    bool connectToDevice(const QString &address) override;
    
    // Connection management
    bool connect() override;
    void disconnect() override;
    bool isConnected() const override;
    
    // Configuration
    void setAZStep(double step) override;
    void setStep(const Step &step) override;
    void setMinRange(const MinRange &minRange) override;
    void setMaxRange(const MaxRange &maxRange) override;
    void setMovement(const Movement &movement) override;
    void setDistance(double distance) override;
    
    // Control
    void start() override;
    void stop() override;
    
signals:
    void connected();
    void disconnected();
    void movementStarted();
    void movementStopped();
    void positionChanged(double az, double el, double pol);
    void errorOccurred(const QString &error);
    
private slots:
    void onTimerTick();
    
private:
    bool m_isConnected;
    bool m_isMoving;
    Step m_step;
    MinRange m_minRange;
    MaxRange m_maxRange;
    Movement m_currentMovement;
    double m_distance;
    QString m_connectedAddress;
    
    // Current position
    double m_currentAZ;
    double m_currentEL;
    double m_currentPOL;
    
    QTimer *m_timer;
    int m_stepCount;
};

#endif // DUMMYPOSITIONER_H
