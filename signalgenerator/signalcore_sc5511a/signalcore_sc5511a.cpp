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

#include "signalcore_sc5511a.h"
#include <iostream>
#include <thread>
#include <chrono>

// parameters to work with the USB device(s)
#define MAXDEVICES 50

SignalCoreSC5511A::SignalCoreSC5511A()
    : m_isConnected(false)
    , m_rfEnabled(false)
    , m_freqHz(5510.0e6)  // 5510 MHz default
    , m_powerDbm(0.0)     // 0 dBm default
    , m_connectedAddress("")
    , dev_handle(NULL)
    , num_of_devices(0)
    , status(0)
{
    // Allocate memory for device list
    device_list = (char**)malloc(sizeof(char*)*MAXDEVICES);
    for (int i=0; i<MAXDEVICES; i++)
        device_list[i] = (char*)malloc(sizeof(char)*SCI_SN_LENGTH);
    
    std::cout << "[SignalCoreSC5511A Plugin] Instance created" << std::endl;
}

std::vector<DeviceInfo> SignalCoreSC5511A::scanDevices()
{
    std::cout << "[SignalCoreSC5511A Plugin] Scanning for devices..." << std::endl;
    
    std::vector<DeviceInfo> devices;
    
    // Search for devices using sc5511a API
    num_of_devices = sc5511a_search_devices(device_list);
    
    if (num_of_devices == 0) {
        std::cout << "[SignalCoreSC5511A Plugin] No signal core devices found" << std::endl;
    } else {
        std::cout << "[SignalCoreSC5511A Plugin] There are " << num_of_devices << " SignalCore " << SCI_PRODUCT_NAME << " USB devices found." << std::endl;
        
        // Convert device_list to DeviceInfo vector
        for (int i = 0; i < num_of_devices; i++) {
            DeviceInfo device;
            device.name = std::string(SCI_PRODUCT_NAME);
            device.serialNumber = std::string(device_list[i]);
            device.address = std::string(device_list[i]); // Use serial number as address
            device.type = "USB";
            device.isAvailable = true;
            devices.push_back(device);
            
            std::cout << "  Device " << (i+1) << " has Serial Number: " << device_list[i] << std::endl;
        }
    }
    
    if (onDevicesScanned) {
        onDevicesScanned(devices);
    }
    return devices;
}

bool SignalCoreSC5511A::connectToDevice(const std::string &address)
{
    if (m_isConnected) {
        std::cerr << "[SignalCoreSC5511A Plugin] Already connected to " << m_connectedAddress << std::endl;
        return false;
    }
    
    std::cout << "[SignalCoreSC5511A Plugin] Connecting to device at: " << address << std::endl;
    
    // Open the device using serial number (address)
    // Need to copy to non-const char* for sc5511a API
    char* serial_number = new char[address.length() + 1];
    strcpy(serial_number, address.c_str());
    
    dev_handle = sc5511a_open_device(serial_number);
    delete[] serial_number;
    
    if (dev_handle == NULL) {
        std::cerr << "[SignalCoreSC5511A Plugin] Device with serial number: " << address << " cannot be opened." << std::endl;
        std::cerr << "[SignalCoreSC5511A Plugin] Please ensure your device is powered on and connected" << std::endl;
        if (onError) {
            onError("Cannot open device: " + address);
        }
        return false;
    }
    
    m_connectedAddress = address;
    m_isConnected = true;
    
    // Disable Sweep/List Mode (set to single tone mode)
    status = sc5511a_set_rf_mode(dev_handle, 0);
    if (status != SUCCESS) {
        std::cerr << "[SignalCoreSC5511A Plugin] Failed to set RF mode" << std::endl;
    }
    
    std::cout << "[SignalCoreSC5511A Plugin] Connected successfully to " << address << std::endl;
    if (onConnected) {
        onConnected();
    }
    return true;
}

SignalCoreSC5511A::~SignalCoreSC5511A()
{
    if (m_isConnected) {
        if (m_rfEnabled) {
            disableRf();
        }
        disconnect();
    }
    
    // Free device list memory
    if (device_list != NULL) {
        for (int i = 0; i < MAXDEVICES; i++) {
            if (device_list[i] != NULL) {
                free(device_list[i]);
            }
        }
        free(device_list);
    }
    
    std::cout << "[SignalCoreSC5511A Plugin] Instance destroyed" << std::endl;
}

bool SignalCoreSC5511A::connect()
{
    if (m_isConnected) {
        std::cerr << "[SignalCoreSC5511A Plugin] Already connected" << std::endl;
        return true;
    }
    
    std::cout << "[SignalCoreSC5511A Plugin] Connecting to simulated instrument..." << std::endl;
    
    // Simulate connection delay
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    m_isConnected = true;
    
    std::cout << "[SignalCoreSC5511A Plugin] Connected successfully" << std::endl;
    std::cout << "  Device: Dummy Signal Generator v1.0" << std::endl;
    std::cout << "  Frequency: " << m_freqHz / 1e6 << " MHz" << std::endl;
    std::cout << "  Power Level: " << m_powerDbm << " dBm" << std::endl;
    std::cout << "  RF Output: " << (m_rfEnabled ? "ON" : "OFF") << std::endl;
    
    if (onConnected) {
        onConnected();
    }
    return true;
}

void SignalCoreSC5511A::disconnect()
{
    if (!m_isConnected) {
        std::cerr << "[SignalCoreSC5511A Plugin] Not connected" << std::endl;
        return;
    }
    
    // Turn off RF before disconnecting
    if (m_rfEnabled) {
        disableRf();
    }
    
    std::cout << "[SignalCoreSC5511A Plugin] Disconnecting from " << m_connectedAddress << std::endl;
    
    // Close the device using sc5511a API
    if (dev_handle != NULL) {
        sc5511a_close_device(dev_handle);
        dev_handle = NULL;
    }
    
    m_isConnected = false;
    m_connectedAddress.clear();
    
    std::cout << "[SignalCoreSC5511A Plugin] Disconnected" << std::endl;
    if (onDisconnected) {
        onDisconnected();
    }
}

bool SignalCoreSC5511A::isConnected() const
{
    return m_isConnected;
}

void SignalCoreSC5511A::setFreq(double freqHz)
{
    m_freqHz = freqHz;
    
    if (m_isConnected && dev_handle != NULL) {
        unsigned long long int rf_freq = (unsigned long long int)freqHz;
        status = sc5511a_set_freq(dev_handle, rf_freq);
        if (status != SUCCESS) {
            std::cerr << "[SignalCoreSC5511A Plugin] Failed to set frequency" << std::endl;
            if (onError) {
                onError("Failed to set frequency");
            }
        } else {
            std::cout << "[SignalCoreSC5511A Plugin] Frequency set to " << freqHz / 1e6 << " MHz" << std::endl;
        }
    } else {
        std::cout << "[SignalCoreSC5511A Plugin] Frequency cached (not connected): " << freqHz / 1e6 << " MHz" << std::endl;
    }
}

void SignalCoreSC5511A::setPower(double powerDbm)
{
    m_powerDbm = powerDbm;
    
    if (m_isConnected && dev_handle != NULL) {
        float rf_level = (float)powerDbm;
        status = sc5511a_set_level(dev_handle, rf_level);
        if (status != SUCCESS) {
            std::cerr << "[SignalCoreSC5511A Plugin] Failed to set power level" << std::endl;
            if (onError) {
                onError("Failed to set power level");
            }
        } else {
            std::cout << "[SignalCoreSC5511A Plugin] Power level set to " << powerDbm << " dBm" << std::endl;
        }
    } else {
        std::cout << "[SignalCoreSC5511A Plugin] Power level cached (not connected): " << powerDbm << " dBm" << std::endl;
    }
}

void SignalCoreSC5511A::enableRf()
{
    if (!m_isConnected) {
        std::cerr << "[SignalCoreSC5511A Plugin] Cannot enable RF - not connected" << std::endl;
        if (onError) {
            onError("Signal Generator not connected");
        }
        return;
    }
    
    if (m_rfEnabled) {
        std::cout << "[SignalCoreSC5511A Plugin] RF already enabled" << std::endl;
        return;
    }
    
    std::cout << "[SignalCoreSC5511A Plugin] Enabling RF output..." << std::endl;
    std::cout << "  Frequency: " << m_freqHz / 1e6 << " MHz" << std::endl;
    std::cout << "  Power Level: " << m_powerDbm << " dBm" << std::endl;
    
    // Enable RF1 output using sc5511a API
    if (dev_handle != NULL) {
        status = sc5511a_set_output(dev_handle, 1);
        if (status != SUCCESS) {
            std::cerr << "[SignalCoreSC5511A Plugin] Failed to enable RF output" << std::endl;
            if (onError) {
                onError("Failed to enable RF output");
            }
            return;
        }
    }
    
    m_rfEnabled = true;
    
    std::cout << "[SignalCoreSC5511A Plugin] RF output ENABLED" << std::endl;
    if (onRfEnabled) {
        onRfEnabled();
    }
}

void SignalCoreSC5511A::disableRf()
{
    if (!m_isConnected) {
        std::cerr << "[SignalCoreSC5511A Plugin] Cannot disable RF - not connected" << std::endl;
        return;
    }
    
    if (!m_rfEnabled) {
        std::cout << "[SignalCoreSC5511A Plugin] RF already disabled" << std::endl;
        return;
    }
    
    std::cout << "[SignalCoreSC5511A Plugin] Disabling RF output..." << std::endl;
    
    // Disable RF1 output using sc5511a API
    if (dev_handle != NULL) {
        status = sc5511a_set_output(dev_handle, 0);
        if (status != SUCCESS) {
            std::cerr << "[SignalCoreSC5511A Plugin] Failed to disable RF output" << std::endl;
        }
    }
    
    m_rfEnabled = false;
    
    std::cout << "[SignalCoreSC5511A Plugin] RF output DISABLED" << std::endl;
    if (onRfDisabled) {
        onRfDisabled();
    }
}

bool SignalCoreSC5511A::isRfEnabled() const
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
        std::cout << "[SignalCoreSC5511A Plugin] Factory: Creating plugin instance" << std::endl;
        return new SignalCoreSC5511A();
    }
    
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    void destroyPlugin(void* plugin)
    {
        std::cout << "[SignalCoreSC5511A Plugin] Factory: Destroying plugin instance" << std::endl;
        delete static_cast<ISignalGeneratorPlugin*>(plugin);
    }
}
