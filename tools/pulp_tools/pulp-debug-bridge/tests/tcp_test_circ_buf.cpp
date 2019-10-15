#include <stdlib.h>
#include <tuple>
#include <iostream>

#include <string.h>

#include "events/events.hpp"
#include "events/tcp-events.hpp"

void write_buf(CircularCharBuffer& buf, size_t cnt) {
    std::cout << "Writing " << cnt << " size " << buf.size() << "\n";
    size_t i = 0;
    while (i++<cnt) {
        assert(buf.write_copy("testing", strlen("testing")+1) == 8);
    }
    std::cout << "After writing " << cnt << " size " << buf.size() << "\n";
}

void read_buf(CircularCharBuffer& buf, size_t cnt) {
    std::cout << "Reading " << cnt << " size " << buf.size() << "\n";
    size_t i = 0;
    while (i++<cnt) {
#ifndef NDEBUG
        char cbuf[8];
        assert(buf.read_copy(cbuf, 8) == 8);
        assert(!strcmp(cbuf, "testing"));
#endif
    }
    std::cout << "After reading " << cnt << " size " << buf.size() << "\n";
}


int main( int argc, const char* argv[] )
{
    CircularCharBuffer buf(800);

    write_buf(buf, 80);
    read_buf(buf, 80);
    write_buf(buf, 80);
    read_buf(buf, 80);
    write_buf(buf, 80);
    read_buf(buf, 80);
    write_buf(buf, 80);
    read_buf(buf, 60);
    assert(buf.size()==20*8);
    std::cout << "Test Passed\n";
    return 0;
}