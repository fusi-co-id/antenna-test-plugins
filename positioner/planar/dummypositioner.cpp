#include "dummypositioner.h"
#include <chrono>
#include <thread>
#include <cmath>
#include <iostream>
#include <sstream>
#include <cstdint>

DummyPositioner::DummyPositioner()
    : m_isConnected(false)
    , m_connectedAddress("")
    , m_socket(nullptr)
{
    // Initialize positions
    m_currentTxPos.AZ = 0; m_currentTxPos.EL = 0; m_currentTxPos.POL = 0;
    m_currentTxPos.X = 0; m_currentTxPos.Y = 0; m_currentTxPos.Z = 0;

    m_currentRxPos.AZ = 0; m_currentRxPos.EL = 0; m_currentRxPos.POL = 0;
    m_currentRxPos.X = 0; m_currentRxPos.Y = 0; m_currentRxPos.Z = 0;
    std::cout << "[Dummy Positioner Plugin] Instance created\n";
}

std::vector<DeviceInfo> DummyPositioner::scanDevices()
{
    std::cout << "[Dummy Positioner Plugin] Scanning for devices...\n";
    
    std::vector<DeviceInfo> devices;
    
    // Simulate finding devices
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Simulate 1 LAN device
    DeviceInfo device1;
    device1.name = "Dummy Positioner";
    device1.serialNumber = "DPS-1000";
    device1.address = "192.168.20.26";
    device1.port = "5000";
    device1.type = "LAN";
    device1.isAvailable = true;
    devices.push_back(device1);
    
    std::cout << "[Dummy Positioner Plugin] Found " << devices.size() << " devices\n";
    
    if (onDevicesScanned) {
        onDevicesScanned(devices);
    }
    return devices;
}

bool DummyPositioner::connectToDevice(const std::string &address, const std::string &port)
{
    if (m_isConnected) {
        std::cerr << "[Dummy Positioner Plugin] Already connected to " << m_connectedAddress << "\n";
        return false;
    }
    
    uint16_t p = port.empty() ? 5000 : static_cast<uint16_t>(std::stoi(port));
    std::cout << "[Dummy Positioner Plugin] Connecting to device at: " << address << " on port " << p << "\n";
    
    try {
        m_socket = new SocketInstrument(address, p, 3, {});
        if (m_socket->isConnected()) {
            m_connectedAddress = address;
            m_isConnected = true;
            std::cout << "[Dummy Positioner Plugin] Connected successfully to " << address << "\n";
            if (onConnected) {
                onConnected();
            }
            return true;
        } else {
            std::cerr << "[Dummy Positioner Plugin] Connection failed\n";
            delete m_socket;
            m_socket = nullptr;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "[Dummy Positioner Plugin] Exception during connection: " << e.what() << "\n";
        return false;
    }
}

DummyPositioner::~DummyPositioner()
{
    if (m_isConnected) {
        disconnect();
    }
    std::cout << "[Dummy Positioner Plugin] Instance destroyed\n";
}

void DummyPositioner::disconnect()
{
    if (!m_isConnected) {
        std::cerr << "[Dummy Positioner Plugin] Not connected\n";
        return;
    }
    
    std::cout << "[Dummy Positioner Plugin] Disconnecting from " << m_connectedAddress << "\n";
    
    if (m_socket) {
        m_socket->disconnect();
        delete m_socket;
        m_socket = nullptr;
    }
    
    m_isConnected = false;
    m_connectedAddress.clear();
    
    std::cout << "[Dummy Positioner Plugin] Disconnected\n";
    if (onDisconnected) {
        onDisconnected();
    }
}

bool DummyPositioner::isConnected() const
{
    return m_isConnected;
}

void DummyPositioner::setTxStep(const Position& step) {}
void DummyPositioner::setTxMinRange(const Position& minRange) {}
void DummyPositioner::setTxMaxRange(const Position& maxRange) {}
void DummyPositioner::setRxStep(const Position& step) {}
void DummyPositioner::setRxMinRange(const Position& minRange) {}
void DummyPositioner::setRxMaxRange(const Position& maxRange) {}

Position DummyPositioner::getCurrentTxPosition() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentTxPos;
}

Position DummyPositioner::getCurrentRxPosition() const
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
        << ";Y=" << position.Y << ";\n";
    return oss.str();
}

void DummyPositioner::moveTxToPosition(const Position &position)
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
        
        std::cout << "[Dummy Positioner Plugin] Sending Tx command: " << logCmd << "\n";
        
        if (m_socket) {
            m_socket->write_raw(command);

            // --- KODE BARU YANG DITAMBAHKAN ---
            // Tunggu balasan dari perangkat yang menandakan pergerakan selesai.
            // Ini membuat fungsi menjadi "blocking" (menunggu).
            try {
                // Sekarang fungsi ini sudah ada di SocketInstrument
                std::string response = m_socket->read_until('\n', 30000); // Timeout 30 detik
            } catch (const std::exception& e) {
                std::cerr << "[Dummy Positioner Plugin] Timeout waiting for movement completion response: " << e.what() << "\n";
            }
        }
    }
    
    if (onTxPositionChanged) {
        onTxPositionChanged(position);
    }
}

void DummyPositioner::moveRxToPosition(const Position &position)
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
        
        std::cout << "[Dummy Positioner Plugin] Sending Rx command: " << logCmd << "\n";
        
        if (m_socket) {
            m_socket->write_raw(command);

            // --- KODE BARU YANG DITAMBAHKAN ---
            // Tunggu balasan dari perangkat yang menandakan pergerakan selesai.
            // Ini membuat fungsi menjadi "blocking" (menunggu).
            try {
                // Sekarang fungsi ini sudah ada di SocketInstrument
                std::string response = m_socket->read_until('\n', 30000); // Timeout 30 detik
            } catch (const std::exception& e) {
                std::cerr << "[Dummy Positioner Plugin] Timeout waiting for movement completion response: " << e.what() << "\n";
            }
        }
    }
    
    if (onRxPositionChanged) {
        onRxPositionChanged(position);
    }
}

void DummyPositioner::stopMovement()
{
    std::cout << "[Dummy Positioner Plugin] stopMovement\n";
    if (isConnected()) {
        std::cout << "[Dummy Positioner Plugin] Sending command: STOP;\n";
        
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
        std::cout << "[Dummy Positioner Plugin] Factory: Creating plugin instance\n";
        return new DummyPositioner();
    }
    
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    void destroyPlugin(void* plugin)
    {
        std::cout << "[Dummy Positioner Plugin] Factory: Destroying plugin instance\n";
        delete static_cast<IPositionerPlugin*>(plugin);
    }
}
