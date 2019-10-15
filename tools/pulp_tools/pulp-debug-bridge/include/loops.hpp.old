/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies SA
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
 * Authors: Martin Croome, GWT (martin.croome@greenwaves.technologies.com)
 */

#ifndef __LOOPS_H
#define __LOOPS_H

#include <stdint.h>
#include <list>
#include <queue>
#include <memory>
#include <functional>
#include <exception>
#include <unordered_map>
#include <mutex>
#include <atomic>

#include "debug_bridge/debug_bridge.h"
#include "cables/cable.hpp"
#include "events/events.hpp"
#include "events/emitter.hpp"

#if defined(__USE_SDL__)
#include <SDL.h>
#include <thread>
#endif

#define PTR_2_INT(__addr) ((unsigned int)(reinterpret_cast<std::uintptr_t>(__addr)&0xffffffff))
#define INT_2_PTR(__addr) (reinterpret_cast<std::uintptr_t>((size_t)__addr))

#define LOOP_DEFAULT_LOOP_USECS 10000
#define LOOP_DEFAULT_SLOW_LOOP_USECS 10000000

class LoopCableException: public std::exception
{
public:
  const char* what() const throw()
  {
    return "Exception accessing cable";
  }
};

enum LooperFinishedStatus {
  LooperFinishedContinue,
  LooperFinishedPause,
  LooperFinishedStop,
  LooperFinishedStopAll
};

class LoopManager;

class Looper {
public:
  Looper(LoopManager * top) : m_top(top) {}
  virtual ~Looper() {}
  virtual LooperFinishedStatus loop_proc(hal_debug_struct_t * debug_struct) = 0;
  virtual LooperFinishedStatus register_proc(hal_debug_struct_t * debug_struct) = 0;
  bool get_paused() { return m_paused; }
  void set_paused(bool paused) { m_paused = paused; }
  virtual void destroy() { m_destroyed = true; }
  virtual void flush(hal_debug_struct_t *) {}
protected:
  LoopManager * m_top;
  bool m_paused = false, m_destroyed = false;
};

SMART_EMITTER(ProgramExit, exit)
SMART_EMITTER(AvailabilityChange, availability_change)

class LoopManager : public ProgramExitEmitter<int32_t>, public AvailabilityChangeEmitter<uint32_t> {
public:
  LoopManager(const EventLoop::SpEventLoop &event_loop, std::shared_ptr<Cable> cable, unsigned int debug_struct_addr, 
      int64_t slow_usecs = LOOP_DEFAULT_SLOW_LOOP_USECS, int64_t fast_usecs = LOOP_DEFAULT_LOOP_USECS, bool check_exit = true);
  ~LoopManager();
  void set_debug_struct_addr(unsigned int debug_struct_addr);
  void start(bool fast);
  void set_loop_speed(bool fast);
  void flush();
  void stop();
  void add_looper(const std::shared_ptr<Looper> &looper);
  void remove_looper(Looper * looper);
  void clear_loopers() {
    stop();
    for (auto l: m_loopers) l->destroy();
    m_loopers.clear();
  }
  void access(bool write, unsigned int addr, int len, char * buf);
  int64_t run_loops();
  Log log;
#ifdef __NEW_REQLOOP__
  void target_state_sync(hal_target_state_t * target);
  bool get_target_available() { return m_target.available != 0; }
#endif
private:
  hal_debug_struct_t * activate();
  bool program_has_exited(hal_debug_struct_t *debug_struct);
  EventLoop::SpTimerEvent m_loop_te;
  std::list<std::shared_ptr<Looper>> m_loopers;

  std::shared_ptr<Cable> m_cable;     
  unsigned int m_debug_struct_addr;
  int64_t m_slow_usecs, m_fast_usecs, m_cur_usecs;
  bool m_stopped = true, m_check_exit;

#ifdef __NEW_REQLOOP__
  bool target_is_available();
  hal_target_state_t m_target;
#endif
};

#if defined(__USE_SDL__)
class Reqloop;

SMART_EMITTER(FramebufferStart, start)

SMART_EMITTER(FramebufferWindowOpen, window_open)

class Framebuffer : 
  public FramebufferStartEmitter<>,
  public FramebufferWindowOpenEmitter<uint32_t>
{
public:
  class UpdateMessage;
  class Window {
  public:
    Window(const std::string &name, int width, int height, int format) :
      m_name(name), m_width(width), m_height(height), m_format(format) {}
    ~Window() { destroy(); }
    void handle_display();
    void handle_update(UpdateMessage * update_message);
    void destroy();
    int get_width() { return m_width; }
    int get_height() { return m_height; }
    int get_pixel_size() { return m_pixel_size; }
  private:
    std::string m_name;
    int m_width;
    int m_height;
    int m_format;
    int m_pixel_size = 1;
    SDL_Surface *m_screen;
    SDL_Texture * m_texture;
    SDL_Renderer *m_renderer;
    SDL_Window *m_window;
    uint32_t *m_pixels;
    bool m_destroyed = true;
  };

  class UpdateMessage {
    friend Window;
  public:
    UpdateMessage(int posx, int posy, int width, int height, int pixel_size) : 
      m_posx(posx), m_posy(posy), m_width(width), m_height(height), m_buffer(width*height*pixel_size) {}
    ~UpdateMessage() {}
    char * get_buffer() { return (char *) &m_buffer[0]; }
    size_t get_buffer_size() { return m_buffer.capacity(); }
  private:
    int m_posx, m_posy, m_width, m_height;
    std::vector<uint8_t> m_buffer;
  };

  Framebuffer(Reqloop *reqloop);
  void destroy();
  void handle_display(uint32_t window_id);
  void handle_update(uint32_t window_id, void * vmsg);
  bool update(uint32_t window_id, uint32_t addr, int posx, int posy, int width, int height);
  void start();
  void open(std::string name, int width, int height, int format);

private:
  std::shared_ptr<Framebuffer::Window> find_window(uint32_t window_id);
  void fb_routine();

  Reqloop *m_top;
  std::thread * m_thread;
  std::mutex m_mut_windows;
  std::unordered_map<uint32_t, std::shared_ptr<Window>> m_windows;
  uint32_t m_next_window_id = 1;
  uint32_t m_display_window, m_update_window;
  bool m_destroyed = false;
  std::atomic<bool> m_running{false};
};
#endif

class Reqloop : public Looper, public std::enable_shared_from_this<Reqloop>
{
#if defined(__USE_SDL__)
  friend Framebuffer;
#endif
public:
  Reqloop(LoopManager * top, const EventLoop::SpEventLoop &event_loop, int64_t req_pause = 0);
#ifdef __USE_SDL__
  ~Reqloop() {
    if (m_active_timer) m_active_timer->setTimeout(-1);
    if (m_frame_buffer) m_frame_buffer->destroy();
  }
#endif
  LooperFinishedStatus loop_proc(hal_debug_struct_t *debug_struct);
  LooperFinishedStatus register_proc(hal_debug_struct_t *debug_struct);

  class FileReqState {
    friend Reqloop;
  public:
    FileReqState(uint32_t fd, uint32_t size, uint32_t ptr);
    FileReqState() = default;
    bool do_write(LoopManager * top);
    bool do_read(LoopManager * top);
  private:
    uint32_t m_fd, m_size, m_iter_size, m_ptr;
    int m_res;
  };

private:
  enum ReqloopFinishedStatus {
    ReqloopFinishedContinue,
    ReqloopFinishedMoreReqs,
    ReqloopFinishedCompletingReq,
    ReqloopFinishedStop,
    ReqloopFinishedStopAll
  };
  void reply_req(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  ReqloopFinishedStatus handle_req_connect(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  ReqloopFinishedStatus handle_req_open(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  int64_t handle_req_read_one(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  ReqloopFinishedStatus handle_req_read(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  int64_t handle_req_write_one(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  ReqloopFinishedStatus handle_req_write(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  ReqloopFinishedStatus handle_req_close(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  ReqloopFinishedStatus handle_req_fb_open(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req);
  void complete_req_fb_open(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req);
  void complete_req_fb_window_open(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req, uint32_t window_id);
  ReqloopFinishedStatus handle_req_fb_update(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req);
  ReqloopFinishedStatus handle_req_disconnect(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  ReqloopFinishedStatus handle_req(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  ReqloopFinishedStatus handle_one_req(hal_debug_struct_t *debug_struct);
#ifdef __NEW_REQLOOP__
  ReqloopFinishedStatus handle_req_target_status_sync(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req);
#endif

  Log log;
  EventLoop::SpEventLoop m_event_loop;
  EventLoop::SpTimerEvent m_active_timer = nullptr;
  bool m_has_error = false;
  int64_t m_req_pause = 50;

  FileReqState m_fstate;
#ifdef __USE_SDL__
  Framebuffer * m_frame_buffer = nullptr;
#endif
};

class Ioloop : public Looper
{
public:
  Ioloop(LoopManager * top);
  ~Ioloop() {}
  LooperFinishedStatus loop_proc(hal_debug_struct_t *debug_struct);
  LooperFinishedStatus register_proc(hal_debug_struct_t *debug_struct);
  void set_max_loops(int max_loops) { m_max_loops = max_loops; }
  void flush(hal_debug_struct_t *debug_struct);
private:
  uint32_t print_len(hal_debug_struct_t *debug_struct);
  void print_one(hal_debug_struct_t *debug_struct, uint32_t len);
  LooperFinishedStatus print_many(hal_debug_struct_t *debug_struct, int cnt);
  int m_max_loops = 10;
  Log log;
};
#endif
