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

#include "dummysignalgenerator.h"
#include <QDebug>
#include <QThread>

DummySignalGenerator::DummySignalGenerator()
    : m_isConnected(false)
    , m_rfEnabled(false)
    , m_freqHz(5510.0e6)  // 5510 MHz default
    , m_powerDbm(0.0)     // 0 dBm default
    , m_connectedAddress("")
{
    qDebug() << "[Dummy SG Plugin] Instance created";
}

QVector<DeviceInfo> DummySignalGenerator::scanDevices()
{
    qDebug() << "[Dummy SG Plugin] Scanning for devices...";
    
    QVector<DeviceInfo> devices;
    
    // Simulate finding devices
    QThread::msleep(200);
    
    // Simulate 2 LAN devices
    DeviceInfo device1;
    device1.name = "Dummy SG-3000";
    device1.serialNumber = "DSG-001122";
    device1.address = "192.168.1.110";
    device1.type = "LAN";
    device1.isAvailable = true;
    devices.append(device1);
    
    DeviceInfo device2;
    device2.name = "Dummy SG-5000";
    device2.serialNumber = "DSG-003344";
    device2.address = "192.168.1.111";
    device2.type = "LAN";
    device2.isAvailable = true;
    devices.append(device2);
    
    // Simulate 1 USB device
    DeviceInfo device3;
    device3.name = "Dummy SG-USB";
    device3.serialNumber = "DSG-USB-7890";
    device3.address = "USB0::0x5678::0x1234::DSG-USB-7890::INSTR";
    device3.type = "USB";
    device3.isAvailable = true;
    devices.append(device3);
    
    qDebug() << "[Dummy SG Plugin] Found" << devices.size() << "devices";
    
    emit devicesScanned(devices);
    return devices;
}

bool DummySignalGenerator::connectToDevice(const QString &address)
{
    if (m_isConnected) {
        qWarning() << "[Dummy SG Plugin] Already connected to" << m_connectedAddress;
        return false;
    }
    
    qDebug() << "[Dummy SG Plugin] Connecting to device at:" << address;
    
    // Simulate connection delay
    QThread::msleep(150);
    
    m_connectedAddress = address;
    m_isConnected = true;
    
    qDebug() << "[Dummy SG Plugin] Connected successfully to" << address;
    emit connected();
    return true;
}

DummySignalGenerator::~DummySignalGenerator()
{
    if (m_isConnected) {
        if (m_rfEnabled) {
            disableRf();
        }
        disconnect();
    }
    qDebug() << "[Dummy SG Plugin] Instance destroyed";
}

bool DummySignalGenerator::connect()
{
    if (m_isConnected) {
        qWarning() << "[Dummy SG Plugin] Already connected";
        return true;
    }
    
    qDebug() << "[Dummy SG Plugin] Connecting to simulated instrument...";
    
    // Simulate connection delay
    QThread::msleep(100);
    
    m_isConnected = true;
    
    qDebug() << "[Dummy SG Plugin] Connected successfully";
    qDebug() << "  Device: Dummy Signal Generator v1.0";
    qDebug() << "  Frequency:" << m_freqHz / 1e6 << "MHz";
    qDebug() << "  Power Level:" << m_powerDbm << "dBm";
    qDebug() << "  RF Output:" << (m_rfEnabled ? "ON" : "OFF");
    
    emit connected();
    return true;
}

void DummySignalGenerator::disconnect()
{
    if (!m_isConnected) {
        qWarning() << "[Dummy SG Plugin] Not connected";
        return;
    }
    
    // Turn off RF before disconnecting
    if (m_rfEnabled) {
        disableRf();
    }
    
    qDebug() << "[Dummy SG Plugin] Disconnecting from" << m_connectedAddress;
    
    m_isConnected = false;
    m_connectedAddress.clear();
    
    qDebug() << "[Dummy SG Plugin] Disconnected";
    emit disconnected();
}

bool DummySignalGenerator::isConnected() const
{
    return m_isConnected;
}

void DummySignalGenerator::setFreq(double freqHz)
{
    m_freqHz = freqHz;
    qDebug() << "[Dummy SG Plugin] Frequency set to" << freqHz / 1e6 << "MHz";
}

void DummySignalGenerator::setPower(double powerDbm)
{
    m_powerDbm = powerDbm;
    qDebug() << "[Dummy SG Plugin] Power level set to" << powerDbm << "dBm";
}

void DummySignalGenerator::enableRf()
{
    if (!m_isConnected) {
        qWarning() << "[Dummy SG Plugin] Cannot enable RF - not connected";
        emit errorOccurred("Signal Generator not connected");
        return;
    }
    
    if (m_rfEnabled) {
        qDebug() << "[Dummy SG Plugin] RF already enabled";
        return;
    }
    
    qDebug() << "[Dummy SG Plugin] Enabling RF output...";
    qDebug() << "  Frequency:" << m_freqHz / 1e6 << "MHz";
    qDebug() << "  Power Level:" << m_powerDbm << "dBm";
    
    // Simulate RF enable delay
    QThread::msleep(50);
    
    m_rfEnabled = true;
    
    qDebug() << "[Dummy SG Plugin] RF output ENABLED";
    emit rfEnabled();
}

void DummySignalGenerator::disableRf()
{
    if (!m_isConnected) {
        qWarning() << "[Dummy SG Plugin] Cannot disable RF - not connected";
        return;
    }
    
    if (!m_rfEnabled) {
        qDebug() << "[Dummy SG Plugin] RF already disabled";
        return;
    }
    
    qDebug() << "[Dummy SG Plugin] Disabling RF output...";
    
    // Simulate RF disable delay
    QThread::msleep(50);
    
    m_rfEnabled = false;
    
    qDebug() << "[Dummy SG Plugin] RF output DISABLED";
    emit rfDisabled();
}

bool DummySignalGenerator::isRfEnabled() const
{
    return m_rfEnabled;
}

// Factory functions for plugin loading
extern "C" {
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    ISignalGeneratorPlugin* createSignalGeneratorPlugin()
    {
        qDebug() << "[Dummy SG Plugin] Factory: Creating plugin instance";
        return new DummySignalGenerator();
    }
    
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    void destroyPlugin(QObject* plugin)
    {
        qDebug() << "[Dummy SG Plugin] Factory: Destroying plugin instance";
        delete plugin;
    }
}
