#include <stdlib.h>
#include <memory>
#include <iostream>

#include "events/events.hpp"

class State {
    public:
        int i = 0;
};

int main( int argc, const char* argv[] )
{
    auto _loop = EventLoop::getLoop();
    auto s = std::make_shared<State>();

    EventLoop::SpTimerEvent ev = _loop->getTimerEvent([&]()->int { 
        printf("%d - Hello, World\n", s->i);
        s->i += 1;
        return (s->i < 10 ? 100 : kEventLoopTimerDone);
    });
    ev->setTimeout(100);
    _loop->start();
    std::cout << "Test Passed\n";
    return 0;
}