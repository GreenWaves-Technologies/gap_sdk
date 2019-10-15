#include "events/events.hpp"

#include <iostream>
#include <string>
#include <memory>
#include <thread>

class State {
public:
    State() {
        loop1 = EventLoop::getLoop(false);
        loop2 = EventLoop::getLoop(false);
    }
    int ae1_cnt = 0, ae2_cnt = 0;
    EventLoop::SpAsyncEvent<int> ae1;
    EventLoop::SpAsyncEvent<std::string> ae2;
    EventLoop::SpEventLoop loop1, loop2;
};

void thread(std::shared_ptr<State> s) {
    std::cout << "Thread starting\n";
    s->loop2->start();
    std::cout << "Thread ending\n";
}

int main( int argc, const char* argv[] )
{
    auto s = std::make_shared<State>();

    s->ae1 = s->loop1->getAsyncEvent<int>([&](int msg){
        std::cout << "Hi from loop 1 - " << msg << " \n";
        s->ae1_cnt++;
        s->ae2->trigger("my message");
        if (s->ae1_cnt > 5) s->loop1->stop();
    });

    s->ae2 = s->loop2->getAsyncEvent<std::string>([&](std::string msg){
        std::cout << "Hi from loop 2 - " << msg << " \n";
        s->ae2_cnt++;
        s->ae1->trigger(2);
        if (s->ae2_cnt > 5) s->loop2->stop();
    }, "boot message");

    std::thread t1(thread, s);
    std::cout << "Main loop starting\n";
    s->loop1->start();
    std::cout << "Main loop ending\n";

    t1.join();

    std::cout << "Test Passed\n";

    return 0;
}