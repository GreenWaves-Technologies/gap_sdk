/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna and GreenWaves Technologies SA
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Authors: Martin Croome, GreenWaves Technologies (martin.croome@greenwaves-technologies.com)
 */

#include "tcp-events.hpp"

void dump(char *buf, int len) {
  if (len > 16) len = 16;
  for (int i=0; i<len; i++)
    printf("%02hhx ", buf[i]);
  printf("... \n");
}

int get_socket_error(socket_t fd)
{
  int err;
  socklen_t len = sizeof(err);
  if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &((char *)&err)[0], &len) == SOCKET_ERROR) {
    return SOCKERRNO;
  }
  return err;
}

bool set_blocking(socket_t fd, bool blocking)
{
#ifdef _WIN32
  unsigned long mode = blocking ? 0 : 1;
  return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? true : false;
#else
  if (fd < 0) {
    return false;
  }
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) {
    return false;
  }

  flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);

  return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
#endif
}

Tcp_socket_owner::Tcp_socket_owner(Log *log, EventLoop::SpEventLoop el)
  : log(log), el(el)
{
  socket_init();
}

Tcp_socket_owner::~Tcp_socket_owner() {
  close_all();
  socket_deinit();
}

bool Tcp_socket_owner::socket_init()
{
  bool res;
  #ifdef _WIN32
    res = ::WSAStartup(MAKEWORD(1,1), &m_wsa_data) == 0;
  #else
    res = true;
  #endif
  return res;
}

bool Tcp_socket_owner::print_error(const char * err_str)
{
  int err_num = SOCKERRNO;
  if (err_num != SOCKERR_WOULDBLOCK && err_num != SOCKERR_INPROGRESS) {
    log->error(err_str, err_num);
    return true;
  }
  return false;
}

void Tcp_socket_owner::socket_deinit()
{
  #ifdef _WIN32
  ::WSACleanup();
  #endif
}

void Tcp_socket_owner::client_disconnected(socket_t socket) {
  auto iter = owned_sockets.find(socket);
  bool found = iter != owned_sockets.end();
  log->protocol("Socket owner socket disconnected (fd %d, found %d)\n", socket, found);
  if (found) {
    ev_timer = el->getTimerEvent([this, socket](){
      log->protocol("Socket owner socket disconnected async (fd %d)\n", socket);
      // at this point we loose the reference
      auto iter = owned_sockets.find(socket);
      if (iter != owned_sockets.end()) {
        tcp_socket_ptr_t p_socket = iter->second;
        emit_disconnected(p_socket);
        owned_sockets.erase(iter);
      }
      return kEventLoopTimerDone;
    });
    ev_timer->setTimeout(0);
  }
};

tcp_socket_ptr_t Tcp_socket_owner::client_connected(socket_t socket) {
  log->protocol("Socket owner socket connected (fd %d)\n", socket);
  // the socket gets a copy of a shared pointer to this so we don't get collected
  // before it has finished
  auto ptr = std::make_shared<Tcp_socket>(this->shared_from_this(), socket);
  owned_sockets[socket] = ptr;
  emit_connected(ptr);
  return ptr;
};

void Tcp_socket_owner::close_all() {
  std::vector<std::shared_ptr<Tcp_socket>> to_close = {};
  for (auto socket_pair : owned_sockets) {
    to_close.push_back(std::move(socket_pair.second));
  }
  for (auto socket : to_close) {
    socket->close_immediate();
  }
}


Tcp_client::Tcp_client(Log * log, EventLoop::SpEventLoop el) : Tcp_socket_owner(log, el)
{

}

Tcp_client::~Tcp_client()
{
}

socket_t Tcp_client::prepare_socket(socket_t fd, const char * address, int port, struct sockaddr_in *addr, bool blocking) {
  struct hostent *he;

  if (socket_count()>0)
    throw TcpException("Client is already connected");

  if (fd != INVALID_SOCKET)
    throw TcpException("Client is already connecting");

  fd = socket(PF_INET, SOCK_STREAM, 0);
  if(fd == INVALID_SOCKET)
  {
    print_error("unable to create socket - error %d\n");
    throw TcpException("unable to create socket");
  }

  if((he = gethostbyname(address)) == NULL) {
    print_error("unable to find host - error %d\n");
    throw TcpException("unable to find host");
  }

  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);
  addr->sin_addr = *((struct in_addr *)he->h_addr_list[0]);
  memset(addr->sin_zero, '\0', sizeof(addr->sin_zero));

  set_blocking(fd, blocking);

  return fd;
}

tcp_socket_ptr_t Tcp_client::connect_blocking(const char * address, int port, int timeout_ms)
{
  struct sockaddr_in addr;
  fd_set fdset;
  struct timeval tv;

  try {
    connecting_socket = prepare_socket(connecting_socket, address, port, &addr, true);
  } catch (TcpException &ex) {
    log->error("unable to connect - %s", ex.what());
    return nullptr;
  }

  int res = ::connect(connecting_socket, (struct sockaddr *)&addr, sizeof(addr));
  if (res == SOCKET_ERROR) {
    print_error("unable to connect - error %d\n");
#ifdef _WIN32
    ::closesocket(connecting_socket);
#else
    ::close(connecting_socket);
#endif
    connecting_socket = INVALID_SOCKET;
    return nullptr;
  }

  FD_ZERO(&fdset);
  FD_SET(connecting_socket, &fdset);
  tv.tv_sec = timeout_ms / 1000;
  tv.tv_usec = (timeout_ms % 1000) * 1000;

  if (select(connecting_socket + 1, NULL, &fdset, NULL, &tv) == 1)
  {
    int err = get_socket_error(connecting_socket);
    if (err != 0) {
      log->error("unable to connect: error %d\n", err);
#ifdef _WIN32
      ::closesocket(connecting_socket);
#else
      ::close(connecting_socket);
#endif
      connecting_socket = INVALID_SOCKET;
      return nullptr;
    }
  } else {
    log->error("unable to connect: connection timed out\n");
#ifdef _WIN32
    ::closesocket(connecting_socket);
#else
    ::close(connecting_socket);
#endif
    connecting_socket = INVALID_SOCKET;
    return nullptr;
  }

  auto ptr = std::make_shared<Tcp_socket>(this->shared_from_this(), connecting_socket);
  owned_sockets[connecting_socket] = ptr;
  return ptr;
}

void Tcp_client::connect(const char * address, int port, int timeout_ms)
{
  struct sockaddr_in addr;

  connecting_socket = prepare_socket(connecting_socket, address, port, &addr, false);

  strncpy(connecting_address, address, 512);
  connecting_port = port;

  log->user("Connecting to (%s:%d)\n", address, port);

  EventLoop::SpFileEvent fe = el->getFileEvent(connecting_socket);

  auto te = el->getTimerEvent([this, &fe] () {
    log->error("unable to connect: connection timed out\n");
    if (connecting_socket != INVALID_SOCKET) close(connecting_socket);
    connecting_socket = INVALID_SOCKET;
    emit_connected(nullptr);
    fe->setEvents(None);
    return kEventLoopTimerDone;
  }, timeout_ms * 1000);

  // note: we are capturing fe by copy so it must be initialized
  fe->on_event([this, te] (socket_t sock, FileEvents UNUSED(events)) {
    // printf("set events none ....\n");
    int err = get_socket_error(sock);
    if (err != 0) {
      te->setTimeout(0);
      log->error("unable to connect: error %d\n", err);
      return None;
    }
    te->setTimeout(-1);
    client_connected(connecting_socket);
    return None;
  });

  fe->setEvents(Writable);
  ::connect(connecting_socket, (struct sockaddr *)&addr, sizeof(addr));
}

Tcp_listener::Tcp_listener(Log *log, EventLoop::SpEventLoop el, port_t port)
  : Tcp_socket_owner(log, el), port(port)
{
}

void Tcp_listener::start() {
  if (ev_incoming) {
    return;
  }
  struct sockaddr_in addr;

// #ifndef _WIN32
  int yes = 1;
// #endif

  log->debug("Tcp_listener started (running %d)\n", this->is_running);

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  socket_in = socket(PF_INET, SOCK_STREAM, 0);
  if(socket_in == INVALID_SOCKET)
  {
    print_error("Unable to create comm socket: %d\n");
    throw TcpException("unable to create socket comm socket");
  }

#ifndef _WIN32 // see http://itamarst.org/writings/win32sockets.html
  if(setsockopt(socket_in, SOL_SOCKET, SO_REUSEADDR, (const char*) &yes, sizeof(int)) == -1) {
    print_error("Unable to setsockopt on the socket: %d\n");
    throw TcpException("unable to setsockopt on the socket");
  }
#endif

  if(setsockopt(socket_in, IPPROTO_TCP, TCP_NODELAY, (const char*) &yes, sizeof(int)) == -1) {
    print_error("Unable to setsockopt on the socket: %d\n");
    throw TcpException("unable to setsockopt on the socket");
  }

  if(bind(socket_in, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    print_error("Unable to bind the socket: %d\n");
    throw TcpException("unable to bind the socket");
  }

  if(listen(socket_in, 1) == -1) {
    print_error("Unable to listen: %d\n");
    throw TcpException("unable to listen on socket");
  }

  ev_incoming = el->getFileEvent(socket_in, [&] (socket_t UNUSED(fd), FileEvents events) {
    if (!(events&Readable)) return Writable;
    accept_socket();
    return NoChange;
  });
  ev_incoming->setEvents(Readable);

  this->is_running = true;
  this->is_stopping = false;

  log->user("RSP server setup on port %d\n", port);
  emit_state_change(ListenerStarted);
}

void Tcp_listener::accept_socket()
{
  socket_t socket_client = INVALID_SOCKET;
  if((socket_client = accept(socket_in, NULL, NULL)) == INVALID_SOCKET)
  {
    if (!print_error("Tcp_listener: Unable to accept connection: %d\n"))
      return;
  }

  log->user("Tcp_listener: client connected (fd %d)\n", socket_client);

  if (!set_blocking(socket_client, false)) {
    print_error("Tcp_listener: unable to set non blocking: %d\n");
#ifdef _WIN32
    ::closesocket(socket_client);
#else
    ::close(socket_client);
#endif
    return;
  }

  if (connected_listener_count()> 0) {
    log->debug("Tcp_listener: call connected callback\n");
    client_connected(socket_client);
    log->debug("Tcp_listener: connected callback returns (is_running: %d)\n", is_running);
  } else {
    log->debug("Tcp_listener: no connected callback - closing socket\n");
#ifdef _WIN32
    ::closesocket(socket_client);
#else
    ::close(socket_client);
#endif
  }
}

void Tcp_listener::stop()
{
  if (this->is_stopping) return;
  this->is_stopping = true;
  log->debug("Tcp_listener stopped (running %d)\n", this->is_running);
  if (this->is_running) {
    close_all();
    this->is_running = false;
    ev_incoming->setEvents(None);
#ifdef _WIN32
    ::closesocket(socket_in);
#else
    ::close(socket_in);
#endif
  }
  this->is_stopping = false;
  emit_state_change(ListenerStopped);
}

const char * Tcp_socket::socket_state_str(TcpSocketState state) {
  switch(state) {
    case SocketClosed:
      return "Closed";
    // case SocketClosing:
    //   return "Closing";
    case SocketOpen:
      return "Open";
    case SocketShutDown:
      return "ShutDown";
    case SocketShuttingDown:
      return "ShuttingDown";
    default:
      return "Bad State Value!";
  }
}

Tcp_socket::Tcp_socket(std::shared_ptr<Tcp_socket_owner> owner, socket_t socket, size_t read_size, size_t write_size) :
  owner(owner), socket(socket), in_buffer(std::make_shared<CircularCharBuffer>(read_size)),
  out_buffer(std::make_shared<CircularCharBuffer>(write_size)), high(write_size * 90 / 100), low(write_size * 70 / 100)
{
  this->owner->log->protocol("Tcp_socket created (fd %d)\n", socket);
  // There are 2 events on each socket. One for events on the socket and a
  // timer event to separate dispatch of events back to the user.
  // The events are disabled at the start to allow the user to setup callbacks
  // set_events(Readable) should then be called
  ev_socket = owner->el->getFileEvent(socket, [this](socket_t UNUSED(fd), FileEvents fd_events){
    this->owner->log->protocol("Socket event called (fd %d event %d, state %s)\n", this->socket, fd_events, socket_state_str(this->state));
    bool trigger_timer = false;
    FileEvents saved_socket_events = this->socket_events;
    // if (state == SocketClosed || state == SocketClosing || state == SocketShutDown) {
    if (state == SocketShutDown) {
      this->set_events(None);
    } if (state == SocketShuttingDown) {
      this->socket_shutdown();
    } else {
      if (fd_events & Writable) {
        trigger_timer = this->socket_writable() || trigger_timer;
      }
      if (state == SocketOpen && (fd_events & Readable)) {
        trigger_timer = this->socket_readable() || trigger_timer;
      }
      this->owner->log->protocol("Trigger timer %d\n", trigger_timer);
      if (state == SocketOpen && trigger_timer) this->ev_socket_timeout->setTimeout(0);
    }
    this->owner->log->protocol("Socket event returns (fd %d, new event state %d, trigger timer %d, state %s)\n", this->socket, this->socket_events, trigger_timer, socket_state_str(this->state));
    return (saved_socket_events != this->socket_events?this->socket_events:NoChange);
  });
  ev_socket_timeout = owner->el->getTimerEvent([this](){
    this->owner->log->protocol("Timeout called (fd %d, state %s)\n", this->socket, socket_state_str(this->state));
    int ret = kEventLoopTimerDone;
    // if (this->state == SocketClosing) {
    //   this->state = SocketClosed;
    //   if (this->closed_cb) this->closed_cb();
    //   this->owner->client_disconnected(this->socket);
    //   this->socket = INVALID_SOCKET;
    // } else
    if (state == SocketShutDown) {
      this->close();
    } if (state == SocketOpen) {
      ret = (this->socket_timeout()?0:kEventLoopTimerDone);
    }
    this->owner->log->protocol("Timeout callback returns (fd %d, ret %d, state %s)\n", this->socket, ret, socket_state_str(this->state));
    return ret;
  });
}

Tcp_socket::~Tcp_socket() {
  owner->log->protocol("socket disposing (fd %d)\n", socket);
  if (socket != INVALID_SOCKET)
    this->close_immediate();
}


void Tcp_socket::socket_shutdown()
{
  while (1) {
    char buf[1024];
    func_ret_t ret = ::recv(socket, buf, 1024, 0);
    if (ret == SOCKET_ERROR || ret == 0) {
      owner->log->debug("Socket shutdown (ret %d)\n", ret);
      state = SocketShutDown;
      this->close();
      break;
    }
  }
}

bool Tcp_socket::socket_writable()
{
  // printf("writable %lu\n", out_buffer->size());
  bool trigger_timer = false;
  // Drain the write buffer
  while (!out_buffer->is_empty()) {
    char * buf;
    size_t len;
    out_buffer->read_block((void**)&buf, &len);
    // printf("writing\n");
    func_ret_t ret = ::send(socket, (const char *)buf, len, 0);
    if (ret == SOCKET_ERROR) {
      int plat_err_no;
      if (this->check_error(&plat_err_no)) {
        emit_error(plat_err_no);
        this->close_immediate();
        return false;
      } else {
        break;
      }
    } else {
      out_buffer->commit_read(ret);
    }
  }
  // If the user has selected writable then check if we keep allowing writing
  if (user_events&Writable) {
    // write should be flowing or we drained to low water
    if (write_flowing || out_buffer->size() <= low) {
      write_flowing = true;
      trigger_timer = true;
    }
  }
  // If the write buffer is empty no point in listening for events
  if (out_buffer->is_empty()) {
    socket_events = (FileEvents) (socket_events & ~Writable);
  }
  return trigger_timer;
}

bool Tcp_socket::socket_readable()
{
  bool trigger_timer = false;
  // Try to read as much as possible
  while (!in_buffer->is_full()) {
    char * buf;
    size_t len;
    in_buffer->write_block((void**)&buf, &len);
    func_ret_t ret = ::recv(socket, &buf[0], len, 0);
    if (ret == 0) {
      state = SocketShutDown;
      this->close();
      return false;
    } else if (ret == SOCKET_ERROR) {
      int plat_err_no;
      if (this->check_error(&plat_err_no)) {
        emit_error(plat_err_no);
        this->close_immediate();
        return false;
      } else {
        break;
      }
    } else {
      //dump(buf, len);
      in_buffer->commit_write(ret);
    }
  }
  if (user_events&Readable) {
    if (!in_buffer->is_empty()) {
      trigger_timer = true;
    }
  }
  if (in_buffer->size() > high) {
    read_flowing = false;
  }
  if (!(user_events&Readable)||!read_flowing) {
    socket_events = (FileEvents) (socket_events & ~Readable);
  }
  return trigger_timer;
}

bool Tcp_socket::after_write()
{
  bool redo = false;
  // maybe switch on write socket events
  if (!out_buffer->is_empty()&&!(socket_events&Writable)) {
    socket_events = (FileEvents) (socket_events | Writable);
  }
  // redo another timer event
  if ((user_events&Writable)&&(write_flowing||out_buffer->size()<low)) {
    redo = true;
    write_flowing = true;
  }
  // printf("after write ..... %d\n", socket_events);
  return redo;
}

bool Tcp_socket::after_read()
{
  bool redo = false;
  // maybe there is some room to read now
  if ((user_events&Readable)&&(read_flowing||in_buffer->size()<low)&&!(socket_events&Readable)) {
    read_flowing = true;
    socket_events = (FileEvents) (socket_events | Readable);
  }
  if ((user_events&Readable)&&!in_buffer->is_empty()) {
    redo = true;
  }
  // printf("after read ..... %d\n", socket_events);
  return redo;
}

void Tcp_socket::read_buffer(const data_cb_t& cb) {
  FileEvents current_socket_events = socket_events;
  if (cb) cb(this->shared_from_this(), this->in_buffer);
  this->after_read();
  if (current_socket_events != socket_events)
    ev_socket->setEvents(socket_events);
}

void Tcp_socket::write_buffer(const data_cb_t& cb) {
  FileEvents current_socket_events = socket_events;
  if (cb) cb(this->shared_from_this(), this->out_buffer);
  this->after_write();
  if (current_socket_events != socket_events)
    ev_socket->setEvents(socket_events);
}

ssize_t Tcp_socket::read_immediate(void * buf, size_t len, bool read_all) {
  ssize_t i = 0;
  while (true) {
    func_ret_t ret = ::recv(socket, &((char*)buf)[i], len - i, 0);
    if (ret == 0) {
      state = SocketShutDown;
      this->ev_socket_timeout->setTimeout(0);
      return -1;
    } else if (ret == SOCKET_ERROR) {
      int plat_err_no;
      if (this->check_error(&plat_err_no)) {
        emit_error(plat_err_no);
        this->ev_socket_timeout->setTimeout(0);
        return -1;
      } else {
        break;
      }
    } else {
      i+=ret;
      if (!read_all||i>=(ssize_t)len) break;
    }
  }
  return i;
}

ssize_t Tcp_socket::write_immediate(const void * buf, size_t len, bool write_all) {
  ssize_t i = 0;
  while (true) {
    func_ret_t ret = ::send(socket, &((char*)buf)[i], len - i, 0);
    if (ret == SOCKET_ERROR) {
      int plat_err_no;
      if (this->check_error(&plat_err_no)) {
        emit_error(plat_err_no);
        this->ev_socket_timeout->setTimeout(0);
        return -1;
      } else {
        break;
      }
    } else {
      i+=ret;
      if (!write_all||i>=(ssize_t)len) break;
    }
  }
  return i;
}

bool Tcp_socket::socket_timeout()
{
  // printf("Socket timeout\n");
  bool redo = false;

  // record current socket events to spot changes
  FileEvents current_socket_events = socket_events;
  // if there is aset callback and writing is enabled by user and there
  // is space to write
  if ((user_events&Writable) && out_buffer->is_empty()) {
    emit_write(this->shared_from_this(), out_buffer);
    if (state != SocketOpen) return true;
    redo = this->after_write() || redo;
  }
  // printf("redo after write %d\n", redo);
  // If there is a read callback and reading is enabled and we have something to read
  if ((user_events&Readable) && !in_buffer->is_empty()) {
    emit_read(this->shared_from_this(), in_buffer);
    if (state != SocketOpen) return true;
    redo = this->after_read() || redo;
  }
  // printf("redo after read %d\n", redo);
  // try again since the read callback may have changed the socket to Writeable
  redo = ((user_events&Writable) && out_buffer->is_empty()) || redo;
  if (current_socket_events != socket_events)
    ev_socket->setEvents(socket_events);

  return redo;
}

bool Tcp_socket::check_error(int * plat_err_no)
{
#ifdef _WIN32
  if ((*plat_err_no = WSAGetLastError()) != WSAEWOULDBLOCK) {
#else
  *plat_err_no = errno;
  if (*plat_err_no != EWOULDBLOCK && state == SocketOpen) {
#endif
    owner->log->error("Error on client (fd %d, err %d) - closing\n", socket, *plat_err_no);
    state = SocketShutDown;
    return true;
  } else {
    return false;
  }
}

bool Tcp_socket::set_blocking(bool blocking) {
  return ::set_blocking(this->socket, blocking);
}

void Tcp_socket::close()
{
  if (state == SocketOpen) {
    owner->log->debug("Try to shutdown socket (fd %d)\n", socket);
    if (::shutdown(socket, LST_SHUT_WR) != SOCKET_ERROR) {
      owner->log->debug("Socket shutdown in progress (fd %d)\n", socket);
      state = SocketShuttingDown;
      socket_events = Readable;
      ev_socket->setEvents(socket_events);
    } else {
      state = SocketShutDown;
    }
  }
  if (state == SocketShutDown) {
    close_immediate();
    // owner->log->debug("Socket shutdown - closing (fd %d)\n", socket);
    // if (socket != INVALID_SOCKET) {
    //   ::close(socket);
    // }
    // state = SocketClosing;
    // socket_events = None;
    // ev_socket->setEvents(socket_events);
    // ev_socket_timeout->setTimeout(0);
  }
}

void Tcp_socket::close_immediate()
{
  owner->log->debug("Closing socket immediately (fd %d)\n", socket);
  if (socket != INVALID_SOCKET) {
#ifdef _WIN32
    ::closesocket(socket);
#else
    ::close(socket);
#endif
  }
  state = SocketClosed;
  socket_events = None;
  ev_socket->setEvents(socket_events);
  ev_socket_timeout->setTimeout(-1);
  emit_closed();
  owner->client_disconnected(this->get_handle());
  socket = INVALID_SOCKET;
}

void Tcp_socket::set_events(FileEvents new_events) {
  // printf("set events %d\n", new_events);
  if (user_events == new_events) return;
  FileEvents old_events = user_events;
  FileEvents saved_socket_events = socket_events;
  bool trigger_timer = false;
  user_events = new_events;
  if ((user_events&Writable)&&!(old_events&Writable)) {
    if (!out_buffer->is_full()) {
      // printf("trigger timer\n");
      write_flowing = true;
      trigger_timer = true;
    }
    if (!out_buffer->is_empty()) {
      socket_events = (FileEvents) (socket_events|Writable);
    }
  }
  if ((user_events&Readable)&&!(old_events&Readable)) {
    if (!in_buffer->is_full()) {
      read_flowing = true;
      socket_events = (FileEvents) (socket_events|Readable);
    }
    if (!in_buffer->is_empty()) {
      trigger_timer = true;
    }
  }
  // printf("set timer %d\n", trigger_timer);
  if (trigger_timer) ev_socket_timeout->setTimeout(0);
  // printf("set socket events saved %d new %d\n", saved_socket_events, socket_events);
  if (saved_socket_events != socket_events) ev_socket->setEvents(socket_events);
}
