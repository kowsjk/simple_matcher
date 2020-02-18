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
    for(std::vector<Level*>::iterator it = buys.begin(); it != buys.end(); ++it)
    {
        delete *it;
    }

    for(std::vector<Level*>::iterator it = sells.begin(); it != sells.end(); ++it)
    {
        delete *it;
    }
}

void Book::insert(Order* order, std::vector<Level*>& side)
{
    Level * level = 0;
    std::vector<Level*>::iterator it = side.begin();
    for(;it != side.end();++it)
    {
        if(order->side == 'B')
        {
            if(order->price > (*it)->price)
            {
                level = new Level(order->price);
                side.insert(it, level);
            }
            else if(order->price == (*it)->price)
            {
                level = *it;
                break;
            }
        }
        else
        {
            if(order->price < (*it)->price)
            {
                level = new Level(order->price);
                side.insert(it, level);
            }
            else if(order->price == (*it)->price)
            {
                level = *it;
                break;
            }
        }
    }

    if(it==side.end())
    {
        level = new Level(order->price);
        side.push_back(level);
    }
    level->orders.push_back(order);
}

void Book::match(Order* order, std::deque<Order*>&  orders)
{
    while(order->size > 0 && !(orders.empty()))
    {
        Order * match = orders.front();
        if(order->size > match->size)
        {

            orders.pop_front();
            order->size -= match->size;
            printf("Trade1 for %s price: %f size %d oid %d matches oid %d\n", order->instrument.c_str(), match->price, match->size, order->ID, match->ID);
            delete match;
            
        }
        else if(match->size > order->size)
        {
            match->size -= order->size;
            printf("Trade2 for %s price: %f size %d oid %d matches oid %d\n", order->instrument.c_str(), match->price, order->size, order->ID, match->ID);
            order->size = 0;
        }
        else
        {
            orders.pop_front();
            printf("Trade3 for %s price: %f size %d oid %d matches oid %d\n", order->instrument.c_str(), match->price, order->size, order->ID, match->ID);
            order->size = 0;
            delete match;
        }
    }
}

void Book::add(Order* order)
{
    if(order->side=='B')
    {
        while(order->size > 0 && !sells.empty() && order->price >= sells[0]->price)
        {
            match(order, sells[0]->orders);
            if(sells[0]->orders.empty())
            {
                sells.erase(sells.begin());
            }
        }
        if(order->size > 0)
        {
            insert(order, buys);
        }
        else
        {
            delete order;
        }
    }
    else if(order->side=='S')
    {
        while(order->size > 0 && !buys.empty() && order->price >= buys[0]->price)
        {
            match(order, buys[0]->orders);
            if(buys[0]->orders.empty())
            {
                buys.erase(buys.begin());
            }
        }
        if(order->size > 0)
        {
            insert(order, sells);
        }
        else
        {
            delete order;
        }
    }
    else
    {
        //invalid order
        delete order;
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

Level::Level(double p)
{
    price = p;
}

Level::~Level()
{
    for(std::deque<Order*>::iterator it = orders.begin(); it != orders.end(); ++it)
    {
        delete *it;
    }
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
    matcher->incomingOrder("MSFT", 54.32, 100, 'B'); //12 trade w/10
    matcher->incomingOrder("AAPL", 98.95, 100, 'B'); //13 trade w/9
    matcher->incomingOrder("MSFT", 12.34, 100, 'B'); //14 


    delete matcher;
    return 1;
} 