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


#include "bridge-commands.hpp"

int BridgeCommands::start_bridge() {
    if (m_command_stack.size() != 1) throw BridgeUnmatchedLoopException();
    queue_next_command();
    return this->m_return_value;
}

void BridgeCommands::queue_next_command() {
    m_state->m_event_loop->getTimerEvent([this](){
        if (m_command_stack.size() == 0) return kEventLoopTimerDone;
        auto bridge_command_collection = m_command_stack.top();
        return bridge_command_collection->execute(this->shared_from_this());
    }, 0);
}

void BridgeCommands::stop_bridge() {
    m_state->m_event_loop->stop();
}

void BridgeCommands::add_execute(const bridge_func_t& cb) {
    m_command_stack.top()->add_command(std::make_shared<BridgeCommandExecute>(cb));
}

void BridgeCommands::add_repeat_start(std::chrono::microseconds delay, int count) {
    auto cloop = std::make_shared<BridgeCommandRepeat>(delay, count);
    // add the loop command to the current series of commands
    m_command_stack.top()->add_command(cloop);
    // add further commands to the loop
    m_command_stack.push(cloop);
}

void BridgeCommands::add_repeat_end() {
    if (m_command_stack.size() <= 1) throw BridgeUnmatchedLoopException();
    // finished with that loop
    m_command_stack.pop();
}

void BridgeCommands::add_delay(std::chrono::microseconds usecs) {
    m_command_stack.top()->add_command(std::make_shared<BridgeCommandDelay>(usecs));
}

void BridgeCommands::add_wait_exit() {
    m_command_stack.top()->add_command(std::make_shared<BridgeCommandWaitExit>(this->shared_from_this()));
}

// Command execution

int64_t BridgeCommands::BridgeCommandExecute::execute(SpBridgeCommands bc) {
    bc->m_return_value = m_cb(reinterpret_cast<void *>(bc.get()));
    return (bc->m_return_value?0:kEventLoopTimerDone);
}

int64_t BridgeCommands::BridgeCommandExecuteAsync::execute(SpBridgeCommands bc) {
    m_cb(reinterpret_cast<void *>(bc.get()), [bc](int result){
        bc->m_return_value = result;
        bc->queue_next_command();
    });
    return kEventLoopTimerDone;
}

int64_t BridgeCommands::BridgeCommandCollection::execute(SpBridgeCommands bc) {
    // Only pop a command once the next command is executed to keep the command
    // alive. This is important in the case of wait exit which will fire after
    // it has executed.
    if (m_started)
        m_command_queue.pop();
    else
        m_started = true;

    // If there are no commands left then pop the command collection
    if (m_command_queue.size() == 0) {
        bc->m_command_stack.pop();
        return 0;
    }
    // execute the next command
    return m_command_queue.front()->execute(bc);
}

int64_t BridgeCommands::BridgeCommandRepeat::execute(SpBridgeCommands bc) {
    // If there are no commands left then pop the command collection (loop)
    if ((m_repeat_times--) <= 0) {
        bc->m_command_stack.pop();
    } else {
        // push a collection which is a copy of the commands onto the command stack
        bc->m_command_stack.push(std::make_shared<BridgeCommandCollection>(this->m_command_queue));
    }
    return m_delay.count();
}

BridgeCommands::BridgeCommandWaitExit::BridgeCommandWaitExit(SpBridgeCommands bc) {
    // hook the exit function of this command sequence
    // this is valid in the lambda since the command is not popped until the
    // queue_next_command is executed
    bc->on_exit([this](){
        // Check if we were waiting and restart commands if we were
        // Todo - There is no check if 2 or more were waiting
        if (m_waiting && !m_bc.expired()) {
            m_waiting = false;
            m_bc.lock()->queue_next_command();
        }
    });
}

int64_t BridgeCommands::BridgeCommandWaitExit::execute(SpBridgeCommands bc) {
    // done until restarted by the hooked exit
    m_waiting = true;
    m_bc = bc;
    return kEventLoopTimerDone;
}

