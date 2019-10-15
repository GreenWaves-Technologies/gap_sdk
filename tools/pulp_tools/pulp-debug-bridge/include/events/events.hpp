// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#ifndef __EVENTS_H
#define __EVENTS_H

#include <memory>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <functional>
#include <utility>
#include <chrono>

#include "warnings.h"
#include "platform.h"

extern "C" {
#include "ae/ae.h"
}

class Barrier
{
public:
  Barrier(const Barrier&) = delete;
  Barrier& operator=(const Barrier&) = delete;
  explicit Barrier(unsigned int count) :
    m_count(count), m_generation(0), 
    m_count_reset_value(count)
  {
  }
  void count_down_and_wait()
  {
    std::unique_lock< std::mutex > lock(m_mutex);
    unsigned int gen = m_generation;
    if (--m_count == 0)
    {
      m_generation++;
      m_count = m_count_reset_value;
      m_cond.notify_all();
      return;
    }
    while (gen == m_generation)
      m_cond.wait(lock);
  }
private:
  std::mutex m_mutex;
  std::condition_variable m_cond;
  unsigned int m_count;
  unsigned int m_generation;
  unsigned int m_count_reset_value;
};

class EventLoopException: public std::exception
{
public:
  const char* what() const throw()
  {
    return "EventLoop error";
  }
};

enum FileEvents {
  None = AE_NONE,
  Readable = AE_READABLE,
  Writable = AE_WRITABLE,
  Both = AE_READABLE|AE_WRITABLE,
  NoChange = 0x80
};

/// Constant specifying that the timer is done and it will be removed.
constexpr int64_t kEventLoopTimerDone = AE_NOMORE;

typedef long long EventID;  // NOLINT

class EventLoop : public std::enable_shared_from_this<EventLoop> {
 public:
  class FileEvent;
  class TimerEvent;
  template <class T>
  class AsyncEvent;
  class Event;
  using SpFileEvent = std::shared_ptr<FileEvent>;
  using SpTimerEvent = std::shared_ptr<TimerEvent>;
  template <class T>
  using SpAsyncEvent = std::shared_ptr<AsyncEvent<T>>;
  using SpEventLoop = std::shared_ptr<EventLoop>;
  // Signature of the handler that will be called when there is a new event
  // on the file descriptor that this handler has been registered for.
  //
  // The arguments are the event flags (read or write).
  using FileCallback = std::function<FileEvents(socket_t, FileEvents)>;

  // This handler will be called when a timer times out. The return is the number of milliseconds the timer
  // shall be reset to or kEventLoopTimerDone if the timer shall not be
  // triggered again.
  using TimerEventCallback = std::function<int64_t()>;

  // This handler will be called when a timer times out. The timer id is
  // passed as an argument. The return is the number of milliseconds the timer
  // shall be reset to or kEventLoopTimerDone if the timer shall not be
  // triggered again.
  template <class T>
  using AsyncEventCallback = std::function<void(T message)>;

  using AsyncCallCompleteCallback = std::function<void(void * result)>;

  using AsyncCallCallback = std::function<void(AsyncCallCompleteCallback)>;

  using DeletedEventCallback = std::function<void()>;

  EventLoop(bool stop_when_empty = true, int loop_id = -1);
  ~EventLoop();

  static SpEventLoop getLoop(bool stop_when_empty = true, int loop_id = -1) {
    return std::make_shared<EventLoop>(stop_when_empty, loop_id);
  }

  class Event : public std::enable_shared_from_this<Event> {
    friend EventLoop;
    public:
      Event(EventLoop::SpEventLoop event_loop) : event_loop(event_loop) {}
      virtual ~Event() {};
    private:
      EventLoop::SpEventLoop event_loop;
  };

  class FileEvent: public Event {
    friend EventLoop;
    public:
      FileEvent(EventLoop::SpEventLoop event_loop, socket_t fd, FileCallback callback);
      ~FileEvent();
      void setEvents(FileEvents events);
      const char * str_event(FileEvents ev);
      void on_event(const FileCallback& callback) { this->callback = std::move(callback); }
    private:
      socket_t fd;
      FileCallback callback;
      aeFileEvent ae_fe;
      bool is_set = false;
  };

  class DeletableEvent : public Event {
    friend EventLoop;
    public:
      DeletableEvent(EventLoop::SpEventLoop event_loop) : Event(event_loop) {}
      virtual ~DeletableEvent() {};
  };

  class TimerEvent: public DeletableEvent {
    friend EventLoop;
    public:
      TimerEvent(EventLoop::SpEventLoop event_loop, TimerEventCallback callback);
      ~TimerEvent();
      void setTimeout(int64_t usecs);
      void setTimeout(std::chrono::microseconds &usecs);
      void on_event(TimerEventCallback callback) { this->callback = callback; }
    private:
      aeTimeEvent ae_te;
      TimerEventCallback callback;
      bool is_set = false;
  };

  template <class T>
  class AsyncEventMsg {
  public:
    AsyncEventMsg(const AsyncEventCallback<T> &callback, const T &msg) : callback(std::move(callback)), msg(std::move(msg)) {}
    AsyncEventCallback<T> callback;
    T msg;
  };

  template <class T>
  class AsyncEvent: public Event {
    friend EventLoop;
    public:
      AsyncEvent(EventLoop::SpEventLoop event_loop) : Event(event_loop), callback(nullptr) {}
      AsyncEvent(EventLoop::SpEventLoop event_loop, AsyncEventCallback<T> callback) : Event(event_loop), callback(std::move(callback)) {}
      AsyncEvent(EventLoop::SpEventLoop event_loop, AsyncEventCallback<T> callback, T msg) : Event(event_loop), callback(std::move(callback)) {
        trigger(msg);
      }
      ~AsyncEvent() {}
      void on_event(AsyncEventCallback<T> callback) { this->callback = callback; }
      void trigger(const T& msg) {
        // the callback should be set at this point
        if (callback == nullptr) throw EventLoopException();
        // The async msg class is used to move out the callback function and the message
        // so once sent the AsyncEvent can actually be destroyed
        AsyncEventMsg<T> * ae_msg = new AsyncEventMsg<T>(callback, msg);
        if (aeCreateAsyncEvent(
            event_loop->loop_,
            EventLoop::asyncEventCallback<T>,
            reinterpret_cast<void*>(ae_msg)
          ) != AE_OK)
          throw EventLoopException();
      }
    private:
      AsyncEventCallback<T> callback;
  };

  SpFileEvent getFileEvent(socket_t fd, FileCallback callback=nullptr ) {
    return std::make_shared<FileEvent>(shared_from_this(), fd, callback); 
  }

  SpTimerEvent getTimerEvent(TimerEventCallback callback = nullptr, int64_t usecs = -1) {
    auto te = std::make_shared<TimerEvent>(shared_from_this(), callback);
    if (usecs >= 0) te->setTimeout(usecs);
    return te;
  }

  template <class T>
  SpAsyncEvent<T> getAsyncEvent(AsyncEventCallback<T> callback, T msg) {
    auto ae = std::make_shared<AsyncEvent<T>>(shared_from_this(), callback);
    ae->trigger(msg);
    return ae;
  }

  template <class T>
  SpAsyncEvent<T> getAsyncEvent(AsyncEventCallback<T> callback = nullptr) {
    auto ae = std::make_shared<AsyncEvent<T>>(shared_from_this(), callback); 
    return ae;
  }

  /// \brief Run the event loop.
  void start();

  /// \brief Stop the event loop
  void stop();

  // hangs the thread that it is called on. Calls the function on the other thread and returns when the function has completed
  void * asyncCall(const AsyncCallCallback& callback);

  void terminate_when_empty(bool yes);
  
  int get_loop_id() { return loop_id; }
private:
  void cleanup();
  static void fileEventCallback(aeEventLoop* loop, socket_t fd, void* context, int events);
  template <class T>
  static void asyncEventCallback(aeEventLoop* UNUSED(event_loop), void* context, int cancelled) {
    AsyncEventMsg<T> *ev_msg = reinterpret_cast<AsyncEventMsg<T>*>(context);
    if (!cancelled)
      ev_msg->callback(ev_msg->msg);
    delete ev_msg;
  }
  static long long timerEventCallback(aeEventLoop* loop, void* context);
  static void eventDeletedCallback(aeEventLoop* loop, void* context);
  static void asyncCallCallback(aeEventLoop* loop, void* context, aeAsyncCallState *state);
  static void loopStopCallback(aeEventLoop* loop);
  aeEventLoop* loop_;
  int loop_id;
  std::unordered_map<Event *, std::shared_ptr<Event>> active_events;
};

#endif  // __EVENTS_H