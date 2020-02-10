#include <stdio.h>
#include "matching.h"


Order::Order(std::string i, double p, char s, int d)
{
    instrument = i;
    price = p;
    side = s;
    ID = d;
}

Order::~Order()
{
}

Book::Book(std::string inst)
{
    instrument = inst;
}    

Book::~Book()
{
    for(std::map<double, std::deque<Order*>*>::iterator it = buys.begin(); it != buys.end(); ++it)
    {
        if(!(it->second->empty()))
        {
            for(std::deque<Order*>::iterator jt = it->second->begin(); jt != it->second->end(); ++jt)
            {
                delete *jt;
            }
        }
        delete it->second;
    }

    for(std::map<double, std::deque<Order*>*>::iterator it = sells.begin(); it != sells.end(); ++it)
    {
        if(!(it->second->empty()))
        {
            for(std::deque<Order*>::iterator jt = it->second->begin(); jt != it->second->end(); ++jt)
            {
                delete *jt;
            }
        }
        delete it->second;
    }
}

void Book::add(Order* order)
{
    if(order->side=='B')
    {
        std::map<double, std::deque<Order*>*>::iterator found=sells.find(order->price);
        if(!(found==sells.end()) && !(found->second->empty()))
        {
            Order * match = found->second->front();
            found->second->pop_front();
            printf("Trade for %s price: %f oid %d matches oid %d\n", order->instrument.c_str(), order->price, order->ID, match->ID);
            delete order;
            delete match;
        }    
        else
        {
            found = buys.find(order->price);
            if(found==buys.end())
            {
                buys[order->price] = new std::deque<Order*>();
            }
            buys[order->price]->push_back(order);
        }
        
    }
    else if(order->side=='S')
    {
        std::map<double, std::deque<Order*>*>::iterator found=buys.find(order->price);
        if(!(found==buys.end()) && !(found->second->empty()))
        {
            Order * match = found->second->front();
            found->second->pop_front();
            printf("Trade for %s price: %f oid %d matches oid %d\n", order->instrument.c_str(), order->price, order->ID, match->ID);
            delete order;
            delete match;
        }
        else
        {
            found = sells.find(order->price);
            if(found==sells.end())
            {
                sells[order->price] = new std::deque<Order*>();
            }
            sells[order->price]->push_back(order);
        }
        
    }
}

Matcher::Matcher()
{
    numOrders = 0;
}

Matcher::~Matcher()
{
    for(std::map<std::string, Book*>::iterator it = books.begin(); it != books.end(); ++it)
    {
        delete it->second;
    }
}


void Matcher::incomingOrder(std::string inst, double price, char side)
{
    ++numOrders;
    Order * order = new Order(inst, price, side, numOrders);
    order->price = price;
    order->side;
    Book * book = findBook(inst);
    //printf("add order %s price %f side %c id %d\n", order->instrument.c_str(), order->price, order->side, order->ID);
    book->add(order);
}

Book * Matcher::findBook(std::string inst)
{
    Book * book = 0;
    std::map<std::string, Book*>::iterator found = books.find(inst);
    if(found==books.end())
    {
        book = new Book(inst);
        books[inst]=book;
    }
    else
    {
        book = found->second;
    }
    return book;
}

int main()
{
    Matcher * matcher = new Matcher();
    matcher->incomingOrder("AAPL", 100.00, 'B'); //1
    matcher->incomingOrder("AAPL", 100.00, 'S'); //2 Trade w/ 1
    
    matcher->incomingOrder("AAPL", 100.00, 'B'); //3
    matcher->incomingOrder("AAPL", 100.00, 'B'); //4
    matcher->incomingOrder("AAPL", 100.00, 'S'); //5 trade w/3
    matcher->incomingOrder("MSFT", 100.00, 'S');

    matcher->incomingOrder("AAPL", 98.85, 'S'); //6
    matcher->incomingOrder("MSFT", 12.34, 'S'); //7
    matcher->incomingOrder("AAPL", 98.95, 'S'); //8
    matcher->incomingOrder("MSFT", 54.32, 'B'); //9
    matcher->incomingOrder("AAPL", 98.95, 'B'); //10 trade w/8
    matcher->incomingOrder("MSFT", 12.34, 'B'); //11 trade w/7


    delete matcher;
    return 1;
} 