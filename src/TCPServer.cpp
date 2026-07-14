#include <winsock2.h>
#include <ws2tcpip.h>
#include "../include/TCPServer.h"
#include <iostream>
#include <sstream>

using namespace std;

TCPServer::TCPServer(MatchEngine& eng) : engine(eng), isRunning(false), server_fd(-1) {
    // Initialize Windows Sockets
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

TCPServer::~TCPServer() { 
    stop(); 
    WSACleanup();
}

void TCPServer::start(int port) {
    isRunning = true;
    serverThread = std::thread(&TCPServer::listenLoop, this, port);
}

void TCPServer::stop() {
    isRunning = false;
    if (server_fd != -1) closesocket(server_fd);
    if (serverThread.joinable()) serverThread.join();
}

void TCPServer::listenLoop(int port) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);
    cout << "TCP Server started on Windows. Listening on port " << port << "...\n";

    while (isRunning) {
        int client_socket = accept(server_fd, nullptr, nullptr);
        if (client_socket < 0) continue;

        char buffer[256] = {0};
        int bytes_received = recv(client_socket, buffer, 255, 0);
        
        if (bytes_received > 0) {
            Order ord = Order::fromString(string(buffer), 0);
            engine.submitOrder(ord); 
        }
        closesocket(client_socket);
    }
}