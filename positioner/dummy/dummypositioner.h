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
#include <string>
#include <thread>
#include <atomic>
#include <chrono>

class DummyPositioner : public IPositionerPlugin
{
    
public:
    DummyPositioner();
    virtual ~DummyPositioner();
    
    // Device discovery
    std::vector<DeviceInfo> scanDevices() override;
    bool connectToDevice(const std::string &address) override;
    
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
    
    // Get Position
    double getCurrentAZ() const override;
    double getCurrentEL() const override;
    double getCurrentPOL() const override;
    
    // Control
    void start() override;
    void stop() override;
    void moveTo(double azimuth, double elevation) override;
    void moveTo(double azimuth, double elevation, double polar) override;
    
private:
    void movementThread();
    
    bool m_isConnected;
    std::atomic<bool> m_isMoving;
    Step m_step;
    MinRange m_minRange;
    MaxRange m_maxRange;
    Movement m_currentMovement;
    double m_distance;
    std::string m_connectedAddress;
    
    // Current position
    double m_currentAZ;
    double m_currentEL;
    double m_currentPOL;
    
    std::thread m_movementThread;
    int m_stepCount;
};

#endif // DUMMYPOSITIONER_H
