#ifndef SOCKET_INSTRUMENT_H
#define SOCKET_INSTRUMENT_H

#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <iostream>
#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif


#include <map>
#include <string>

class SocketInstrument {
public:
    SocketInstrument(const std::string& ip, int port, int timeout_sec = 5, const std::map<std::string, std::string>& cmd_map = {});
    ~SocketInstrument();
    void write(const std::string& command, const std::string& value = "");
    void write_raw(const std::string& raw_command);
    std::string query(const std::string& command);
    std::string read_until(char delimiter, int timeout_ms);
    void disconnect();
    bool isConnected() const;
private:
    int sockfd;
    bool connected;
    std::string terminator;
    std::map<std::string, std::string> commands;
    void connectSocket(const std::string& ip, int port, int timeout_sec);
    std::string sendAndReceive(const std::string& cmd);
};

#endif // SOCKET_INSTRUMENT_H
