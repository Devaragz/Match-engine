#include "../include/orderBook.h"
#include<iostream>
using namespace std;

void OrderBook::adddOrder(Order newOrder){
    if(newOrder.isBuy){
        buyOrders[newOrder.price].push_back(newOrder);
        auto it = --buyOrders[newOrder.price].end();
        orderMap[newOrder.orderId] = {true, newOrder.price, it};
    }else{
        sellOrders[newOrder.price].push_back(newOrder);
        auto it = --sellOrders[newOrder.price].end();
        orderMap[newOrder.orderId] = {false, newOrder.price, it};
    }
}

void OrderBook::printBook(){
    cout<<"\n===== BUY ORDERS =====\n";
    for(auto &level : buyOrders){
        cout<<"Price: "<<level.first<<"|Orders: "<<level.second.size()<<"\n";
    }
    cout<<"\n===== SELL ORDERS =====\n";
    for(auto &level : sellOrders){
        cout<<"Price: "<<level.first<<"|Orders: "<<level.second.size()<<"\n";
    }
}

bool OrderBook::hasSellOrders(){
    return !sellOrders.empty();
}

double OrderBook::getBestSellPrice(){
    return sellOrders.begin()->first;
}

Order& OrderBook::getFirstSellOrder(){
    return sellOrders.begin()->second.front();
}

void OrderBook::removeBestSellOrder(){
    auto mapIt = sellOrders.begin();
    int id = mapIt->second.front().orderId;
    orderMap.erase(id); // Remove from lookup table
    mapIt->second.pop_front();
    if(mapIt->second.empty()){
        sellOrders.erase(mapIt);
    }
}

bool OrderBook::hasBuyOrders(){
    return !buyOrders.empty();
}

double OrderBook::getBestBuyPrice(){
    return buyOrders.begin()->first;
}

Order& OrderBook::getFirstBuyOrder(){
    return buyOrders.begin()->second.front();
}

void OrderBook::removeBestBuyOrder(){
    auto mapIt = buyOrders.begin();
    int id = mapIt->second.front().orderId;
    orderMap.erase(id); // Remove from lookup table
    mapIt->second.pop_front();
    if(mapIt->second.empty()){
        buyOrders.erase(mapIt);
    }
}

bool OrderBook::cancelOrder(int orderId){
    auto lookupIt = orderMap.find(orderId);
    if (lookupIt == orderMap.end()) return false;
    OrderLocation loc = lookupIt->second;
    orderMap.erase(lookupIt);
    if (loc.isBuy) {
        buyOrders[loc.price].erase(loc.it);
        if (buyOrders[loc.price].empty()) {
            buyOrders.erase(loc.price);
        }
    } else {
        sellOrders[loc.price].erase(loc.it);
        if (sellOrders[loc.price].empty()) {
            sellOrders.erase(loc.price);
        }
    }
    return true;
}

bool OrderBook::modifyOrder(int orderId,double newPrice,int newQuantity){
    auto lookupIt = orderMap.find(orderId);
    if (lookupIt == orderMap.end()) return false;

    Order updatedOrder = *(lookupIt->second.it);
    cancelOrder(orderId);
    updatedOrder.price = newPrice;
    updatedOrder.quantity = newQuantity;
    adddOrder(updatedOrder);
    return true;
}

void OrderBook::removeExpiredOrders(long long currentTime){
    for(auto buyIt=buyOrders.begin();buyIt!=buyOrders.end();){
        auto &orders=buyIt->second;
        for(auto ordIt=orders.begin();ordIt!=orders.end();){
            if(ordIt->expiryTime!=-1 && ordIt->expiryTime<=currentTime){
                cout<<"ORDER EXPIRED: "<<ordIt->orderId<<"\n";
                ordIt=orders.erase(ordIt);
            }else{
                ++ordIt;
            }
        }
        if(orders.empty()){
            buyIt=buyOrders.erase(buyIt);
        }else{
            ++buyIt;
        }
    }
    for(auto selIt=sellOrders.begin();selIt!=sellOrders.end();){
        auto &orders=selIt->second;
        for(auto ordIt=orders.begin();ordIt!=orders.end();){
            if(ordIt->expiryTime!=-1 && ordIt->expiryTime<=currentTime){
                cout<<"ORDER EXPIRED: "<<ordIt->orderId<<"\n";
            }else{
                ++ordIt;
            }
        }
        if(orders.empty()){
            selIt=sellOrders.erase(selIt);
        }else{
            ++selIt;
        }
    }
}