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

#ifndef __BRIDGE_STATE_H
#define __BRIDGE_STATE_H

#include "json.hpp"
#include "events/events.hpp"
#include "reqloop.hpp"
#include "reqserver.hpp"
#include "gdb-server/gdb-server.hpp"
#include "cables/cable.hpp"
#include "cables/adv_dbg_itf/adv_dbg_itf.hpp"

#include <string>

class BridgeCommands;

class BridgeState {
    public:
        BridgeState(const char * config_string) {
            m_event_loop = EventLoop::getLoop();
            m_bridge_commands = std::make_shared<BridgeCommands>(this);
            m_system_config = js::import_config_from_string(std::string(config_string));
        }
        EventLoop::SpEventLoop m_event_loop;
        std::shared_ptr<Adv_dbg_itf> m_adu = nullptr;
        std::shared_ptr<Gdb_server> m_gdb_server = nullptr;
        std::shared_ptr<ReqLoop> m_req_loop = nullptr;
        std::shared_ptr<BridgeCommands> m_bridge_commands;
        std::shared_ptr<ReqServer> m_req_server = nullptr;
        js::config * m_system_config;
};

#endif