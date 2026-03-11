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

#include "dummypositioner.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>

DummyPositioner::DummyPositioner()
    : m_isConnected(false)
    , m_isMoving(false)
    , m_connectedIPAddress("")
    , m_stepCount(0)
{
    // Initialize step with default values
    m_txStep.AZ = 1.0;
    m_txStep.EL = 1.0;
    m_txStep.POL = 1.0;
    m_txStep.X = 0.1;
    m_txStep.Y = 0.1;
    m_txStep.Z = 0.1;

    m_rxStep.AZ = 1.0;
    m_rxStep.EL = 1.0;
    m_rxStep.POL = 1.0;
    m_rxStep.X = 0.1;
    m_rxStep.Y = 0.1;
    m_rxStep.Z = 0.1;
    
    // Initialize min range
    m_minTxRange.AZ = -180.0;
    m_minTxRange.EL = -90.0;
    m_minTxRange.POL = -180.0;
    m_minTxRange.X = -100.0;
    m_minTxRange.Y = -100.0;
    m_minTxRange.Z = -100.0;

    m_minRxRange.AZ = -180.0;
    m_minRxRange.EL = -90.0;
    m_minRxRange.POL = -180.0;
    m_minRxRange.X = -100.0;
    m_minRxRange.Y = -100.0;
    m_minRxRange.Z = -100.0;
    
    // Initialize max range
    m_maxTxRange.AZ = 180.0;
    m_maxTxRange.EL = 90.0;
    m_maxTxRange.POL = 180.0;
    m_maxTxRange.X = 100.0;
    m_maxTxRange.Y = 100.0;
    m_maxTxRange.Z = 100.0;
    
    m_maxRxRange.AZ = 180.0;
    m_maxRxRange.EL = 90.0;
    m_maxRxRange.POL = 180.0;
    m_maxRxRange.X = 100.0;
    m_maxRxRange.Y = 100.0;
    m_maxRxRange.Z = 100.0;
    
    // Initialize position
    m_currentTxPosition.AZ = 0.0;
    m_currentTxPosition.EL = 0.0;
    m_currentTxPosition.POL = 0.0;
    m_currentTxPosition.X = 0.0;
    m_currentTxPosition.Y = 0.0;
    m_currentTxPosition.Z = 0.0;

    m_currentRxPosition.AZ = 0.0;
    m_currentRxPosition.EL = 0.0;
    m_currentRxPosition.POL = 0.0;
    m_currentRxPosition.X = 0.0;
    m_currentRxPosition.Y = 0.0;
    m_currentRxPosition.Z = 0.0;
    
    std::cout << "[Dummy Positioner Plugin] Instance created" << std::endl;
}

std::vector<DeviceInfo> DummyPositioner::scanDevices()
{
    std::cout << "[Dummy Positioner Plugin] Scanning for devices..." << std::endl;
    
    std::vector<DeviceInfo> devices;
    
    // Simulate finding devices
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Simulate 1 LAN device
    DeviceInfo device1;
    device1.name = "Dummy Positioner-AZ/EL";
    device1.serialNumber = "DPS-1000";
    device1.address = "192.168.1.120";
    device1.type = "LAN";
    device1.isAvailable = true;
    devices.push_back(device1);
    
    // // Simulate 1 Serial device
    // DeviceInfo device2;
    // device2.name = "Dummy Positioner-6DOF";
    // device2.serialNumber = "DPS-2000";
    // device2.address = "COM3";
    // device2.type = "Serial";
    // device2.isAvailable = true;
    // devices.push_back(device2);
    
    std::cout << "[Dummy Positioner Plugin] Found " << devices.size() << " devices" << std::endl;
    
    // if (onDevicesScanned) {
    //     try {
    //         onDevicesScanned(devices);
    //     } catch (const std::exception& ex) {
    //         std::cerr << "[Dummy SG Plugin] Exception in onDevicesScanned callback: " << ex.what() << std::endl;
    //     } catch (...) {
    //         std::cerr << "[Dummy SG Plugin] Unknown exception in onDevicesScanned callback" << std::endl;
    //     }
    // } else {
    //     std::cerr << "[Dummy SG Plugin] onDevicesScanned callback is not set" << std::endl;
    // }
    return devices;
}

bool DummyPositioner::connectToDevice(const std::string &address, const std::string &port)
{
    if (m_isConnected) {
        std::cerr << "[Dummy Positioner Plugin] Already connected to " << m_connectedIPAddress << " on port: " << m_connectedPort << std::endl;
        return false;
    }
    
    std::cout << "[Dummy Positioner Plugin] Connecting to device at: " << address << " on port: " << port << std::endl;
    
    // Simulate connection delay
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    
    m_connectedIPAddress = address;
    m_connectedPort = port;
    m_isConnected = true;
    
    std::cout << "[Dummy Positioner Plugin] Connected successfully to " << address << " on port: " << port << std::endl;
    if (onConnected) {
        onConnected();
    }
    return true;
}

DummyPositioner::~DummyPositioner()
{
    if (m_isConnected) {
        disconnect();
    }
    std::cout << "[Dummy Positioner Plugin] Instance destroyed" << std::endl;
}

void DummyPositioner::disconnect()
{
    if (!m_isConnected) {
        std::cerr << "[Dummy Positioner Plugin] Not connected" << std::endl;
        return;
    }
    
    std::cout << "[Dummy Positioner Plugin] Disconnecting from " << m_connectedIPAddress << std::endl;
    
    m_isConnected = false;
    m_connectedIPAddress.clear();
    
    std::cout << "[Dummy Positioner Plugin] Disconnected" << std::endl;
    if (onDisconnected) {
        onDisconnected();
    }
}

bool DummyPositioner::isConnected() const
{
    return m_isConnected;
}

void DummyPositioner::setTxStep(const Position &step)
{
    m_txStep.AZ = step.AZ;
    m_txStep.EL = step.EL;
    m_txStep.POL = step.POL;
    m_txStep.X = step.X;
    m_txStep.Y = step.Y;
    m_txStep.Z = step.Z;
    std::cout << "[Planar Positioner Plugin] TX AZ step set to " << step.AZ << " degrees" << std::endl;
    std::cout << "[Planar Positioner Plugin] TX EL step set to " << step.EL << " degrees" << std::endl;
    std::cout << "[Planar Positioner Plugin] TX POL step set to " << step.POL << " degrees" << std::endl;
    std::cout << "[Planar Positioner Plugin] TX X step set to " << step.X << " mm" << std::endl;
    std::cout << "[Planar Positioner Plugin] TX Y step set to " << step.Y << " mm" << std::endl;
    std::cout << "[Planar Positioner Plugin] TX Z step set to " << step.Z << " mm" << std::endl;
}

void DummyPositioner::setRxStep(const Position &step)
{
    m_rxStep.AZ = step.AZ;
    m_rxStep.EL = step.EL;
    m_rxStep.POL = step.POL;
    m_rxStep.X = step.X;
    m_rxStep.Y = step.Y;
    m_rxStep.Z = step.Z;
    std::cout << "[Planar Positioner Plugin] RX AZ step set to " << step.AZ << " degrees" << std::endl;
    std::cout << "[Planar Positioner Plugin] RX EL step set to " << step.EL << " degrees" << std::endl;
    std::cout << "[Planar Positioner Plugin] RX POL step set to " << step.POL << " degrees" << std::endl;
    std::cout << "[Planar Positioner Plugin] RX X step set to " << step.X << " mm" << std::endl;
    std::cout << "[Planar Positioner Plugin] RX Y step set to " << step.Y << " mm" << std::endl;
    std::cout << "[Planar Positioner Plugin] RX Z step set to " << step.Z << " mm" << std::endl;
}

void DummyPositioner::setTxMinRange(const Position &minRange)
{
    m_minTxRange.AZ = minRange.AZ;
    m_minTxRange.EL = minRange.EL;
    m_minTxRange.POL = minRange.POL;
    m_minTxRange.X = minRange.X;
    m_minTxRange.Y = minRange.Y;
    m_minTxRange.Z = minRange.Z;
    std::cout << "[Planar Positioner Plugin] Min Tx Range set:" << std::endl;
    std::cout << "  AZ: " << minRange.AZ << " EL: " << minRange.EL << " POL: " << minRange.POL << "X: " << minRange.X << " Y: " << minRange.Y << " Z: " << minRange.Z << std::endl;
}

void DummyPositioner::setRxMinRange(const Position &minRange)
{
    m_minRxRange.AZ = minRange.AZ;
    m_minRxRange.EL = minRange.EL;
    m_minRxRange.POL = minRange.POL;
    m_minRxRange.X = minRange.X;
    m_minRxRange.Y = minRange.Y;
    m_minRxRange.Z = minRange.Z;
    std::cout << "[Planar Positioner Plugin] Min Rx Range set:" << std::endl;
    std::cout << "  AZ: " << minRange.AZ << " EL: " << minRange.EL << " POL: " << minRange.POL << "X: " << minRange.X << " Y: " << minRange.Y << " Z: " << minRange.Z << std::endl;
}

void DummyPositioner::setTxMaxRange(const Position &maxRange)
{
    m_maxTxRange.AZ = maxRange.AZ;
    m_maxTxRange.EL = maxRange.EL;
    m_maxTxRange.POL = maxRange.POL;
    m_maxTxRange.X = maxRange.X;
    m_maxTxRange.Y = maxRange.Y;
    m_maxTxRange.Z = maxRange.Z;
    std::cout << "[Planar Positioner Plugin] Max Tx Range set:" << std::endl;
    std::cout << "  AZ: " << maxRange.AZ << " EL: " << maxRange.EL << " POL: " << maxRange.POL << "X: " << maxRange.X << " Y: " << maxRange.Y << " Z: " << maxRange.Z << std::endl;
}

void DummyPositioner::setRxMaxRange(const Position &maxRange)
{
    m_maxRxRange.AZ = maxRange.AZ;
    m_maxRxRange.EL = maxRange.EL;
    m_maxRxRange.POL = maxRange.POL;
    m_maxRxRange.X = maxRange.X;
    m_maxRxRange.Y = maxRange.Y;
    m_maxRxRange.Z = maxRange.Z;
    std::cout << "[Planar Positioner Plugin] Max Rx Range set:" << std::endl;
    std::cout << "  AZ: " << maxRange.AZ << " EL: " << maxRange.EL << " POL: " << maxRange.POL << "X: " << maxRange.X << " Y: " << maxRange.Y << " Z: " << maxRange.Z << std::endl;
}

Position DummyPositioner::getCurrentTxPosition() const
{
    return m_currentTxPosition;
}

Position DummyPositioner::getCurrentRxPosition() const
{
    return m_currentRxPosition;
}

void DummyPositioner::moveTxToPosition(const Position &position)
{
    // Populate Real Motor Command Here based on the position movement
    // The syntax is: motor_1 on [diatance] [direction] 
    // motor_1: TX horizontal
    // motor_2: TX vertical
    // motor_5: TX polarization

    // if (m_currentTxPosition.X != position.X) {
    //     m_motorCommand[0] = "motor_1 on " + std::to_string(std::abs(position.X - m_currentTxPosition.X)) + ((position.X > m_currentTxPosition.X) ? " maju" : " mundur");
    // }
    // if (m_currentTxPosition.Y != position.Y) {
    //     m_motorCommand[1] = "motor_2 on " + std::to_string(std::abs(position.Y - m_currentTxPosition.Y)) + ((position.Y > m_currentTxPosition.Y) ? " naik" : " turun");
    // }
    // if (m_currentTxPosition.POL != position.POL) {
    //     m_motorCommand[4] = "motor_5 on " + std::to_string(std::abs(position.POL - m_currentTxPosition.POL)) + ((position.POL > m_currentTxPosition.POL) ? " CW" : " CCW");
    // }

    std::cout << "[Planar Positioner Plugin] moveTxToPosition called with position: AZ=" << position.AZ << " EL=" << position.EL << " POL=" << position.POL << " X=" << position.X << " Y=" << position.Y << " Z=" << position.Z << std::endl;
}

void DummyPositioner::moveRxToPosition(const Position &position)
{
    // Populate Real Motor Command Here based on the position movement
    // The syntax is: motor_1 on [diatance] [direction] 
    // motor_3: RX horizontal
    // motor_4: RX vertical
    
    // if (m_currentRxPosition.X != position.X) {
    //     m_motorCommand[2] = "motor_3 on " + std::to_string(std::abs(position.X - m_currentRxPosition.X)) + ((position.X > m_currentRxPosition.X) ? " maju" : " mundur");
    // }
    // if (m_currentRxPosition.Y != position.Y) {
    //     m_motorCommand[3] = "motor_4 on " + std::to_string(std::abs(position.Y - m_currentRxPosition.Y)) + ((position.Y > m_currentRxPosition.Y) ? " naik" : " turun");
    // }
    
    std::cout << "[Planar Positioner Plugin] moveRxToPosition called with position: AZ=" << position.AZ << " EL=" << position.EL << " POL=" << position.POL << " X=" << position.X << " Y=" << position.Y << " Z=" << position.Z << std::endl;  
}

void DummyPositioner::stopMovement()
{
    std::cout << "[Planar Positioner Plugin] Stopping movement..." << std::endl;
}

// Factory function to create plugin instance
extern "C" {
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    IPositionerPlugin* createPositionerPlugin()
    {
        std::cout << "[Dummy Positioner Plugin] Factory: Creating plugin instance" << std::endl;
        return new DummyPositioner();
    }
    
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    void destroyPlugin(void* plugin)
    {
        std::cout << "[Dummy Positioner Plugin] Factory: Destroying plugin instance" << std::endl;
        delete static_cast<IPositionerPlugin*>(plugin);
    }
}
