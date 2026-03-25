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

#include "planar.h"
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif
#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <vector>
#include <regex>
#include <memory>
#include <mutex>

namespace {

class TeeStreamBuf : public std::streambuf {
public:
    TeeStreamBuf(std::streambuf* first, std::streambuf* second)
        : m_first(first), m_second(second)
    {
    }

protected:
    int overflow(int ch) override
    {
        if (ch == EOF) {
            return !EOF;
        }

        const int firstResult = m_first ? m_first->sputc(static_cast<char>(ch)) : ch;
        const int secondResult = m_second ? m_second->sputc(static_cast<char>(ch)) : ch;
        return (firstResult == EOF || secondResult == EOF) ? EOF : ch;
    }

    int sync() override
    {
        const int firstSync = m_first ? m_first->pubsync() : 0;
        const int secondSync = m_second ? m_second->pubsync() : 0;
        return (firstSync == 0 && secondSync == 0) ? 0 : -1;
    }

private:
    std::streambuf* m_first;
    std::streambuf* m_second;
};

std::once_flag g_debugLogInitFlag;
std::unique_ptr<std::ofstream> g_logFile;
std::unique_ptr<TeeStreamBuf> g_teeBuf;

bool readDebugFlagFromJson(const std::filesystem::path& jsonPath)
{
    std::ifstream input(jsonPath);
    if (!input.is_open()) {
        return false;
    }

    std::string jsonText((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    const std::regex debugRegex(R"("debug"\s*:\s*true)", std::regex_constants::icase);
    return std::regex_search(jsonText, debugRegex);
}

std::filesystem::path findPlanarConfigPath()
{
    const std::filesystem::path cwd = std::filesystem::current_path();
    const std::vector<std::filesystem::path> candidates = {
        cwd / "planar.json",
        cwd / "positioner" / "planar" / "planar.json",
        cwd / "instruments" / "positioner" / "planar" / "planar.json"
    };

    for (const auto& path : candidates) {
        if (std::filesystem::exists(path)) {
            return path;
        }
    }

    return {};
}

void initializeDebugLogMirror()
{
    std::call_once(g_debugLogInitFlag, []() {
        const std::filesystem::path configPath = findPlanarConfigPath();
        if (configPath.empty() || !readDebugFlagFromJson(configPath)) {
            return;
        }

        const std::filesystem::path logPath = configPath.parent_path() / "log_plugin.txt";
        g_logFile = std::make_unique<std::ofstream>(logPath, std::ios::app);
        if (!g_logFile->is_open()) {
            return;
        }

        g_teeBuf = std::make_unique<TeeStreamBuf>(std::cout.rdbuf(), g_logFile->rdbuf());
        std::cout.rdbuf(g_teeBuf.get());
        std::cout << "[Planar Positioner Plugin] Debug log mirror enabled at " << logPath.string() << std::endl;
    });
}

} // namespace

PlanarPositioner::PlanarPositioner()
    : m_isConnected(false)
    , m_isMoving(false)
    , m_connectedIPAddress("")
    , m_stepCount(0)
{
    initializeDebugLogMirror();

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

    std::cout << "[Planar Positioner Plugin] Instance created" << std::endl;
}

std::vector<DeviceInfo> PlanarPositioner::scanDevices()
{
    std::cout << "[Planar Positioner Plugin] Scanning for devices..." << std::endl;
    
    std::vector<DeviceInfo> devices;
    
    // Simulate finding devices
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Simulate 1 LAN device
    DeviceInfo device1;
    device1.name = "Planar Positioner-AZ/EL";
    device1.serialNumber = "DPS-1000";
    device1.address = "192.168.1.120";
    device1.port = "5025";
    device1.type = "LAN";
    device1.isAvailable = true;
    devices.push_back(device1);

    m_connectedIPAddress = device1.address;
    m_connectedPort = device1.port;
    
    // // Simulate 1 Serial device
    // DeviceInfo device2;
    // device2.name = "Planar Positioner-6DOF";
    // device2.serialNumber = "DPS-2000";
    // device2.address = "COM3";
    // device2.type = "Serial";
    // device2.isAvailable = true;
    // devices.push_back(device2);
    
    std::cout << "[Planar Positioner Plugin] Found " << devices.size() << " devices" << std::endl;
    
    // if (onDevicesScanned) {
    //     try {
    //         onDevicesScanned(devices);
    //     } catch (const std::exception& ex) {
    //         std::cerr << "[Planar SG Plugin] Exception in onDevicesScanned callback: " << ex.what() << std::endl;
    //     } catch (...) {
    //         std::cerr << "[Planar SG Plugin] Unknown exception in onDevicesScanned callback" << std::endl;
    //     }
    // } else {
    //     std::cerr << "[Planar SG Plugin] onDevicesScanned callback is not set" << std::endl;
    // }
    return devices;
}

bool PlanarPositioner::connectToDevice(const std::string &address, const std::string &port)
{
    if (m_isConnected) {
        std::cerr << "[Planar Positioner Plugin] Already connected to " << m_connectedIPAddress << " on port: " << m_connectedPort << std::endl;
        return false;
    }
    
    std::cout << "[Planar Positioner Plugin] Connecting to device at: " << address << " on port: " << port << std::endl;
    
    bool success = false;
#ifdef _WIN32
    // Only attempt socket if address looks like an IP
    if (address.find(".") != std::string::npos) {
        m_socketInstrument = new SocketInstrument(address, std::stoi(port), 5, {});
    } else {
        // Not a LAN device, simulate connection
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        success = true;
    }
#else
    // Non-Windows: just simulate
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    success = true;
#endif
    if (success) {
        m_connectedIPAddress = address;
        m_isConnected = true;
        std::cout << "[Planar Positioner Plugin] Connected successfully to " << address << std::endl;
        if (onConnected) {
            onConnected();
        }
    }
    return success;
}

PlanarPositioner::~PlanarPositioner()
{
    if (m_isConnected) {
        disconnect();
    }
    std::cout << "[Planar Positioner Plugin] Instance destroyed" << std::endl;
}

void PlanarPositioner::disconnect()
{
    if (!m_isConnected) {
        std::cerr << "[Planar Positioner Plugin] Not connected" << std::endl;
        return;
    }
        
    std::cout << "[Planar Positioner Plugin] Disconnecting from " << m_connectedIPAddress << std::endl;
    
    m_isConnected = false;
    m_connectedIPAddress.clear();
    
    std::cout << "[Planar Positioner Plugin] Disconnected" << std::endl;
    if (onDisconnected) {
        onDisconnected();
    }
}

bool PlanarPositioner::isConnected() const
{
    return m_isConnected;
}

void PlanarPositioner::setTxStep(const Position &step)
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

void PlanarPositioner::setRxStep(const Position &step)
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

void PlanarPositioner::setTxMinRange(const Position &minRange)
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

void PlanarPositioner::setRxMinRange(const Position &minRange)
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

void PlanarPositioner::setTxMaxRange(const Position &maxRange)
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

void PlanarPositioner::setRxMaxRange(const Position &maxRange)
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

Position PlanarPositioner::getCurrentTxPosition() const
{
    return m_currentTxPosition;
}

Position PlanarPositioner::getCurrentRxPosition() const
{
    return m_currentRxPosition;
}

void PlanarPositioner::stopMovement()
{
    if (m_isMoving) {
        std::cout << "[Planar Positioner Plugin] Stopping movement..." << std::endl;
        m_isMoving = false;
        // Send stop command to all motors
        m_socketInstrument->write_raw("motor_1 off");
        m_socketInstrument->write_raw("motor_2 off");
        m_socketInstrument->write_raw("motor_3 off");
        m_socketInstrument->write_raw("motor_4 off");
        m_socketInstrument->write_raw("motor_5 off");
    } else {
        std::cout << "[Planar Positioner Plugin] No movement to stop" << std::endl;
    }
}

void PlanarPositioner::moveTxToPosition(const Position &position)
{
    // Populate Real Motor Command Here based on the position movement
    // The syntax is: motor_1 on [diatance] [direction] 
    // motor_1: TX horizontal
    // motor_2: TX vertical
    // motor_5: TX polarization

    if (m_currentTxPosition.X != position.X) {
        std::string motorCommand = "motor_1 on " + std::to_string(std::abs(position.X - m_currentTxPosition.X)) + ((position.X > m_currentTxPosition.X) ? " maju" : " mundur");
        m_socketInstrument->write_raw(motorCommand);
    }
    if (m_currentTxPosition.Y != position.Y) {
        std::string motorCommand  = "motor_2 on " + std::to_string(std::abs(position.Y - m_currentTxPosition.Y)) + ((position.Y > m_currentTxPosition.Y) ? " naik" : " turun");
        m_socketInstrument->write_raw(motorCommand);
    }
    if (m_currentTxPosition.POL != position.POL) {
        std::string motorCommand = "motor_5 on " + std::to_string(std::abs(position.POL - m_currentTxPosition.POL)) + ((position.POL > m_currentTxPosition.POL) ? " CW" : " CCW");
        m_socketInstrument->write_raw(motorCommand);
    }

    std::cout << "[Planar Positioner Plugin] moveTxToPosition called with position: AZ=" << position.AZ << " EL=" << position.EL << " POL=" << position.POL << " X=" << position.X << " Y=" << position.Y << " Z=" << position.Z << std::endl;
}

void PlanarPositioner::moveRxToPosition(const Position &position)
{
    // Populate Real Motor Command Here based on the position movement
    // The syntax is: motor_1 on [diatance] [direction] 
    // motor_3: RX horizontal
    // motor_4: RX vertical
    
    if (m_currentRxPosition.X != position.X) {
        std::string motorCommand = "motor_3 on " + std::to_string(std::abs(position.X - m_currentRxPosition.X)) + ((position.X > m_currentRxPosition.X) ? " maju" : " mundur");
        m_socketInstrument->write_raw(motorCommand);
    }
    if (m_currentRxPosition.Y != position.Y) {
        std::string motorCommand = "motor_4 on " + std::to_string(std::abs(position.Y - m_currentRxPosition.Y)) + ((position.Y > m_currentRxPosition.Y) ? " naik" : " turun");
        m_socketInstrument->write_raw(motorCommand);
    }
    
    std::cout << "[Planar Positioner Plugin] moveRxToPosition called with position: AZ=" << position.AZ << " EL=" << position.EL << " POL=" << position.POL << " X=" << position.X << " Y=" << position.Y << " Z=" << position.Z << std::endl;  
}

// Factory function to create plugin instance
extern "C" {
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    IPositionerPlugin* createPositionerPlugin()
    {
        std::cout << "[Planar Positioner Plugin] Factory: Creating plugin instance" << std::endl;
        return new PlanarPositioner();
    }
    
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    void destroyPlugin(void* plugin)
    {
        std::cout << "[Planar Positioner Plugin] Factory: Destroying plugin instance" << std::endl;
        delete static_cast<IPositionerPlugin*>(plugin);
    }
}
