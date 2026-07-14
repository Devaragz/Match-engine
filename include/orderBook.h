#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include<map>
#include<unordered_map>
#include<list>
#include "order.h"
using namespace std;

struct OrderLocation {
    bool isBuy;
    double price;
    list<Order>::iterator it;
};

class OrderBook{
private:
    map<double,list<Order>,greater<double>>buyOrders;
    map<double,list<Order>>sellOrders;
    unordered_map<int, OrderLocation> orderMap;
public:
    void adddOrder(Order newOrder);
    void printBook();
    bool hasSellOrders();
    double getBestSellPrice();
    Order& getFirstSellOrder();
    void removeBestSellOrder();
    bool hasBuyOrders();
    double getBestBuyPrice();
    Order& getFirstBuyOrder();
    void removeBestBuyOrder();
    bool cancelOrder(int orderId);
    bool modifyOrder(int orderId,double newPrice,int newQauntity);
    void removeExpiredOrders(long long currentTime);
};

#endif