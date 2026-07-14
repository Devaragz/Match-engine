#ifndef TRADE_H
#define TRADE_H

struct Trade
{
    int buyOrderId;
    int sellOrderId;
    int quantity;
    double price;

    Trade(int buuId,int sellId,int qty,double p){
        buyOrderId=buuId;
        sellOrderId=sellId;
        quantity=qty;
        price=p;
    }
};

#endif