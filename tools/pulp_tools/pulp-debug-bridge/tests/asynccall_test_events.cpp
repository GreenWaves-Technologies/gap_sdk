#include "events/events.hpp"

#include <iostream>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>

class State {
public:
    State() {
        loop1 = EventLoop::getLoop(true, 1);
        loop2 = EventLoop::getLoop(false, 2);
    }
    int ae1_cnt = 0, ae2_cnt = 0;
    EventLoop::SpEventLoop loop1, loop2;
};

void thread(const State& s) {
    std::cout << "# Thread starting " << std::this_thread::get_id() << "\n";
    s.loop2->start();
    std::cout << "# Thread ending\n";
}


void call2(State * s, EventLoop::AsyncCallCompleteCallback complete) {
    s->ae2_cnt++;
    std::cout << "# Hello from loop2 - count " << s->ae2_cnt << "\n";
    EventLoop::SpTimerEvent te_loop2 = s->loop2->getTimerEvent([complete](){
        std::cout << "# Hello from loop2 - completed wait\n";
        complete((void*)1);
        return (kEventLoopTimerDone);
    });
    te_loop2->setTimeout(100);
}

void close2(State * s, EventLoop::AsyncCallCompleteCallback complete) {
    std::cout << "# Stopping loop 2 on thread\n";
    s->loop2->stop();
    std::cout << "# Return from stop 2 on thread\n";
    complete((void*)1);
}

int main( int argc, const char* argv[] )
{
    using namespace std::placeholders;

    State s{};
    State * p_s = &s;

    std::cout << "event 1";
    EventLoop::SpTimerEvent te1 = s.loop1->getTimerEvent([&](){
        std::cout << "# bootstrap call\n";
        void * res = s.loop2->asyncCall(std::bind(call2, p_s, _1));
        std::cout << "# after call res " << res << "\n";
        return kEventLoopTimerDone;
    });
    te1->setTimeout(0);   
    std::cout << "event 2";
    EventLoop::SpTimerEvent te2 = s.loop1->getTimerEvent([&](){
        // have to stop loop2 on its thread before we stop
        std::cout << "# Call stop 2 on main thread\n";
        s.loop2->asyncCall(std::bind(close2, p_s, _1));
        std::cout << "# Return from stop 2 on main thread\n";
        s.loop1->stop();
        return kEventLoopTimerDone;
    });
    te2->setTimeout(200000);   
    std::thread t1(thread, s);
    std::cout << "# Main loop starting\n";
    s.loop1->start();
    std::cout << "# Main loop ending\n";

    t1.join();

    std::cout << "# Test Passed\n";

    return 0;
}