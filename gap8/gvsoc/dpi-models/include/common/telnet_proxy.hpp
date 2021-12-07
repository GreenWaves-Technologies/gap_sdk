#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

class Telnet_proxy
{
  public:
    void pop_byte(uint8_t*);
    void push_byte(uint8_t*);
    Telnet_proxy(int);
    
  private:
    bool open_telnet_socket(int port);
 
    std::mutex rx_mutex;
    std::mutex tx_mutex;
    std::condition_variable rx_cond;
    std::condition_variable tx_cond;
    std::queue<uint8_t> rx_queue;
    std::queue<uint8_t> tx_queue;
    void push_byte_from_proxy(uint8_t*);
    int pop_byte_from_client(uint8_t*);

    void listener(void);
    void proxy_loop(int);
    
    int telnet_socket;
    int telnet_port;
    
    std::thread *loop_thread;
    std::thread *listener_thread;
};
