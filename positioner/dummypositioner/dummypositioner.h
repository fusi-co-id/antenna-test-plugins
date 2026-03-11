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
    bool connectToDevice(const std::string &address, const std::string &port) override;
    
    // Connection management
    void disconnect() override;
    bool isConnected() const override;
    
    // Configuration
    virtual void setTxStep(const Position &step) override;
    virtual void setTxMinRange(const Position &minRange) override;
    virtual void setTxMaxRange(const Position &maxRange) override;
    virtual void setRxStep(const Position &step) override;
    virtual void setRxMinRange(const Position &minRange) override;
    virtual void setRxMaxRange(const Position &maxRange) override;

    // Get Position
    virtual Position getCurrentTxPosition() const override;
    virtual Position getCurrentRxPosition() const override;

    // Control
    virtual void moveTxToPosition(const Position &position) override;
    virtual void moveRxToPosition(const Position &position) override;
    virtual void stopMovement() override;
    
    // Callback functions for events (optional, can be nullptr)
    std::function<void()> onConnected;
    std::function<void()> onDisconnected;
    std::function<void()> onTxMovementStarted;
    std::function<void()> onTxMovementStopped;
    std::function<void()> onRxMovementStarted;
    std::function<void()> onRxMovementStopped;
    std::function<void(const Position&)> onTxPositionChanged;
    std::function<void(const Position&)> onRxPositionChanged;
    std::function<void(const std::string&)> onError;
    std::function<void(const std::vector<DeviceInfo>&)> onDevicesScanned;
            
private:
    void movementThread();
    
    // Socket Connection
    std::string m_connectedIPAddress;
    std::string m_connectedPort;
    bool m_isConnected;

    std::atomic<bool> m_isMoving;
    Position m_txStep;
    Position m_minTxRange;
    Position m_maxTxRange;
    Position m_rxStep;
    Position m_minRxRange;
    Position m_maxRxRange;
    
    // Current position
    Position m_currentTxPosition;
    Position m_currentRxPosition;
    
    std::thread m_movementThread;
    int m_stepCount;
};

#endif // DUMMYPOSITIONER_H
