#include "rotary.h"
#include <chrono>
#include <thread>
#include <cmath>
#include <iostream>
#include <sstream>
#include <cstdint>

Rotary::Rotary()
    : m_isConnected(false)
    , m_connectedAddress("")
    , m_socket(nullptr)
{
    // Initialize positions
    m_currentTxPos.AZ = 0; m_currentTxPos.EL = 0; m_currentTxPos.POL = 0;
    m_currentTxPos.X = 0; m_currentTxPos.Y = 0; m_currentTxPos.Z = 0;

    m_currentRxPos.AZ = 0; m_currentRxPos.EL = 0; m_currentRxPos.POL = 0;
    m_currentRxPos.X = 0; m_currentRxPos.Y = 0; m_currentRxPos.Z = 0;
    m_currentRxPos.RV = 0; m_currentRxPos.RH = 0; m_currentRxPos.LIN = 0;
    std::cout << "[Rotary Plugin] Instance created\n";
}

std::vector<DeviceInfo> Rotary::scanDevices()
{
    std::cout << "[Rotary Plugin] Scanning for devices...\n";
    
    std::vector<DeviceInfo> devices;
    
    // Simulate finding devices
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Simulate 1 LAN device
    DeviceInfo device1;
    device1.name = "Rotary Positioner";
    device1.serialNumber = "RTR-1000";
    device1.address = "192.168.20.26";
    device1.port = "5000";
    device1.type = "LAN";
    device1.isAvailable = true;
    devices.push_back(device1);
    
    std::cout << "[Rotary Plugin] Found " << devices.size() << " devices\n";
    
    if (onDevicesScanned) {
        onDevicesScanned(devices);
    }
    return devices;
}

bool Rotary::connectToDevice(const std::string &address, const std::string &port)
{
    if (m_isConnected) {
        std::cerr << "[Rotary Plugin] Already connected to " << m_connectedAddress << "\n";
        return false;
    }
    
    uint16_t p = port.empty() ? 5000 : static_cast<uint16_t>(std::stoi(port));
    std::cout << "[Rotary Plugin] Connecting to device at: " << address << " on port " << p << "\n";
    
    try {
        m_socket = new SocketInstrument(address, p, 3, {});
        if (m_socket->isConnected()) {
            m_connectedAddress = address;
            m_isConnected = true;
            std::cout << "[Rotary Plugin] Connected successfully to " << address << "\n";
            if (onConnected) {
                onConnected();
            }
            return true;
        } else {
            std::cerr << "[Rotary Plugin] Connection failed\n";
            delete m_socket;
            m_socket = nullptr;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "[Rotary Plugin] Exception during connection: " << e.what() << "\n";
        return false;
    }
}

Rotary::~Rotary()
{
    if (m_isConnected) {
        disconnect();
    }
    std::cout << "[Rotary Plugin] Instance destroyed\n";
}

void Rotary::disconnect()
{
    if (!m_isConnected) {
        std::cerr << "[Rotary Plugin] Not connected\n";
        return;
    }
    
    std::cout << "[Rotary Plugin] Disconnecting from " << m_connectedAddress << "\n";
    
    if (m_socket) {
        m_socket->disconnect();
        delete m_socket;
        m_socket = nullptr;
    }
    
    m_isConnected = false;
    m_connectedAddress.clear();
    
    std::cout << "[Rotary Plugin] Disconnected\n";
    if (onDisconnected) {
        onDisconnected();
    }
}

bool Rotary::isConnected() const
{
    return m_isConnected;
}

void Rotary::setTxStep(const Position& step) {}
void Rotary::setTxMinRange(const Position& minRange) {}
void Rotary::setTxMaxRange(const Position& maxRange) {}
void Rotary::setRxStep(const Position& step) {}
void Rotary::setRxMinRange(const Position& minRange) {}
void Rotary::setRxMaxRange(const Position& maxRange) {}

Position Rotary::getCurrentTxPosition() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentTxPos;
}

Position Rotary::getCurrentRxPosition() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentRxPos;
}

// Helper function to create the command string consistently for both TX and RX
static std::string buildCommandString(const Position& position)
{
    std::ostringstream oss;
    oss << "V=" << position.Z 
        << ";AZ=" << position.AZ 
        << ";EL=" << position.EL 
        << ";POL=" << position.POL 
        << ";X=" << position.X 
        << ";Y=" << position.Y
        << ";RV=" << position.RV
        << ";RH=" << position.RH
        << ";LIN=" << position.LIN << ";\n";
    return oss.str();
}

void Rotary::moveTxToPosition(const Position &position)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_currentTxPos = position;
    }

    if (isConnected()) {
        // The previous implementation was incorrect, only sending partial data.
        // This now uses a consistent helper to build the full command string.
        std::string command = buildCommandString(position);
        std::string logCmd = command;
        if (!logCmd.empty() && logCmd.back() == '\n') logCmd.pop_back();
        
        std::cout << "[Rotary Plugin] Sending Tx command: " << logCmd << "\n";
        
        if (m_socket) {
            m_socket->write_raw(command);

            // --- KODE BARU YANG DITAMBAHKAN ---
            // Tunggu balasan dari perangkat yang menandakan pergerakan selesai.
            // Ini membuat fungsi menjadi "blocking" (menunggu).
            try {
                // Sekarang fungsi ini sudah ada di SocketInstrument
                std::string response = m_socket->read_until('\n', 30000); // Timeout 30 detik
            } catch (const std::exception& e) {
                std::cerr << "[Rotary Plugin] Timeout waiting for movement completion response: " << e.what() << "\n";
            }
        }
    }
    
    if (onTxPositionChanged) {
        onTxPositionChanged(position);
    }
}

void Rotary::moveRxToPosition(const Position &position)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_currentRxPos = position;
    }
    
    if (isConnected()) {
        // Ensure RX also sends the complete command string using the same helper.
        std::string command = buildCommandString(position);
        std::string logCmd = command;
        if (!logCmd.empty() && logCmd.back() == '\n') logCmd.pop_back();
        
        std::cout << "[Rotary Plugin] Sending Rx command: " << logCmd << "\n";
        
        if (m_socket) {
            m_socket->write_raw(command);

            // --- KODE BARU YANG DITAMBAHKAN ---
            // Tunggu balasan dari perangkat yang menandakan pergerakan selesai.
            // Ini membuat fungsi menjadi "blocking" (menunggu).
            try {
                // Sekarang fungsi ini sudah ada di SocketInstrument
                std::string response = m_socket->read_until('\n', 30000); // Timeout 30 detik
            } catch (const std::exception& e) {
                std::cerr << "[Rotary Plugin] Timeout waiting for movement completion response: " << e.what() << "\n";
            }
        }
    }
    
    if (onRxPositionChanged) {
        onRxPositionChanged(position);
    }
}

void Rotary::stopMovement()
{
    std::cout << "[Rotary Plugin] stopMovement\n";
    if (isConnected()) {
        std::cout << "[Rotary Plugin] Sending command: STOP;\n";
        
        if (m_socket) {
            m_socket->write_raw("STOP;\n");
        }
    }
    if (onTxMovementStopped) onTxMovementStopped();
    if (onRxMovementStopped) onRxMovementStopped();
}

// Factory function to create plugin instance
extern "C" {
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    IPositionerPlugin* createPositionerPlugin()
    {
        std::cout << "[Rotary Plugin] Factory: Creating plugin instance\n";
        return new Rotary();
    }
    
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    void destroyPlugin(void* plugin)
    {
        std::cout << "[Rotary Plugin] Factory: Destroying plugin instance\n";
        delete static_cast<IPositionerPlugin*>(plugin);
    }
}
