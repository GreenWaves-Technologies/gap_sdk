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

#ifndef __BRIDGE_COMMANDS_H
#define __BRIDGE_COMMANDS_H

#include <memory>
#include <queue>
#include <stack>
#include <chrono>
#include <exception>
#include <list>
#include "events/emitter.hpp"
#include "events/events.hpp"
#include "reqloop.hpp"
#include "bridge-state.hpp"

class BridgeCommandsException : public std::exception {
};

class BridgeUnmatchedLoopException : public BridgeCommandsException {
public:
    const char* what() const throw() { return "Unmatched startloop or endloop command"; }
};

SMART_EMITTER(BridgeCommandsExit, exit)

class BridgeCommands : public std::enable_shared_from_this<BridgeCommands>, public BridgeCommandsExitEmitter<> {
  public:
    using bridge_func_t = std::function<int(void *)>;
    using bridge_cont_complete_func_t = std::function<void(int)>;
    using bridge_cont_func_t = std::function<void(void *, bridge_cont_complete_func_t)>;
    using SpBridgeCommands = std::shared_ptr<BridgeCommands>;

    class BridgeCommand : public std::enable_shared_from_this<BridgeCommand> {
    public:
        virtual ~BridgeCommand() { };
        virtual int64_t execute(SpBridgeCommands bc) = 0;
    };

    class BridgeCommandExecute : public BridgeCommand {
    public:
        ~BridgeCommandExecute() {}
        BridgeCommandExecute(const bridge_func_t& cb) : m_cb(std::move(cb)) {}
        int64_t execute(SpBridgeCommands bc);
    private:
        bridge_func_t m_cb;
    };

    class BridgeCommandExecuteAsync : public BridgeCommand {
    public:
        ~BridgeCommandExecuteAsync() {}
        BridgeCommandExecuteAsync(const bridge_cont_func_t& cb, const bridge_cont_complete_func_t& cb_complete) : 
            m_cb(std::move(cb)), m_cb_complete(std::move(cb_complete)) {}
        int64_t execute(SpBridgeCommands bc);
    private:
        bridge_cont_func_t m_cb;
        bridge_cont_complete_func_t m_cb_complete;
    };

    class BridgeCommandDelay : public BridgeCommand {
    public:
        ~BridgeCommandDelay() {}
        BridgeCommandDelay(std::chrono::microseconds duration) : m_duration(duration) {}
        int64_t execute(SpBridgeCommands UNUSED(bc)) { return m_duration.count(); };
    private:
        std::chrono::microseconds m_duration;
    };

    class BridgeCommandCollection : public BridgeCommand {
    public:
        ~BridgeCommandCollection() { while (!m_command_queue.empty()) m_command_queue.pop(); }
        BridgeCommandCollection() {}
        BridgeCommandCollection(const std::queue<std::shared_ptr<BridgeCommand>> &command_queue) : m_command_queue(std::move(command_queue)) {}
        virtual int64_t execute(SpBridgeCommands bc);
        void add_command(const std::shared_ptr<BridgeCommand> &command) { m_command_queue.emplace(command); }
    protected:
        std::queue<std::shared_ptr<BridgeCommand>> m_command_queue;
        bool m_started = false;
    };

    class BridgeCommandRepeat : public BridgeCommandCollection {
    public:
        ~BridgeCommandRepeat() {}
        BridgeCommandRepeat(std::chrono::microseconds delay, int repeat_times) : m_delay(delay), m_repeat_times(repeat_times) {}
        int64_t execute(SpBridgeCommands bc);
    private:
        std::chrono::microseconds m_delay;
        int m_repeat_times;
    };

    class BridgeCommandWaitExit : public BridgeCommand {
    public:
        ~BridgeCommandWaitExit() {
        }
        BridgeCommandWaitExit(SpBridgeCommands bc);
        int64_t execute(SpBridgeCommands bc);
    private:
        bool m_waiting = false;
        std::weak_ptr<BridgeCommands> m_bc;
    };

    BridgeCommands(BridgeState * state) :
        m_state(state) {
            m_command_stack.push(std::make_shared<BridgeCommandCollection>());
        }
    
    ~BridgeCommands() { 
        clearall_exit();
        while(!m_command_stack.empty()) m_command_stack.pop();
    }

    int start_bridge();
    void stop_bridge();
    void queue_next_command();
    void add_execute(const bridge_func_t& cb);
    void add_execute(const bridge_cont_func_t& cb);
    void add_repeat_start(std::chrono::microseconds delay, int count);
    void add_repeat_end();
    void add_delay(std::chrono::microseconds delay);
    void add_wait_exit();
    void trigger_exit() { emit_exit(); }

    EventLoop::SpEventLoop get_loop() { return m_state->m_event_loop; }
    std::shared_ptr<ReqLoop> get_req_loop() { return m_state->m_req_loop; }

  private:
    BridgeState * m_state;
    std::stack<std::shared_ptr<BridgeCommandCollection>> m_command_stack;
    int m_return_value;
};

#endif