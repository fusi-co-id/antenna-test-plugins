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
#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>

DummySignalAnalyzer::DummySignalAnalyzer()
    : m_isConnected(false)
    , m_startFreqHz(5460.0e6)  // 5460 MHz default
    , m_stopFreqHz(5560.0e6)   // 5560 MHz default (100 MHz span)
    , m_rbwHz(1.0e6)           // 1 MHz default
    , m_connectedAddress("")
{
    // Initialize random generator with current time
    m_randomGenerator.seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::cout << "[Dummy SA Plugin] Instance created" << std::endl;
}

DummySignalAnalyzer::~DummySignalAnalyzer()
{
    if (m_isConnected) {
        disconnect();
    }
    std::cout << "[Dummy SA Plugin] Instance destroyed" << std::endl;
}

std::vector<DeviceInfo> DummySignalAnalyzer::scanDevices()
{
    std::cout << "[Dummy SA Plugin] Scanning for devices..." << std::endl;
    
    std::vector<DeviceInfo> devices;
    
    // Simulate finding devices
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Simulate 2-3 LAN devices
    DeviceInfo device1;
    device1.name = "Dummy SA-1000";
    device1.serialNumber = "DSA-1000";
    device1.address = "192.168.1.100";
    device1.type = "LAN";
    device1.isAvailable = true;
    devices.push_back(device1);
    
    // DeviceInfo device2;
    // device2.name = "Dummy SA-2000";
    // device2.serialNumber = "DSA-2000";
    // device2.address = "192.168.1.101";
    // device2.type = "LAN";
    // device2.isAvailable = true;
    // devices.push_back(device2);
    
    // // Simulate 1 USB device
    // DeviceInfo device3;
    // device3.name = "Dummy SA-USB";
    // device3.serialNumber = "DSA-USB-9012";
    // device3.address = "USB0::0x1234::0x5678::DSA-USB-9012::INSTR";
    // device3.type = "USB";
    // device3.isAvailable = true;
    // devices.push_back(device3);
    
    std::cout << "[Dummy SA Plugin] Found " << devices.size() << " devices" << std::endl;
    
    if (onDevicesScanned) {
        onDevicesScanned(devices);
    }
    return devices;
}

bool DummySignalAnalyzer::connectToDevice(const std::string &address)
{
    if (m_isConnected) {
        std::cerr << "[Dummy SA Plugin] Already connected to " << m_connectedAddress << std::endl;
        return false;
    }
    
    std::cout << "[Dummy SA Plugin] Connecting to device at: " << address << std::endl;
    
    // Simulate connection delay
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    
    m_connectedAddress = address;
    m_isConnected = true;
    
    std::cout << "[Dummy SA Plugin] Connected successfully to " << address << std::endl;
    if (onConnected) {
        onConnected();
    }
    return true;
}

bool DummySignalAnalyzer::connect()
{
    if (m_isConnected) {
        std::cerr << "[Dummy SA Plugin] Already connected" << std::endl;
        return true;
    }
    
    std::cout << "[Dummy SA Plugin] Connecting to simulated instrument..." << std::endl;
    
    // Simulate connection delay
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    m_isConnected = true;
    
    std::cout << "[Dummy SA Plugin] Connected successfully" << std::endl;
    std::cout << "  Device: Dummy Signal Analyzer v1.0" << std::endl;
    std::cout << "  Start Freq: " << m_startFreqHz / 1e6 << " MHz" << std::endl;
    std::cout << "  Stop Freq: " << m_stopFreqHz / 1e6 << " MHz" << std::endl;
    std::cout << "  RBW: " << m_rbwHz / 1e6 << " MHz" << std::endl;
    
    if (onConnected) {
        onConnected();
    }
    return true;
}

void DummySignalAnalyzer::disconnect()
{
    if (!m_isConnected) {
        std::cerr << "[Dummy SA Plugin] Not connected" << std::endl;
        return;
    }
    
    std::cout << "[Dummy SA Plugin] Disconnecting from " << m_connectedAddress << std::endl;
    
    m_isConnected = false;
    m_connectedAddress.clear();
    
    std::cout << "[Dummy SA Plugin] Disconnected" << std::endl;
    if (onDisconnected) {
        onDisconnected();
    }
}

bool DummySignalAnalyzer::isConnected() const
{
    return m_isConnected;
}

void DummySignalAnalyzer::setStartFreq(double freqHz)
{
    m_startFreqHz = freqHz;
    std::cout << "[Dummy SA Plugin] Start Freq set to " << freqHz / 1e6 << " MHz" << std::endl;
}

void DummySignalAnalyzer::setStopFreq(double freqHz)
{
    m_stopFreqHz = freqHz;
    std::cout << "[Dummy SA Plugin] Stop Freq set to " << freqHz / 1e6 << " MHz" << std::endl;
}

void DummySignalAnalyzer::setRBW(double freqHz)
{
    m_rbwHz = freqHz;
    std::cout << "[Dummy SA Plugin] RBW set to " << freqHz / 1e6 << " MHz" << std::endl;
}

Peak DummySignalAnalyzer::findPeak()
{
    Peak peak;
    peak.frequencyHz = 0.0;
    peak.leveldBm = -100.0;
    
    if (!m_isConnected) {
        std::cerr << "[Dummy SA Plugin] Cannot find peak - not connected" << std::endl;
        if (onError) {
            onError("Signal Analyzer not connected");
        }
        return peak;
    }
    
    // Simulate measurement with realistic behavior
    // Generate a peak somewhere in the frequency range with some randomness
    double freqRange = m_stopFreqHz - m_startFreqHz;
    double centerFreq = m_startFreqHz + freqRange / 2.0;
    
    // Add some randomness around center (simulate slight variations)
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double randomOffset = (dist(m_randomGenerator) - 0.5) * freqRange * 0.2;
    peak.frequencyHz = centerFreq + randomOffset;
    
    // Generate realistic power level with some noise
    // Base level around -50 dBm with ±10 dB variation
    peak.leveldBm = -50.0 + (dist(m_randomGenerator) - 0.5) * 20.0;
    
    std::cout << "[Dummy SA Plugin] Peak found at " 
             << peak.frequencyHz / 1e6 << " MHz, "
             << peak.leveldBm << " dBm" << std::endl;
    
    if (onPeakFound) {
        onPeakFound(peak);
    }
    return peak;
}

// Factory functions for plugin loading
extern "C" {
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    ISignalAnalyzerPlugin* createSignalAnalyzerPlugin()
    {
        std::cout << "[Dummy SA Plugin] Factory: Creating plugin instance" << std::endl;
        return new DummySignalAnalyzer();
    }
    
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    void destroyPlugin(void* plugin)
    {
        std::cout << "[Dummy SA Plugin] Factory: Destroying plugin instance" << std::endl;
        delete static_cast<ISignalAnalyzerPlugin*>(plugin);
    }
}
