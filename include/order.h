#ifndef ORDER_H
#define ORDER_H
#include<bits/stdc++.h>
using namespace std;

struct Order
{
    int orderId;
    bool isBuy;
    int quantity;
    double price;
    bool isMarket;
    bool isStop;
    double stopPrice;
    bool isIOC;
    bool isFOK;
    long long timestamp;
    long long expiryTime;
    bool isIceberg;
    int displayQuantity;
    int hiddenQuantity;

    Order(int id,bool buy,int qty,double p,long time,bool market=false,long long exp=-1,bool ioc=false,bool fok=false,bool stop=false,double stp=0,bool iceberg=false,int displayQty=0){
        orderId=id;
        isBuy=buy;
        quantity=qty;
        price=p;
        timestamp=time;
        isMarket=market;
        isIOC=ioc;
        isFOK=fok;
        isStop=stop;
        stopPrice=stp;
        expiryTime=exp;
        isIceberg=iceberg;
        if(isIceberg){
            displayQuantity=min(qty,displayQty);
            hiddenQuantity=qty-displayQuantity;
        }else{
            displayQuantity=qty;
            hiddenQuantity=0;
        }
    }
    static Order fromString(const string& message, long long currentTime) {
        std::stringstream ss(message);
        int id, qty, displayQty;
        double p, stp;
        int buy, market, ioc, fok, stop, iceberg;
        long long exp;

        ss >> id >> buy >> qty >> p >> market >> exp >> ioc >> fok >> stop >> stp >> iceberg >> displayQty;

        return Order(id, buy == 1, qty, p, currentTime, market == 1, exp, ioc == 1, fok == 1, stop == 1, stp, iceberg == 1, displayQty);
    }
};

#endif