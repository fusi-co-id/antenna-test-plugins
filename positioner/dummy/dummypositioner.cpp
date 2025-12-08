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
#include <QDebug>
#include <QThread>
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
    
    // Create timer for simulating movement
    m_timer = new QTimer(this);
    QObject::connect(m_timer, &QTimer::timeout, this, &DummyPositioner::onTimerTick);
    
    qDebug() << "[Dummy Positioner Plugin] Instance created";
}

QVector<DeviceInfo> DummyPositioner::scanDevices()
{
    qDebug() << "[Dummy Positioner Plugin] Scanning for devices...";
    
    QVector<DeviceInfo> devices;
    
    // Simulate finding devices
    QThread::msleep(200);
    
    // Simulate 1 LAN device
    DeviceInfo device1;
    device1.name = "Dummy Positioner-AZ/EL";
    device1.serialNumber = "DPS-002468";
    device1.address = "192.168.1.120";
    device1.type = "LAN";
    device1.isAvailable = true;
    devices.append(device1);
    
    // Simulate 1 Serial device
    DeviceInfo device2;
    device2.name = "Dummy Positioner-6DOF";
    device2.serialNumber = "DPS-SER-1357";
    device2.address = "COM3";
    device2.type = "Serial";
    device2.isAvailable = true;
    devices.append(device2);
    
    qDebug() << "[Dummy Positioner Plugin] Found" << devices.size() << "devices";
    
    emit devicesScanned(devices);
    return devices;
}

bool DummyPositioner::connectToDevice(const QString &address)
{
    if (m_isConnected) {
        qWarning() << "[Dummy Positioner Plugin] Already connected to" << m_connectedAddress;
        return false;
    }
    
    qDebug() << "[Dummy Positioner Plugin] Connecting to device at:" << address;
    
    // Simulate connection delay
    QThread::msleep(150);
    
    m_connectedAddress = address;
    m_isConnected = true;
    
    qDebug() << "[Dummy Positioner Plugin] Connected successfully to" << address;
    emit connected();
    return true;
}

DummyPositioner::~DummyPositioner()
{
    if (m_isConnected) {
        disconnect();
    }
    qDebug() << "[Dummy Positioner Plugin] Instance destroyed";
}

bool DummyPositioner::connect()
{
    if (m_isConnected) {
        qWarning() << "[Dummy Positioner Plugin] Already connected";
        return true;
    }
    
    qDebug() << "[Dummy Positioner Plugin] Connecting to simulated positioner...";
    
    // Simulate connection delay
    QThread::msleep(100);
    
    m_isConnected = true;
    
    qDebug() << "[Dummy Positioner Plugin] Connected successfully";
    qDebug() << "  Device: Dummy Positioner v2.0";
    qDebug() << "  AZ Range:" << m_minRange.AZ << "to" << m_maxRange.AZ << "degrees";
    qDebug() << "  EL Range:" << m_minRange.EL << "to" << m_maxRange.EL << "degrees";
    qDebug() << "  POL Range:" << m_minRange.POL << "to" << m_maxRange.POL << "degrees";
    
    emit connected();
    return true;
}

void DummyPositioner::disconnect()
{
    if (!m_isConnected) {
        qWarning() << "[Dummy Positioner Plugin] Not connected";
        return;
    }
    
    if (m_isMoving) {
        stop();
    }
    
    qDebug() << "[Dummy Positioner Plugin] Disconnecting from" << m_connectedAddress;
    
    m_isConnected = false;
    m_connectedAddress.clear();
    
    qDebug() << "[Dummy Positioner Plugin] Disconnected";
    emit disconnected();
}

bool DummyPositioner::isConnected() const
{
    return m_isConnected;
}

void DummyPositioner::setAZStep(double step)
{
    m_step.AZ = step;
    qDebug() << "[Dummy Positioner Plugin] AZ Step set to" << step << "degrees";
}

void DummyPositioner::setStep(const Step &step)
{
    m_step = step;
    qDebug() << "[Dummy Positioner Plugin] Step set:";
    qDebug() << "  AZ:" << step.AZ << "EL:" << step.EL << "POL:" << step.POL;
    qDebug() << "  X:" << step.X << "Y:" << step.Y << "V:" << step.V;
}

void DummyPositioner::setMinRange(const MinRange &minRange)
{
    m_minRange = minRange;
    qDebug() << "[Dummy Positioner Plugin] Min Range set:";
    qDebug() << "  AZ:" << minRange.AZ << "EL:" << minRange.EL << "POL:" << minRange.POL;
}

void DummyPositioner::setMaxRange(const MaxRange &maxRange)
{
    m_maxRange = maxRange;
    qDebug() << "[Dummy Positioner Plugin] Max Range set:";
    qDebug() << "  AZ:" << maxRange.AZ << "EL:" << maxRange.EL << "POL:" << maxRange.POL;
}

void DummyPositioner::setMovement(const Movement &movement)
{
    m_currentMovement = movement;
    qDebug() << "[Dummy Positioner Plugin] Movement set:";
    qDebug() << "  AZ:" << movement.AZ << "EL:" << movement.EL << "POL:" << movement.POL;
    qDebug() << "  X:" << movement.X << "Y:" << movement.Y << "V:" << movement.V;
}

void DummyPositioner::setDistance(double distance)
{
    m_distance = distance;
    qDebug() << "[Dummy Positioner Plugin] Distance set to" << distance;
}

void DummyPositioner::start()
{
    if (!m_isConnected) {
        qWarning() << "[Dummy Positioner Plugin] Cannot start - not connected";
        emit errorOccurred("Positioner not connected");
        return;
    }
    
    if (m_isMoving) {
        qWarning() << "[Dummy Positioner Plugin] Already moving";
        return;
    }
    
    qDebug() << "[Dummy Positioner Plugin] Starting movement...";
    qDebug() << "  From position: AZ=" << m_currentAZ << "EL=" << m_currentEL << "POL=" << m_currentPOL;
    
    m_isMoving = true;
    m_stepCount = 0;
    
    // Start timer to simulate movement (update every 100ms)
    m_timer->start(100);
    
    emit movementStarted();
}

void DummyPositioner::stop()
{
    if (!m_isMoving) {
        qWarning() << "[Dummy Positioner Plugin] Not moving";
        return;
    }
    
    qDebug() << "[Dummy Positioner Plugin] Stopping movement...";
    
    m_timer->stop();
    m_isMoving = false;
    
    qDebug() << "  Final position: AZ=" << m_currentAZ << "EL=" << m_currentEL << "POL=" << m_currentPOL;
    qDebug() << "  Steps taken:" << m_stepCount;
    
    emit movementStopped();
}

void DummyPositioner::onTimerTick()
{
    if (!m_isMoving) {
        return;
    }
    
    // Update position based on movement and step
    double nextAZ = m_currentAZ + (m_currentMovement.AZ * m_step.AZ);
    double nextEL = m_currentEL + (m_currentMovement.EL * m_step.EL);
    double nextPOL = m_currentPOL + (m_currentMovement.POL * m_step.POL);
    
    // Check bounds
    if (nextAZ < m_minRange.AZ || nextAZ > m_maxRange.AZ) {
        qDebug() << "[Dummy Positioner Plugin] AZ limit reached:" << nextAZ;
        stop();
        return;
    }
    
    if (nextEL < m_minRange.EL || nextEL > m_maxRange.EL) {
        qDebug() << "[Dummy Positioner Plugin] EL limit reached:" << nextEL;
        stop();
        return;
    }
    
    if (nextPOL < m_minRange.POL || nextPOL > m_maxRange.POL) {
        qDebug() << "[Dummy Positioner Plugin] POL limit reached:" << nextPOL;
        stop();
        return;
    }
    
    // Update position
    m_currentAZ = nextAZ;
    m_currentEL = nextEL;
    m_currentPOL = nextPOL;
    m_stepCount++;
    
    // Emit position changed
    emit positionChanged(m_currentAZ, m_currentEL, m_currentPOL);
    
    // Check if we've moved the specified distance (for demo, stop after 50 steps)
    if (m_stepCount >= 50) {
        qDebug() << "[Dummy Positioner Plugin] Movement completed (50 steps)";
        stop();
    }
}

// Factory function to create plugin instance
extern "C" Q_DECL_EXPORT IPositionerPlugin* createPositionerPlugin()
{
    return new DummyPositioner();
}
