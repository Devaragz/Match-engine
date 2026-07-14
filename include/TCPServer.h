#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "MatchEngine.h"
#include <thread>
#include <atomic>

class TCPServer {
private:
    MatchEngine& engine;
    int server_fd;
    std::atomic<bool> isRunning;
    std::thread serverThread;

    void listenLoop(int port);

public:
    TCPServer(MatchEngine& eng);
    ~TCPServer();
    void start(int port);
    void stop();
};

#endif