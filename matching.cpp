#include <stdio.h>
#include "matching.h"


Order::Order(std::string i, double p, char s, int sz, int d)
{
    instrument = i;
    price = p;
    side = s;
    ID = d;
    size = sz;
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

void Book::insert(Order* order, std::map<double, std::deque<Order*>*>& side)
{
    std::map<double, std::deque<Order*>*>::iterator found = side.find(order->price);
    if(found==side.end())
    {
        side[order->price] = new std::deque<Order*>();
    }
    side[order->price]->push_back(order);
}

void Book::match(Order* order, std::map<double, std::deque<Order*>*>::iterator &found)
{
    while(order->size > 0 && !(found->second->empty()))
    {
        Order * match = found->second->front();
        if(order->size > match->size)
        {

            found->second->pop_front();
            order->size -= match->size;
            printf("Trade1 for %s price: %f size %d oid %d matches oid %d\n", order->instrument.c_str(), order->price, match->size, order->ID, match->ID);
            delete match;
            
        }
        else if(match->size > order->size)
        {
            match->size -= order->size;
            printf("Trade2 for %s price: %f size %d oid %d matches oid %d\n", order->instrument.c_str(), order->price, order->size, order->ID, match->ID);
            order->size = 0;
        }
        else
        {
            found->second->pop_front();
            printf("Trade3 for %s price: %f size %d oid %d matches oid %d\n", order->instrument.c_str(), order->price, order->size, order->ID, match->ID);
            order->size = 0;
            delete match;
        }
    }
    if(order->size > 0)
    {
        insert(order, order->side=='B' ? buys : sells);
    }
    else
    {
        delete order;
    }
    
}

void Book::add(Order* order)
{
    if(order->side=='B')
    {
        std::map<double, std::deque<Order*>*>::iterator found=sells.find(order->price);
        if(!(found==sells.end()) && !(found->second->empty()))
        {
            match(order, found);
        }    
        else
        {
            insert(order, buys);
        }
        
    }
    else if(order->side=='S')
    {
        std::map<double, std::deque<Order*>*>::iterator found=buys.find(order->price);
        if(!(found==buys.end()) && !(found->second->empty()))
        {
            match(order, found);
        }
        else
        {
            insert(order, sells);
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


void Matcher::incomingOrder(std::string inst, double price, int size, char side)
{
    ++numOrders;
    Order * order = new Order(inst, price, side, size, numOrders);
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
    matcher->incomingOrder("AAPL", 100.00, 100, 'B'); //1
    matcher->incomingOrder("AAPL", 100.00, 100, 'S'); //2 Trade w/ 1@100
    
    matcher->incomingOrder("AAPL", 100.00, 100, 'B'); //3
    matcher->incomingOrder("AAPL", 100.00, 100, 'B'); //4
    matcher->incomingOrder("AAPL", 100.00, 50, 'S'); //5 Trade w/ 3@50
    matcher->incomingOrder("AAPL", 100.00, 100, 'S'); //6 Trade w/3@50 && w/4@50
    matcher->incomingOrder("AAPL", 100.00, 100, 'S'); //7 Trade w/ 4@50
    matcher->incomingOrder("AAPL", 100.00, 100, 'B'); //8 Trade w/ 7@50

    matcher->incomingOrder("AAPL", 98.85, 100, 'S'); //9
    matcher->incomingOrder("MSFT", 12.34, 100, 'S'); //10
    matcher->incomingOrder("AAPL", 98.95, 100, 'S'); //11
    matcher->incomingOrder("MSFT", 54.32, 100, 'B'); //12
    matcher->incomingOrder("AAPL", 98.95, 100, 'B'); //13 trade w/ 11
    matcher->incomingOrder("MSFT", 12.34, 100, 'B'); //14 trade w/ 10


    delete matcher;
    return 1;
} 