#include <atomic>
#include <iostream>

class order {
public: 
    order() { id=c.fetch_add(1); }
    int id;
private:
    static std::atomic<int> c;
};

std::atomic<int> order::c(0);

int main() {
    order *o1 = new order();
    order *o2 = new order();
    std::cout << o1->id << std::endl; // Expect 0
    std::cout << o2->id << std::endl; // Expect 1
}
