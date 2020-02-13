#include <string>
#include <map>
#include <deque>

class Order
{
    public:
        Order(std::string i, double p, char s, int sz, int d);
        ~Order();
        std::string instrument;
        double price;
        char side;
        int size;
        int ID;
};

class Book
{
    public:
        Book(std::string inst);
        ~Book();
        void add(Order* Order);
        void insert(Order* order, std::map<double, std::deque<Order*>*>& side);
        void match(Order* order, std::map<double, std::deque<Order*>*>::iterator &found);

        std::map<double, std::deque<Order*>*> buys;
        std::map<double, std::deque<Order*>*> sells;
        std::string instrument;
};

class Matcher
{
    public:
        Matcher();
        ~Matcher();
        std::map<std::string,Book*> books;
        void incomingOrder(std::string inst, double price, int size, char side);
        Book * findBook(std::string inst);
        int numOrders;
};