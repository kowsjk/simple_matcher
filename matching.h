#include <string>
#include <map>
#include <deque>

class Order
{
    public:
        Order(std::string i, double p, char s, int d);
        ~Order();//TODO
        std::string instrument;
        double price;
        char side;
        int ID;
};

class Book
{
    public:
        Book(std::string inst);
        ~Book();//TODO
        void add(Order* Order);

        std::map<double, std::deque<Order*>*> buys;
        std::map<double, std::deque<Order*>*> sells;
        std::string instrument;
};

class Matcher
{
    public:
        Matcher();//TODO
        ~Matcher();//TODO
        std::map<std::string,Book*> books;
        void incomingOrder(std::string inst, double price, char side);
        Book * findBook(std::string inst);
        int numOrders;
};