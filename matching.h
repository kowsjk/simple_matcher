#include <string>
#include <map>
#include <deque>
#include <vector>


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

class Level //TODO
{
    public:
        Level(double p);
        ~Level();
        double price;
        std::deque<Order*>  orders;
};

class Book
{
    public:
        Book(std::string inst);
        ~Book(); 
        void add(Order* Order);
        std::string instrument;
        std::vector<Level*> buys;
        std::vector<Level*> sells;
        void insert(Order* order, std::vector<Level*>& side); 
        void match(Order* order, std::deque<Order*>&  orders);
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

