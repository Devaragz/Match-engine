#include "../include/MatchEngine.h"
#include<iostream>
using namespace std;

MatchEngine::MatchEngine() : isRunning(true) {
    tradeLog.open("trades.txt",std::ios::app);
    workerThread = std::thread(&MatchEngine::processLoop, this);
}

MatchEngine::~MatchEngine() {
    isRunning = false;
    // Push dummy order to wake up condition variable
    orderQueue.push(Order(-1, false, 0, 0, 0)); 
    if(workerThread.joinable()) {
        workerThread.join();
    }
    if(tradeLog.is_open()){
        tradeLog.close();
    }
}

void MatchEngine::submitOrder(Order newOrder) {
    orderQueue.push(newOrder);
}

void MatchEngine::processLoop() {
    Order currentOrder(-1, false, 0, 0, 0);
    while(isRunning) {
        orderQueue.wait_and_pop(currentOrder);
        if(!isRunning || currentOrder.orderId == -1) break; 
        
        std::lock_guard<std::mutex> lock(engineMutex);
        processOrder(currentOrder);
    }
}

void MatchEngine::processOrder(Order newOrder){
    cout<<"Processing Order "<<newOrder.orderId<<"\n";
    if(newOrder.isFOK){
        if(!canFullyFill(newOrder)){
            cout<<"FOK ORDER CANCELLED: "<<newOrder.orderId<<"\n";
            return;
        }
    }
    if(newOrder.isBuy){
        while(book.hasSellOrders() && newOrder.quantity>0 && (newOrder.isMarket || newOrder.price>=book.getBestSellPrice())){
            Order &sellOrder=book.getFirstSellOrder();
            int trqty=min(newOrder.quantity,sellOrder.quantity);
            double tradeprice=sellOrder.price;
            tradeHistory.push_back(Trade(newOrder.orderId,sellOrder.orderId,trqty,tradeprice));
            cout<<"TRADE EXECUTED\n";
            totalVolume+=trqty;
            totalTradedValue+=(tradeprice*trqty);
            if(tradeLog.is_open()){
                tradeLog<<"BUY:"<<newOrder.orderId<<" | SELL:"<<sellOrder.orderId<<" | QTY:"<<trqty<<" | PRICE:"<<tradeprice<<"\n";
                tradeLog.flush();
            }
            cout<<"Buy Order: "<<newOrder.orderId<<"\n";
            cout<<"Sell Order: "<<sellOrder.orderId<<"\n";
            cout<<"Quantity: "<<trqty<<"\n";
            checkStopOrders(sellOrder.price);
            sellOrder.quantity-=trqty;
            if(sellOrder.isIceberg){
                sellOrder.displayQuantity-=trqty;
                if(sellOrder.displayQuantity==0 && sellOrder.hiddenQuantity>0){
                    int refill=min(100,sellOrder.hiddenQuantity);
                    sellOrder.displayQuantity=refill;
                    sellOrder.hiddenQuantity-=refill;
                }
            }
            newOrder.quantity-=trqty;
            if(sellOrder.quantity==0){
                book.removeBestSellOrder();
            }
        }
        if(newOrder.quantity>0){
            if(newOrder.isIOC){
                cout<<"IOC BUY cancelled remaining: "<<newOrder.quantity<<"\n";
            }else{
                cout<<"Remaining Buy Quantity: "<<newOrder.quantity<<"\n";
                if(!newOrder.isMarket){
                    book.adddOrder(newOrder);
                }
            }
            }
    }else{
        while(book.hasBuyOrders() && newOrder.quantity>0 && (newOrder.isMarket || newOrder.price<=book.getBestBuyPrice())){
            Order &buyOrder=book.getFirstBuyOrder();
            int trqty=min(newOrder.quantity,buyOrder.quantity);
            double tradeprice=buyOrder.price;
            tradeHistory.push_back(Trade(buyOrder.orderId,newOrder.orderId,trqty,tradeprice));
            cout<<"TRADE EXECUTED\n";
            totalVolume+=trqty;
            totalTradedValue+=(tradeprice*trqty);
            if (tradeLog.is_open()) {
                tradeLog<<"BUY:"<<buyOrder.orderId<<" | SELL:"<<newOrder.orderId<<" | QTY:"<<trqty<<" | PRICE:"<<tradeprice<<"\n";
                tradeLog.flush(); 
            }
            cout<<"Buy Order: "<<buyOrder.orderId<<"\n";
            cout<<"Sell Order: "<<newOrder.orderId<<"\n";
            cout<<"Quantity: "<<trqty<<"\n";
            checkStopOrders(buyOrder.price);
            buyOrder.quantity-=trqty;
            if(buyOrder.isIceberg){
                buyOrder.displayQuantity-=trqty;
                if(buyOrder.displayQuantity==0 && buyOrder.hiddenQuantity>0){
                    int refill=min(100,buyOrder.hiddenQuantity);
                    buyOrder.displayQuantity=refill;
                    buyOrder.hiddenQuantity-=refill;
                }
            }
            newOrder.quantity-=trqty;
            if(buyOrder.quantity==0){
                book.removeBestBuyOrder();
            }
        }
        if(newOrder.quantity>0){
            if(newOrder.isIOC){
                cout<<"IOC SELL cancelled remaining: "<<newOrder.quantity<<"\n";
            }else{
                cout<<"Remaining Sell Quantity: "<<newOrder.quantity<<"\n";
                if(!newOrder.isMarket){
                    book.adddOrder(newOrder);
                }
            }
        }
    }
}

void MatchEngine::printOrderBoook(){
    std::lock_guard<std::mutex> lock(engineMutex);
    book.printBook();
}

void MatchEngine::printTrades(){
    cout<<"\n==== TRADE HISTORY ====\n";
    for(const auto &trade:tradeHistory){
        cout<<"BUY:"<<trade.buyOrderId<<" | SELL:"<<trade.sellOrderId<<" | QTY:"<<trade.quantity<<" | PRICE:" <<trade.price<<"\n";
    }
}

bool MatchEngine::cancelOrder(int orderId){
    std::lock_guard<std::mutex> lock(engineMutex);
    if(book.cancelOrder(orderId)){
        cout<<"Order "<<orderId<<" cancelled\n";
        return true;
    }
    cout<<"Order not found\n";
    return false;
}

bool MatchEngine::modifyOrder(int orderId,double newPrice,int newQuantity){
    std::lock_guard<std::mutex> lock(engineMutex);
    bool success=book.modifyOrder(orderId,newPrice,newQuantity);
    if(success){
        cout<<"Order "<<orderId<<" modified\n";
    }
    return success;
}

void MatchEngine::addStopOrder(Order order){
    stopOrders.push_back(order);
    cout<<"STOP ORDER STORED: "<<order.orderId<<"\n";
}

void MatchEngine::checkStopOrders(double lastTradePrice){
    for(auto it=stopOrders.begin();it!=stopOrders.end();){
        bool trigger=false;
        if(it->isBuy && lastTradePrice>=it->stopPrice){
            trigger=true;
        }
        if(!it->isBuy && lastTradePrice<=it->stopPrice){
            trigger=true;
        }
        if(trigger){
            cout<<"STOP ORDER TRIGGERED: "<<it->orderId<<"\n";
            Order triggerOrder=*it;
            triggerOrder.isStop=false;
            triggerOrder.isMarket=true;
            it=stopOrders.erase(it);
            processOrder(triggerOrder);
        }else{
            ++it;
        }
    }
}

void MatchEngine::removeExpiredOrders(long long currentTime){
    book.removeExpiredOrders(currentTime);
}

bool MatchEngine::canFullyFill(Order order){
    int available=0;
    if(order.isBuy){
        OrderBook tempBook=book;
        while(tempBook.hasSellOrders()){
            Order &sell=tempBook.getFirstSellOrder();
            if(!order.isMarket && sell.price>order.price){
                break;
            }
            available+=sell.quantity;
            tempBook.removeBestSellOrder();
            if(available>=order.quantity){
                return true;
            }
        }
    }else{
        OrderBook tempBook=book;
        while(tempBook.hasBuyOrders()){
            Order &buy=tempBook.getFirstBuyOrder();
            if(!order.isMarket && buy.price<order.price){
                break;
            }
            available+=buy.quantity;
            tempBook.removeBestBuyOrder();
            if(available>=order.quantity){
                return true;
            }
        }
    }
    return false;
}

void MatchEngine::printAnalytics() {
    std::lock_guard<std::mutex> lock(engineMutex);
    cout << "\n=== ENGINE ANALYTICS ===\n";
    cout << "Total Volume Traded : " << totalVolume << "\n";
    if (totalVolume > 0) {
        double vwap = totalTradedValue / totalVolume;
        cout << "VWAP                : " << vwap << "\n";
    } else {
        cout << "VWAP                : 0.0\n";
    }
}