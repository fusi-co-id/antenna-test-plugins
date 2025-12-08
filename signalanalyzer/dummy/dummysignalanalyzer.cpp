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

#include "dummysignalanalyzer.h"
#include <QDebug>
#include <QThread>
#include <QRandomGenerator>
#include <cmath>

DummySignalAnalyzer::DummySignalAnalyzer()
    : m_isConnected(false)
    , m_startFreqHz(5460.0e6)  // 5460 MHz default
    , m_stopFreqHz(5560.0e6)   // 5560 MHz default (100 MHz span)
    , m_rbwHz(1.0e6)           // 1 MHz default
    , m_connectedAddress("")
{
    qDebug() << "[Dummy SA Plugin] Instance created";
}

DummySignalAnalyzer::~DummySignalAnalyzer()
{
    if (m_isConnected) {
        disconnect();
    }
    qDebug() << "[Dummy SA Plugin] Instance destroyed";
}

QVector<DeviceInfo> DummySignalAnalyzer::scanDevices()
{
    qDebug() << "[Dummy SA Plugin] Scanning for devices...";
    
    QVector<DeviceInfo> devices;
    
    // Simulate finding devices
    QThread::msleep(200);
    
    // Simulate 2-3 LAN devices
    DeviceInfo device1;
    device1.name = "Dummy SA-1000";
    device1.serialNumber = "DSA-001234";
    device1.address = "192.168.1.100";
    device1.type = "LAN";
    device1.isAvailable = true;
    devices.append(device1);
    
    DeviceInfo device2;
    device2.name = "Dummy SA-2000";
    device2.serialNumber = "DSA-005678";
    device2.address = "192.168.1.101";
    device2.type = "LAN";
    device2.isAvailable = true;
    devices.append(device2);
    
    // Simulate 1 USB device
    DeviceInfo device3;
    device3.name = "Dummy SA-USB";
    device3.serialNumber = "DSA-USB-9012";
    device3.address = "USB0::0x1234::0x5678::DSA-USB-9012::INSTR";
    device3.type = "USB";
    device3.isAvailable = true;
    devices.append(device3);
    
    qDebug() << "[Dummy SA Plugin] Found" << devices.size() << "devices";
    
    emit devicesScanned(devices);
    return devices;
}

bool DummySignalAnalyzer::connectToDevice(const QString &address)
{
    if (m_isConnected) {
        qWarning() << "[Dummy SA Plugin] Already connected to" << m_connectedAddress;
        return false;
    }
    
    qDebug() << "[Dummy SA Plugin] Connecting to device at:" << address;
    
    // Simulate connection delay
    QThread::msleep(150);
    
    m_connectedAddress = address;
    m_isConnected = true;
    
    qDebug() << "[Dummy SA Plugin] Connected successfully to" << address;
    emit connected();
    return true;
}

bool DummySignalAnalyzer::connect()
{
    if (m_isConnected) {
        qWarning() << "[Dummy SA Plugin] Already connected";
        return true;
    }
    
    qDebug() << "[Dummy SA Plugin] Connecting to simulated instrument...";
    
    // Simulate connection delay
    QThread::msleep(100);
    
    m_isConnected = true;
    
    qDebug() << "[Dummy SA Plugin] Connected successfully";
    qDebug() << "  Device: Dummy Signal Analyzer v1.0";
    qDebug() << "  Start Freq:" << m_startFreqHz / 1e6 << "MHz";
    qDebug() << "  Stop Freq:" << m_stopFreqHz / 1e6 << "MHz";
    qDebug() << "  RBW:" << m_rbwHz / 1e6 << "MHz";
    
    emit connected();
    return true;
}

void DummySignalAnalyzer::disconnect()
{
    if (!m_isConnected) {
        qWarning() << "[Dummy SA Plugin] Not connected";
        return;
    }
    
    qDebug() << "[Dummy SA Plugin] Disconnecting from" << m_connectedAddress;
    
    m_isConnected = false;
    m_connectedAddress.clear();
    
    qDebug() << "[Dummy SA Plugin] Disconnected";
    emit disconnected();
}

bool DummySignalAnalyzer::isConnected() const
{
    return m_isConnected;
}

void DummySignalAnalyzer::setStartFreq(double freqHz)
{
    m_startFreqHz = freqHz;
    qDebug() << "[Dummy SA Plugin] Start Freq set to" << freqHz / 1e6 << "MHz";
}

void DummySignalAnalyzer::setStopFreq(double freqHz)
{
    m_stopFreqHz = freqHz;
    qDebug() << "[Dummy SA Plugin] Stop Freq set to" << freqHz / 1e6 << "MHz";
}

void DummySignalAnalyzer::setRBW(double freqHz)
{
    m_rbwHz = freqHz;
    qDebug() << "[Dummy SA Plugin] RBW set to" << freqHz / 1e6 << "MHz";
}

Peak DummySignalAnalyzer::findPeak()
{
    Peak peak;
    
    if (!m_isConnected) {
        qWarning() << "[Dummy SA Plugin] Cannot find peak - not connected";
        emit errorOccurred("Signal Analyzer not connected");
        return peak;
    }
    
    // Simulate measurement with realistic behavior
    // Generate a peak somewhere in the frequency range with some randomness
    double freqRange = m_stopFreqHz - m_startFreqHz;
    double centerFreq = m_startFreqHz + freqRange / 2.0;
    
    // Add some randomness around center (simulate slight variations)
    double randomOffset = (QRandomGenerator::global()->generateDouble() - 0.5) * freqRange * 0.2;
    peak.frequencyHz = centerFreq + randomOffset;
    
    // Generate realistic power level with some noise
    // Base level around -50 dBm with ±10 dB variation
    peak.leveldBm = -50.0 + (QRandomGenerator::global()->generateDouble() - 0.5) * 20.0;
    
    qDebug() << "[Dummy SA Plugin] Peak found at" 
             << peak.frequencyHz / 1e6 << "MHz,"
             << peak.leveldBm << "dBm";
    
    emit peakFound(peak);
    return peak;
}

// Factory functions for plugin loading
extern "C" {
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    ISignalAnalyzerPlugin* createSignalAnalyzerPlugin()
    {
        qDebug() << "[Dummy SA Plugin] Factory: Creating plugin instance";
        return new DummySignalAnalyzer();
    }
    
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    void destroyPlugin(QObject* plugin)
    {
        qDebug() << "[Dummy SA Plugin] Factory: Destroying plugin instance";
        delete plugin;
    }
}
