#include <iostream>
#include <thread>
#include <chrono>
#include "../include/MatchEngine.h"
#include "../include/TCPServer.h"

using namespace std;

int main() {
    system("cls"); // Clears terminal for a clean video start
    cout << "--- BOOTING SYSTEM ARCHITECTURE ---\n";
    MatchEngine engine;

    cout << "\n[1] DEMONSTRATING O(1) LOOKUP & MODIFICATION\n";
    engine.submitOrder(Order(999, true, 100, 150.0, 1));
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Let queue process
    
    engine.modifyOrder(999, 155.0, 200);
    engine.cancelOrder(999);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    cout << "\n[2] STARTING TCP SERVER (MULTITHREADING ACTIVE)\n";
    TCPServer server(engine);
    server.start(8080);

    cout << "Waiting for high-frequency network orders...\n";
    cout << "Press ENTER to shut down gracefully and print analytics.\n\n";

    cin.get(); 

    cout << "\n--- SHUTTING DOWN ---\n";
    engine.printOrderBoook();
    engine.printAnalytics();
    
    server.stop();
    return 0;
}