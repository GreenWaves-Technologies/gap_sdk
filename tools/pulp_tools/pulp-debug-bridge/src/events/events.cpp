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

#include "events.hpp"

constexpr int kInitialEventLoopSize = 1024;

EventLoop::EventLoop(bool stop_when_empty, int loop_id) : loop_id(loop_id) {
  loop_ = aeCreateEventLoop(kInitialEventLoopSize, reinterpret_cast<void*>(this), (stop_when_empty?AE_DEFAULT_FLAGS:AE_DEFAULT_FLAGS&~AE_STOP_WHEN_EMPTY), EventLoop::loopStopCallback);
}

EventLoop::~EventLoop() {
  // Eventloop should be stopped before disposal
  assert(loop_->stop);
  aeDeleteEventLoop(loop_);
}

// AsyncCall uses an Aync event to call a closure in the loop thread blocking the caller
// until it completes.


void * EventLoop::asyncCall(const EventLoop::AsyncCallCallback& callback) {
  // keep a copy of the callback in the stack of this function
  // since it is not needed after this returns
  EventLoop::AsyncCallCallback local_callback(std::move(callback));
  return aeAsyncCall(loop_, EventLoop::asyncCallCallback, reinterpret_cast<void*>(&local_callback));
}

void EventLoop::terminate_when_empty(bool yes) {
  aeStopWhenEmpty(loop_, (yes?1:0));
}

void EventLoop::start() { aeMain(loop_); }

void EventLoop::stop() {
  aeStop(loop_);
}

void EventLoop::cleanup() {
  active_events.clear();
}

void EventLoop::loopStopCallback(aeEventLoop* event_loop) {
  aeCancelPendingCalls(event_loop);
  reinterpret_cast<EventLoop *>(event_loop->loopdata)->cleanup();
}

void EventLoop::fileEventCallback(aeEventLoop* UNUSED(event_loop), socket_t fd, void* context, int events) {
  FileEvent *evt = reinterpret_cast<FileEvent*>(context);
  FileEvents new_events;
  if (evt->callback) {
    new_events = (evt->callback)(fd, (FileEvents) events);
  } else {
    new_events = None;
  }
  if (new_events != NoChange) evt->setEvents(new_events);
}

long long EventLoop::timerEventCallback(aeEventLoop* UNUSED(event_loop), void* context) {
  TimerEvent *evt = reinterpret_cast<TimerEvent*>(context);
  // printf("Timer callback timer %p\n", &evt->ae_te);
  if (evt->callback) {
    return (evt->callback)();
  } else {
    return kEventLoopTimerDone;
  }
}

void EventLoop::asyncCallCallback(aeEventLoop* UNUSED(event_loop), void* context, aeAsyncCallState *state) {
  EventLoop::AsyncCallCallback *cb = reinterpret_cast<AsyncCallCallback*>(context);
  (*cb)([state](void * result){ aeAsyncCallComplete(state, result); });
}

void EventLoop::eventDeletedCallback(aeEventLoop* UNUSED(ae_event_loop), void* context) {
  Event * ev = reinterpret_cast<Event*>(context);
  // printf("Delete callback timer %p\n", &((TimerEvent *)ev)->ae_te);
  ev->event_loop->active_events.erase(reinterpret_cast<Event*>(context));
}

EventLoop::FileEvent::FileEvent(EventLoop::SpEventLoop event_loop, socket_t fd, FileCallback callback) : Event(event_loop), fd(fd), callback(std::move(callback)) {
  assert(event_loop);
}

EventLoop::FileEvent::~FileEvent() {
  // printf("FileEvent disposing .... %d\n", fd);
  setEvents(None);
}

const char * EventLoop::FileEvent::str_event(FileEvents ev) {
  if (ev == (Readable|Writable)) return "Read|Write";
  if (ev == (Readable)) return "Read";
  if (ev == (Writable)) return "Write";
  if (ev == (None)) return "None";
  return "Unknown!";
}

void EventLoop::FileEvent::setEvents(FileEvents events) {
  // printf("fileevent set %d %s %p\n", fd, str_event(events), &ae_fe);
  if (events == None) {
    if (is_set) {
      // printf("call delete file event\n");
      is_set = false;
      aeDeleteFileEvent(event_loop->loop_, &ae_fe);
      event_loop->active_events.erase(static_cast<Event *>(this));
    }
  } else {
    if (!is_set) {
      if (aeCreateFileEvent(event_loop->loop_, &ae_fe, fd, events, EventLoop::fileEventCallback, reinterpret_cast<void*>(this)) != AE_OK)
        throw EventLoopException();
      is_set = true;
      Event * ev = static_cast<Event *>(this);
      event_loop->active_events.emplace(std::make_pair(ev, ev->shared_from_this()));
    } else {
      if (aeSetMaskFileEvent(event_loop->loop_, &ae_fe, events) != AE_OK)
        throw EventLoopException();
    }
  }
}

EventLoop::TimerEvent::TimerEvent(EventLoop::SpEventLoop event_loop, TimerEventCallback callback) : DeletableEvent(event_loop), callback(std::move(callback)) {
}

EventLoop::TimerEvent::~TimerEvent() {
  setTimeout(-1);
}

void EventLoop::TimerEvent::setTimeout(int64_t usecs) {
  // printf("set timeout: id %p usecs %ld\n", &ae_te, usecs);
  if (usecs < 0) {
    if (is_set) {
      is_set = false;
      // printf("delete time event\n");
      aeDeleteTimeEvent(event_loop->loop_, &ae_te);
    }
  } else {
    if (!is_set) {
      aeCreateTimeEvent(event_loop->loop_, &ae_te, EventLoop::timerEventCallback, reinterpret_cast<void*>(this), EventLoop::eventDeletedCallback, reinterpret_cast<void*>((DeletableEvent *)this));
      event_loop->active_events.emplace(std::make_pair(this, static_cast<Event *>(this)->shared_from_this()));
      is_set = true;
    }
    aeSetTimeoutTimeEvent(event_loop->loop_, &ae_te, usecs);
  }
}

void EventLoop::TimerEvent::setTimeout(std::chrono::microseconds &usecs) {
  setTimeout(usecs.count());
}




