#ifndef MATCHENGINE_H
#define MATCHENGINE_H

#include "orderBook.h"
#include "trade.h"
#include "ThreadSafeQueue.h"
#include<vector>
#include <thread>
#include <atomic>
#include <mutex>
#include<fstream>

class MatchEngine{
private:
    OrderBook book;
    vector<Trade> tradeHistory;
    vector<Order> stopOrders;
    ThreadSafeQueue<Order> orderQueue;
    std::thread workerThread;
    std::atomic<bool> isRunning;
    std::mutex engineMutex;
    std::ofstream tradeLog;
    void processLoop();
    double totalTradedValue=0.0;
    long long totalVolume=0;
public:
    MatchEngine();
    ~MatchEngine();

    void submitOrder(Order newOrder);
    void processOrder(Order newOrder);
    void printOrderBoook();
    void printTrades();
    bool cancelOrder(int orderId);
    bool modifyOrder(int orderId,double newPrice,int newQauntity);
    void checkStopOrders(double lastTradePrice);
    void addStopOrder(Order order);
    void removeExpiredOrders(long long currentTime);
    bool canFullyFill(Order order);
    void printAnalytics();
};

#endif