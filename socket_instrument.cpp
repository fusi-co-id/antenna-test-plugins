
#include "socket_instrument.h"
#include <iostream>
#ifdef _WIN32
#include <ws2tcpip.h>
#endif

SocketInstrument::SocketInstrument(const std::string& ip, int port, int timeout_sec, const std::map<std::string, std::string>& cmd_map)
    : connected(false), terminator("\r\n"), commands(cmd_map) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }
#endif
    connectSocket(ip, port, timeout_sec);
}

SocketInstrument::~SocketInstrument() {
    disconnect();
#ifdef _WIN32
    WSACleanup();
#endif
}

void SocketInstrument::connectSocket(const std::string& ip, int port, int timeout_sec) {
#ifdef _WIN32
    sockfd = (int)socket(AF_INET, SOCK_STREAM, 0);
#else
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
#endif
    if (sockfd < 0) throw std::runtime_error("Socket creation failed");

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
#ifdef _WIN32
    // Use PCSTR (const char*) for IPv4 string
    if (InetPton(AF_INET, (PCSTR)ip.c_str(), &serv_addr.sin_addr) != 1) {
        throw std::runtime_error("Invalid IP address format");
    }
#else
    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) != 1) {
        throw std::runtime_error("Invalid IP address format");
    }
#endif
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        throw std::runtime_error("Connection failed");
    }
    connected = true;
}

void SocketInstrument::write(const std::string& command, const std::string& value) {
    auto it = commands.find(command);
    if (it == commands.end()) throw std::runtime_error("Unknown command: " + command);
    std::string cmd = it->second;
    size_t pos = cmd.find("{}");
    if (pos != std::string::npos) {
        cmd.replace(pos, 2, value);
    }
    cmd += terminator;
    send(sockfd, cmd.c_str(), (int)cmd.size(), 0);
}

void SocketInstrument::write_raw(const std::string& raw_command) {
    send(sockfd, raw_command.c_str(), (int)raw_command.size(), 0);
}

std::string SocketInstrument::query(const std::string& command) {
    auto it = commands.find(command);
    if (it == commands.end()) throw std::runtime_error("Unknown command: " + command);
    std::string cmd = it->second + terminator;
    send(sockfd, cmd.c_str(), (int)cmd.size(), 0);
    char buffer[4096] = {0};
    int n = recv(sockfd, buffer, sizeof(buffer)-1, 0);
    if (n > 0) return std::string(buffer, n);
    return "";
}

void SocketInstrument::disconnect() {
    if (connected) {
#ifdef _WIN32
        closesocket(sockfd);
#else
        close(sockfd);
#endif
        connected = false;
    }
}

bool SocketInstrument::isConnected() const {
    return connected;
}
