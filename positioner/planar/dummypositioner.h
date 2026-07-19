#ifndef DUMMYPOSITIONER_H
#define DUMMYPOSITIONER_H

#include "iplugininterface.h"
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include "socket_instrument.h"


class DummyPositioner : public IPositionerPlugin
{
public:
    DummyPositioner();
    virtual ~DummyPositioner();
    
    // Device discovery
    std::vector<DeviceInfo> scanDevices() override;
    bool connectToDevice(const std::string &address, const std::string &port) override;
    
    // Connection management
    void disconnect() override;
    bool isConnected() const override;
    
    // Configuration
    void setTxStep(const Position& step) override;
    void setTxMinRange(const Position& minRange) override;
    void setTxMaxRange(const Position& maxRange) override;
    void setRxStep(const Position& step) override;
    void setRxMinRange(const Position& minRange) override;
    void setRxMaxRange(const Position& maxRange) override;
    
    // Get Position
    Position getCurrentTxPosition() const override;
    Position getCurrentRxPosition() const override;
    
    // Control
    void moveTxToPosition(const Position& position) override;
    void moveRxToPosition(const Position& position) override;
    void stopMovement() override;
    
private:
    SocketInstrument *m_socket;
    std::atomic<bool> m_isConnected;
    std::string m_connectedAddress;
    
    Position m_currentTxPos;
    Position m_currentRxPos;
    mutable std::mutex m_mutex;
};

#endif // DUMMYPOSITIONER_H
