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
#include <iostream>
#include <thread>
#include <chrono>

DummySignalGenerator::DummySignalGenerator()
    : m_isConnected(false)
    , m_rfEnabled(false)
    , m_freqHz(5510.0e6)  // 5510 MHz default
    , m_powerDbm(0.0)     // 0 dBm default
    , m_connectedAddress("")
{
    std::cout << "[Dummy SG Plugin] Instance created" << std::endl;
}

std::vector<DeviceInfo> DummySignalGenerator::scanDevices()
{
    std::cout << "[Dummy SG Plugin] Scanning for devices..." << std::endl;
    
    std::vector<DeviceInfo> devices;
    
    // Simulate finding devices
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Simulate 2 LAN devices
    DeviceInfo device1;
    device1.name = "Dummy SG-3000";
    device1.serialNumber = "DSG-001122";
    device1.address = "192.168.1.110";
    device1.type = "LAN";
    device1.isAvailable = true;
    devices.push_back(device1);
    
    DeviceInfo device2;
    device2.name = "Dummy SG-5000";
    device2.serialNumber = "DSG-003344";
    device2.address = "192.168.1.111";
    device2.type = "LAN";
    device2.isAvailable = true;
    devices.push_back(device2);
    
    // Simulate 1 USB device
    DeviceInfo device3;
    device3.name = "Dummy SG-USB";
    device3.serialNumber = "DSG-USB-7890";
    device3.address = "USB0::0x5678::0x1234::DSG-USB-7890::INSTR";
    device3.type = "USB";
    device3.isAvailable = true;
    devices.push_back(device3);
    
    std::cout << "[Dummy SG Plugin] Found " << devices.size() << " devices" << std::endl;
    
    if (onDevicesScanned) {
        onDevicesScanned(devices);
    }
    return devices;
}

bool DummySignalGenerator::connectToDevice(const std::string &address)
{
    if (m_isConnected) {
        std::cerr << "[Dummy SG Plugin] Already connected to " << m_connectedAddress << std::endl;
        return false;
    }
    
    std::cout << "[Dummy SG Plugin] Connecting to device at: " << address << std::endl;
    
    // Simulate connection delay
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    
    m_connectedAddress = address;
    m_isConnected = true;
    
    std::cout << "[Dummy SG Plugin] Connected successfully to " << address << std::endl;
    if (onConnected) {
        onConnected();
    }
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
    std::cout << "[Dummy SG Plugin] Instance destroyed" << std::endl;
}

bool DummySignalGenerator::connect()
{
    if (m_isConnected) {
        std::cerr << "[Dummy SG Plugin] Already connected" << std::endl;
        return true;
    }
    
    std::cout << "[Dummy SG Plugin] Connecting to simulated instrument..." << std::endl;
    
    // Simulate connection delay
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    m_isConnected = true;
    
    std::cout << "[Dummy SG Plugin] Connected successfully" << std::endl;
    std::cout << "  Device: Dummy Signal Generator v1.0" << std::endl;
    std::cout << "  Frequency: " << m_freqHz / 1e6 << " MHz" << std::endl;
    std::cout << "  Power Level: " << m_powerDbm << " dBm" << std::endl;
    std::cout << "  RF Output: " << (m_rfEnabled ? "ON" : "OFF") << std::endl;
    
    if (onConnected) {
        onConnected();
    }
    return true;
}

void DummySignalGenerator::disconnect()
{
    if (!m_isConnected) {
        std::cerr << "[Dummy SG Plugin] Not connected" << std::endl;
        return;
    }
    
    // Turn off RF before disconnecting
    if (m_rfEnabled) {
        disableRf();
    }
    
    std::cout << "[Dummy SG Plugin] Disconnecting from " << m_connectedAddress << std::endl;
    
    m_isConnected = false;
    m_connectedAddress.clear();
    
    std::cout << "[Dummy SG Plugin] Disconnected" << std::endl;
    if (onDisconnected) {
        onDisconnected();
    }
}

bool DummySignalGenerator::isConnected() const
{
    return m_isConnected;
}

void DummySignalGenerator::setFreq(double freqHz)
{
    m_freqHz = freqHz;
    std::cout << "[Dummy SG Plugin] Frequency set to " << freqHz / 1e6 << " MHz" << std::endl;
}

void DummySignalGenerator::setPower(double powerDbm)
{
    m_powerDbm = powerDbm;
    std::cout << "[Dummy SG Plugin] Power level set to " << powerDbm << " dBm" << std::endl;
}

void DummySignalGenerator::enableRf()
{
    if (!m_isConnected) {
        std::cerr << "[Dummy SG Plugin] Cannot enable RF - not connected" << std::endl;
        if (onError) {
            onError("Signal Generator not connected");
        }
        return;
    }
    
    if (m_rfEnabled) {
        std::cout << "[Dummy SG Plugin] RF already enabled" << std::endl;
        return;
    }
    
    std::cout << "[Dummy SG Plugin] Enabling RF output..." << std::endl;
    std::cout << "  Frequency: " << m_freqHz / 1e6 << " MHz" << std::endl;
    std::cout << "  Power Level: " << m_powerDbm << " dBm" << std::endl;
    
    // Simulate RF enable delay
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    m_rfEnabled = true;
    
    std::cout << "[Dummy SG Plugin] RF output ENABLED" << std::endl;
    if (onRfEnabled) {
        onRfEnabled();
    }
}

void DummySignalGenerator::disableRf()
{
    if (!m_isConnected) {
        std::cerr << "[Dummy SG Plugin] Cannot disable RF - not connected" << std::endl;
        return;
    }
    
    if (!m_rfEnabled) {
        std::cout << "[Dummy SG Plugin] RF already disabled" << std::endl;
        return;
    }
    
    std::cout << "[Dummy SG Plugin] Disabling RF output..." << std::endl;
    
    // Simulate RF disable delay
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    m_rfEnabled = false;
    
    std::cout << "[Dummy SG Plugin] RF output DISABLED" << std::endl;
    if (onRfDisabled) {
        onRfDisabled();
    }
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
        std::cout << "[Dummy SG Plugin] Factory: Creating plugin instance" << std::endl;
        return new DummySignalGenerator();
    }
    
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    void destroyPlugin(void* plugin)
    {
        std::cout << "[Dummy SG Plugin] Factory: Destroying plugin instance" << std::endl;
        delete static_cast<ISignalGeneratorPlugin*>(plugin);
    }
}
