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
    , m_distance(0.0)
    , m_currentAZ(0.0)
    , m_currentEL(0.0)
    , m_currentPOL(0.0)
    , m_connectedAddress("")
    , m_stepCount(0)
{
    // Initialize step with default values
    m_step.AZ = 1.0;
    m_step.EL = 1.0;
    m_step.POL = 1.0;
    m_step.X = 0.1;
    m_step.Y = 0.1;
    m_step.V = 0.1;
    
    // Initialize min range
    m_minRange.AZ = -180.0;
    m_minRange.EL = -90.0;
    m_minRange.POL = -180.0;
    m_minRange.X = -100.0;
    m_minRange.Y = -100.0;
    m_minRange.V = -100.0;
    
    // Initialize max range
    m_maxRange.AZ = 180.0;
    m_maxRange.EL = 90.0;
    m_maxRange.POL = 180.0;
    m_maxRange.X = 100.0;
    m_maxRange.Y = 100.0;
    m_maxRange.V = 100.0;
    
    // Initialize movement
    m_currentMovement.AZ = 0.0;
    m_currentMovement.EL = 0.0;
    m_currentMovement.POL = 0.0;
    m_currentMovement.X = 0.0;
    m_currentMovement.Y = 0.0;
    m_currentMovement.V = 0.0;
    
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
    
    if (onDevicesScanned) {
        onDevicesScanned(devices);
    }
    return devices;
}

bool DummyPositioner::connectToDevice(const std::string &address)
{
    if (m_isConnected) {
        std::cerr << "[Dummy Positioner Plugin] Already connected to " << m_connectedAddress << std::endl;
        return false;
    }
    
    std::cout << "[Dummy Positioner Plugin] Connecting to device at: " << address << std::endl;
    
    // Simulate connection delay
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    
    m_connectedAddress = address;
    m_isConnected = true;
    
    std::cout << "[Dummy Positioner Plugin] Connected successfully to " << address << std::endl;
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

bool DummyPositioner::connect()
{
    if (m_isConnected) {
        std::cerr << "[Dummy Positioner Plugin] Already connected" << std::endl;
        return true;
    }
    
    std::cout << "[Dummy Positioner Plugin] Connecting to simulated positioner..." << std::endl;
    
    // Simulate connection delay
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    m_isConnected = true;
    
    std::cout << "[Dummy Positioner Plugin] Connected successfully" << std::endl;
    std::cout << "  Device: Dummy Positioner v2.0" << std::endl;
    std::cout << "  AZ Range: " << m_minRange.AZ << " to " << m_maxRange.AZ << " degrees" << std::endl;
    std::cout << "  EL Range: " << m_minRange.EL << " to " << m_maxRange.EL << " degrees" << std::endl;
    std::cout << "  POL Range: " << m_minRange.POL << " to " << m_maxRange.POL << " degrees" << std::endl;
    
    if (onConnected) {
        onConnected();
    }
    return true;
}

void DummyPositioner::disconnect()
{
    if (!m_isConnected) {
        std::cerr << "[Dummy Positioner Plugin] Not connected" << std::endl;
        return;
    }
    
    if (m_isMoving) {
        stop();
    }
    
    std::cout << "[Dummy Positioner Plugin] Disconnecting from " << m_connectedAddress << std::endl;
    
    m_isConnected = false;
    m_connectedAddress.clear();
    
    std::cout << "[Dummy Positioner Plugin] Disconnected" << std::endl;
    if (onDisconnected) {
        onDisconnected();
    }
}

bool DummyPositioner::isConnected() const
{
    return m_isConnected;
}

void DummyPositioner::setAZStep(double step)
{
    m_step.AZ = step;
    std::cout << "[Dummy Positioner Plugin] AZ Step set to " << step << " degrees" << std::endl;
}

void DummyPositioner::setStep(const Step &step)
{
    m_step = step;
    std::cout << "[Dummy Positioner Plugin] Step set:" << std::endl;
    std::cout << "  AZ: " << step.AZ << " EL: " << step.EL << " POL: " << step.POL << std::endl;
    std::cout << "  X: " << step.X << " Y: " << step.Y << " V: " << step.V << std::endl;
}

void DummyPositioner::setMinRange(const MinRange &minRange)
{
    m_minRange = minRange;
    std::cout << "[Dummy Positioner Plugin] Min Range set:" << std::endl;
    std::cout << "  AZ: " << minRange.AZ << " EL: " << minRange.EL << " POL: " << minRange.POL << std::endl;
}

void DummyPositioner::setMaxRange(const MaxRange &maxRange)
{
    m_maxRange = maxRange;
    std::cout << "[Dummy Positioner Plugin] Max Range set:" << std::endl;
    std::cout << "  AZ: " << maxRange.AZ << " EL: " << maxRange.EL << " POL: " << maxRange.POL << std::endl;
}

void DummyPositioner::setMovement(const Movement &movement)
{
    m_currentMovement = movement;
    std::cout << "[Dummy Positioner Plugin] Movement set:" << std::endl;
    std::cout << "  AZ: " << movement.AZ << " EL: " << movement.EL << " POL: " << movement.POL << std::endl;
    std::cout << "  X: " << movement.X << " Y: " << movement.Y << " V: " << movement.V << std::endl;
}

void DummyPositioner::setDistance(double distance)
{
    m_distance = distance;
    std::cout << "[Dummy Positioner Plugin] Distance set to " << distance << std::endl;
}

double DummyPositioner::getCurrentAZ() const
{
    return m_currentAZ;
}

double DummyPositioner::getCurrentEL() const
{
    return m_currentEL;
}

double DummyPositioner::getCurrentPOL() const
{
    return m_currentPOL;
}

void DummyPositioner::moveTo(double azimuth, double elevation)
{
    if (!m_isConnected) {
        std::cerr << "[Dummy Positioner Plugin] Cannot move - not connected" << std::endl;
        if (onError) {
            onError("Positioner not connected");
        }
        return;
    }
    
    std::cout << "[Dummy Positioner Plugin] Moving to position: AZ=" << azimuth << "° EL=" << elevation << "°" << std::endl;
    
    // Stop any existing movement
    if (m_isMoving) {
        stop();
    }
    
    // Calculate movement direction
    m_currentMovement.AZ = (azimuth > m_currentAZ) ? 1.0 : -1.0;
    m_currentMovement.EL = (elevation > m_currentEL) ? 1.0 : -1.0;
    m_currentMovement.POL = 0.0; // Don't change polarization
    
    // Start movement
    m_isMoving = true;
    m_stepCount = 0;
    
    // Start movement thread with target position
    m_movementThread = std::thread([this, azimuth, elevation]() {
        while (m_isMoving) {
            // Calculate distance to target
            double azDiff = azimuth - m_currentAZ;
            double elDiff = elevation - m_currentEL;
            
            // Check if we're close enough
            if (std::abs(azDiff) < m_step.AZ && std::abs(elDiff) < m_step.EL) {
                m_currentAZ = azimuth;
                m_currentEL = elevation;
                std::cout << "[Dummy Positioner Plugin] Target position reached" << std::endl;
                m_isMoving = false;
                break;
            }
            
            // Move one step toward target
            if (std::abs(azDiff) >= m_step.AZ) {
                m_currentAZ += (azDiff > 0 ? m_step.AZ : -m_step.AZ);
            }
            if (std::abs(elDiff) >= m_step.EL) {
                m_currentEL += (elDiff > 0 ? m_step.EL : -m_step.EL);
            }
            
            m_stepCount++;
            
            // Emit position changed callback
            if (onPositionChanged) {
                onPositionChanged(m_currentAZ, m_currentEL, m_currentPOL);
            }
            
            // Sleep for 100ms between steps
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        if (onMovementStopped) {
            onMovementStopped();
        }
    });
    
    if (onMovementStarted) {
        onMovementStarted();
    }
}

void DummyPositioner::moveTo(double azimuth, double elevation, double polar)
{
    if (!m_isConnected) {
        std::cerr << "[Dummy Positioner Plugin] Cannot move - not connected" << std::endl;
        if (onError) {
            onError("Positioner not connected");
        }
        return;
    }
    
    std::cout << "[Dummy Positioner Plugin] Moving to position: AZ=" << azimuth << "° EL=" << elevation << "° POL=" << polar << "°" << std::endl;
    
    // Stop any existing movement
    if (m_isMoving) {
        stop();
    }
    
    // Calculate movement direction
    m_currentMovement.AZ = (azimuth > m_currentAZ) ? 1.0 : -1.0;
    m_currentMovement.EL = (elevation > m_currentEL) ? 1.0 : -1.0;
    m_currentMovement.POL = (polar > m_currentPOL) ? 1.0 : -1.0;
    
    // Start movement
    m_isMoving = true;
    m_stepCount = 0;
    
    // Start movement thread with target position
    m_movementThread = std::thread([this, azimuth, elevation, polar]() {
        while (m_isMoving) {
            // Calculate distance to target
            double azDiff = azimuth - m_currentAZ;
            double elDiff = elevation - m_currentEL;
            double polDiff = polar - m_currentPOL;
            
            // Check if we're close enough
            if (std::abs(azDiff) < m_step.AZ && std::abs(elDiff) < m_step.EL && std::abs(polDiff) < m_step.POL) {
                m_currentAZ = azimuth;
                m_currentEL = elevation;
                m_currentPOL = polar;
                std::cout << "[Dummy Positioner Plugin] Target position reached" << std::endl;
                m_isMoving = false;
                break;
            }
            
            // Move one step toward target
            if (std::abs(azDiff) >= m_step.AZ) {
                m_currentAZ += (azDiff > 0 ? m_step.AZ : -m_step.AZ);
            }
            if (std::abs(elDiff) >= m_step.EL) {
                m_currentEL += (elDiff > 0 ? m_step.EL : -m_step.EL);
            }
            if (std::abs(polDiff) >= m_step.POL) {
                m_currentPOL += (polDiff > 0 ? m_step.POL : -m_step.POL);
            }
            
            m_stepCount++;
            
            // Emit position changed callback
            if (onPositionChanged) {
                onPositionChanged(m_currentAZ, m_currentEL, m_currentPOL);
            }
            
            // Sleep for 100ms between steps
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        if (onMovementStopped) {
            onMovementStopped();
        }
    });
    
    if (onMovementStarted) {
        onMovementStarted();
    }
}

void DummyPositioner::start()
{
    if (!m_isConnected) {
        std::cerr << "[Dummy Positioner Plugin] Cannot start - not connected" << std::endl;
        if (onError) {
            onError("Positioner not connected");
        }
        return;
    }
    
    if (m_isMoving) {
        std::cerr << "[Dummy Positioner Plugin] Already moving" << std::endl;
        return;
    }
    
    std::cout << "[Dummy Positioner Plugin] Starting movement..." << std::endl;
    std::cout << "  From position: AZ=" << m_currentAZ << " EL=" << m_currentEL << " POL=" << m_currentPOL << std::endl;
    
    m_isMoving = true;
    m_stepCount = 0;
    
    // Start movement thread
    m_movementThread = std::thread(&DummyPositioner::movementThread, this);
    
    if (onMovementStarted) {
        onMovementStarted();
    }
}

void DummyPositioner::stop()
{
    if (!m_isMoving) {
        std::cerr << "[Dummy Positioner Plugin] Not moving" << std::endl;
        return;
    }
    
    std::cout << "[Dummy Positioner Plugin] Stopping movement..." << std::endl;
    
    m_isMoving = false;
    
    // Wait for movement thread to finish
    if (m_movementThread.joinable()) {
        m_movementThread.join();
    }
    
    std::cout << "  Final position: AZ=" << m_currentAZ << " EL=" << m_currentEL << " POL=" << m_currentPOL << std::endl;
    std::cout << "  Steps taken: " << m_stepCount << std::endl;
    
    if (onMovementStopped) {
        onMovementStopped();
    }
}

void DummyPositioner::movementThread()
{
    while (m_isMoving) {
        // Update position based on movement and step
        double nextAZ = m_currentAZ + (m_currentMovement.AZ * m_step.AZ);
        double nextEL = m_currentEL + (m_currentMovement.EL * m_step.EL);
        double nextPOL = m_currentPOL + (m_currentMovement.POL * m_step.POL);
        
        // Check bounds
        if (nextAZ < m_minRange.AZ || nextAZ > m_maxRange.AZ) {
            std::cout << "[Dummy Positioner Plugin] AZ limit reached: " << nextAZ << std::endl;
            m_isMoving = false;
            break;
        }
        
        if (nextEL < m_minRange.EL || nextEL > m_maxRange.EL) {
            std::cout << "[Dummy Positioner Plugin] EL limit reached: " << nextEL << std::endl;
            m_isMoving = false;
            break;
        }
        
        if (nextPOL < m_minRange.POL || nextPOL > m_maxRange.POL) {
            std::cout << "[Dummy Positioner Plugin] POL limit reached: " << nextPOL << std::endl;
            m_isMoving = false;
            break;
        }
        
        // Update position
        m_currentAZ = nextAZ;
        m_currentEL = nextEL;
        m_currentPOL = nextPOL;
        m_stepCount++;
        
        // Emit position changed callback
        if (onPositionChanged) {
            onPositionChanged(m_currentAZ, m_currentEL, m_currentPOL);
        }
        
        // Check if we've moved the specified distance (for demo, stop after 50 steps)
        if (m_stepCount >= 50) {
            std::cout << "[Dummy Positioner Plugin] Movement completed (50 steps)" << std::endl;
            m_isMoving = false;
            break;
        }
        
        // Sleep for 100ms between steps
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
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
