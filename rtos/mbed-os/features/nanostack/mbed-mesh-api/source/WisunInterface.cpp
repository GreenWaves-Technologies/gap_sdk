/*
 * Copyright (c) 2018 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "WisunInterface.h"
#include "include/wisun_tasklet.h"
#include "callback_handler.h"
#include "NanostackLockGuard.h"
#include "mesh_system.h"
#include "randLIB.h"

#include "ns_trace.h"
#define TRACE_GROUP "WSIn"

class Nanostack::WisunInterface : public Nanostack::MeshInterface {
public:
    virtual nsapi_error_t bringup(bool dhcp, const char *ip,
                                  const char *netmask, const char *gw,
                                  nsapi_ip_stack_t stack = IPV6_STACK,
                                  bool blocking = true);
    virtual nsapi_error_t bringdown();
    virtual char *get_gateway(char *buf, nsapi_size_t buflen);

    friend class Nanostack;
    friend class ::WisunInterface;
private:
    WisunInterface(NanostackRfPhy &phy) : MeshInterface(phy) { }
    mesh_error_t init();
    mesh_error_t mesh_connect();
    mesh_error_t mesh_disconnect();
};

Nanostack::WisunInterface *WisunInterface::get_interface() const
{
    return static_cast<Nanostack::WisunInterface *>(_interface);
}

nsapi_error_t WisunInterface::do_initialize()
{
    if (!_interface) {
        _interface = new (std::nothrow) Nanostack::WisunInterface(*_phy);
        if (!_interface) {
            return NSAPI_ERROR_NO_MEMORY;
        }
        _interface->attach(_connection_status_cb);
    }
    return NSAPI_ERROR_OK;
}

nsapi_error_t Nanostack::WisunInterface::bringup(bool dhcp, const char *ip,
                                                 const char *netmask, const char *gw,
                                                 nsapi_ip_stack_t stack, bool blocking)
{
    nanostack_lock();

    if (register_phy() < 0) {
        nanostack_unlock();
        return NSAPI_ERROR_DEVICE_ERROR;
    }

    _blocking = blocking;

    // After the RF is up, we can seed the random from it.
    randLIB_seed_random();

    mesh_error_t status = init();
    if (status != MESH_ERROR_NONE) {
        nanostack_unlock();
        return map_mesh_error(status);
    }

    status = mesh_connect();
    if (status != MESH_ERROR_NONE) {
        nanostack_unlock();
        return map_mesh_error(status);
    }

    // Release mutex before blocking
    nanostack_unlock();

    if (blocking) {
        // wait connection for ever
        int32_t count = connect_semaphore.wait(osWaitForever);

        if (count <= 0) {
            return NSAPI_ERROR_DHCP_FAILURE; // sort of...
        }
    }
    return 0;

}

nsapi_error_t Nanostack::WisunInterface::bringdown()
{
    NanostackLockGuard lock;

    mesh_error_t status = mesh_disconnect();

    return map_mesh_error(status);
}

mesh_error_t Nanostack::WisunInterface::init()
{
    wisun_tasklet_init();
    __mesh_handler_set_callback(this);
    interface_id = wisun_tasklet_network_init(_device_id);

    if (interface_id == -2) {
        return MESH_ERROR_PARAM;
    } else if (interface_id == -3) {
        return MESH_ERROR_MEMORY;
    } else if (interface_id < 0) {
        return MESH_ERROR_UNKNOWN;
    }
    return MESH_ERROR_NONE;
}

mesh_error_t Nanostack::WisunInterface::mesh_connect()
{
    int8_t status = -9; // init to unknown error
    tr_debug("connect()");

    status = wisun_tasklet_connect(&__mesh_handler_c_callback, interface_id);

    if (status >= 0) {
        return MESH_ERROR_NONE;
    } else if (status == -1) {
        return MESH_ERROR_PARAM;
    } else if (status == -2) {
        return MESH_ERROR_MEMORY;
    } else if (status == -3) {
        return MESH_ERROR_STATE;
    } else {
        return MESH_ERROR_UNKNOWN;
    }
}

mesh_error_t Nanostack::WisunInterface::mesh_disconnect()
{
    int8_t status = -1;

    status = wisun_tasklet_disconnect(true);

    if (status >= 0) {
        return MESH_ERROR_NONE;
    }

    return MESH_ERROR_UNKNOWN;
}

char *Nanostack::WisunInterface::get_gateway(char *buf, nsapi_size_t buflen)
{
    NanostackLockGuard lock;
    if (wisun_tasklet_get_router_ip_address(buf, buflen) == 0) {
        return buf;
    }
    return NULL;
}

bool WisunInterface::getRouterIpAddress(char *address, int8_t len)
{
    return _interface->get_gateway(address, len);
}

#define WISUN 0x2345
#if MBED_CONF_NSAPI_DEFAULT_MESH_TYPE == WISUN && DEVICE_802_15_4_PHY
MBED_WEAK MeshInterface *MeshInterface::get_target_default_instance()
{
    static bool inited;
    static WisunInterface interface;
    singleton_lock();
    if (!inited) {
        nsapi_error_t result = interface.initialize(&NanostackRfPhy::get_default_instance());
        if (result != 0) {
            tr_error("Wi-SUN initialize failed: %d", result);
            singleton_unlock();
            return NULL;
        }
        inited = true;
    }
    singleton_unlock();
    return &interface;
}
#endif
