
#include "stdio.h"
#include "cables/cable.hpp"

class MockCable : public Cable {
    public:
        MockCable() : Cable(NULL) {}
        bool access(bool write, unsigned int, int len, char* buf) {
            if (write) {
                m_write += len;
            } else {
                m_read += len;
                memset(buf, 0, len);
            }
            printf("send string\n");
            return false; 
        }
        void get_res(int * write, int * read) { *write = m_write; *read = m_read; }
    private:
        int m_write = 0, m_read = 0;
};