#include "dpi/models.hpp"
#include <common/telnet_proxy.hpp>
#include <stdint.h>
#include <unistd.h>
#include <thread>
#include <condition_variable>
#include <iostream>
#include <queue>
#include <mutex>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/prctl.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>

bool Telnet_proxy::open_telnet_socket(int telnet_port)
{
  struct sockaddr_in addr;
  int yes = 1;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(telnet_port);
  addr.sin_addr.s_addr = INADDR_ANY;
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  this->telnet_socket = socket(PF_INET, SOCK_STREAM, 0);

  if(this->telnet_socket < 0)
  {
    //fprintf(stderr, "Unable to create comm socket: %s\n", strerror(errno));
    return false;
  }

  if(setsockopt(this->telnet_socket, SOL_SOCKET, SO_REUSEADDR, &yes,
        sizeof(int)) == -1) {
    //fprintf(stderr, "Unable to setsockopt on the socket: %s\n", strerror(errno));
    return false;
  }

  if(bind(this->telnet_socket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    //fprintf(stderr, "Unable to bind the socket: %s\n", strerror(errno));
    return false;
  }

  if(listen(this->telnet_socket, 1) == -1) {
    //fprintf(stderr, "Unable to listen: %s\n", strerror(errno));
    return false;
  }
  return true;
}

void Telnet_proxy::pop_byte(uint8_t *byte)
{
  std::unique_lock<std::mutex> lock(this->rx_mutex);
  while(rx_queue.empty())
  {
    rx_cond.wait(lock);
  }
  *byte = rx_queue.front();
  rx_queue.pop();
}

void Telnet_proxy::push_byte_from_proxy(uint8_t *byte)
{
  std::unique_lock<std::mutex> lock(this->rx_mutex);
  rx_queue.push(*byte);
  rx_cond.notify_one();
}

/**
 * Attempt to pop a byte from client
 * non blocking, returns -1 if no byte is availbale, 0 otherwise
 * 
 */
int Telnet_proxy::pop_byte_from_client(uint8_t *byte)
{
  std::unique_lock<std::mutex> lock(this->tx_mutex);
  if(tx_queue.empty())
  {
    return -1;
  }
  *byte = tx_queue.front();
  tx_queue.pop();
  return 0;
}

void Telnet_proxy::push_byte(uint8_t *byte)
{
  std::unique_lock<std::mutex> lock(this->tx_mutex);
  tx_queue.push(*byte);
}

void Telnet_proxy::listener(void)
{
  int client_fd;
  while(1)
  {
    if((client_fd = accept(telnet_socket, NULL, NULL)) == -1) {
      if(errno == EAGAIN) continue;
      std::cerr << "failed to accept connection" << strerror(errno) << std::endl;
      return;
    }
    std::cerr << "telnet connected" << std::endl;
    this->loop_thread = new std::thread(&Telnet_proxy::proxy_loop, this, client_fd);
    this->loop_thread->join();
  }
}

void Telnet_proxy::proxy_loop(int socket_fd)
{
  // structure to poll our socket
  struct pollfd poll_socket = {socket_fd, POLLIN, 0};
  // 10 ms timeout
  struct timespec ppoll_timeout = {0, 10000};
  sigset_t sigmask;
  // fetch old signal set
  sigprocmask(0, NULL, &sigmask);
  while(1)
  {
    int ret = 0;
    uint8_t byte = 0;

    ret = recv(socket_fd, (void *)&byte, sizeof(uint8_t), MSG_DONTWAIT);
    if(ret != sizeof(uint8_t))
    {
      if(ret == -1)
      {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
          ppoll(&poll_socket, 1, &ppoll_timeout, &sigmask);
        }
        else
        {
          std::cerr << "recv call ended with error " << strerror(errno) << std::endl;
          return;
        }
      }
      if(ret == 0)
      {
        std::cerr << "did not recv anything" << std::endl;
        return;
      }
    }
    else
    {
      this->push_byte_from_proxy(&byte);
    }

    if(!this->pop_byte_from_client(&byte))
    {
      ret = send(socket_fd, &byte, sizeof(uint8_t), 0);
      if(ret < 0)
      {
        std::cerr << "send call ended with error " << strerror(errno) << std::endl;
        return;
      }
    }
  }
}

Telnet_proxy::Telnet_proxy(int telnet_port)
{
  if (this->open_telnet_socket(telnet_port))
  {
    this->telnet_port = telnet_port;
    this->listener_thread = new std::thread(&Telnet_proxy::listener, this);
  }
  else
  {
    std::cerr << "open telnet port failed" << std::endl;
  }
}
