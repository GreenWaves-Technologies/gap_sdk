/*
 * Copyright (c) 2018-2019, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
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

#include <string.h>
#include "nsconfig.h"
#include "ns_types.h"
#include "ns_trace.h"
#include "net_interface.h"
#include "eventOS_event.h"
#include "NWK_INTERFACE/Include/protocol.h"
#include "6LoWPAN/Bootstraps/protocol_6lowpan.h"
#include "6LoWPAN/Bootstraps/protocol_6lowpan_interface.h"
#include "6LoWPAN/ws/ws_config.h"
#include "6LoWPAN/ws/ws_common.h"
#include "6LoWPAN/ws/ws_bootstrap.h"
#include "RPL/rpl_control.h"
#include "RPL/rpl_data.h"
#include "Common_Protocols/icmpv6.h"
#include "Common_Protocols/icmpv6_radv.h"
#include "ws_management_api.h"
#include "net_rpl.h"
#include "Service_Libs/nd_proxy/nd_proxy.h"
#include "6LoWPAN/ws/ws_bbr_api_internal.h"
#include "DHCPv6_Server/DHCPv6_server_service.h"

#define TRACE_GROUP "wsbs"

#define RPL_INSTANCE_ID 1

#ifdef HAVE_WS_BORDER_ROUTER


/* when creating BBR make ULA dodag ID always and when network becomes available add prefix to DHCP
 *
 *
 */
static int8_t backbone_interface_id = -1; // BBR backbone information

static uint8_t static_dodag_prefix[8] = {0xfd, 0x00, 0x61, 0x72, 0x6d};
static uint8_t static_ula_address[16] = {0};
static uint8_t static_dodag_id[16] = {0};
static uint8_t global_dodag_id[16] = {0};
static uint32_t bbr_delay_timer = 20; // initial delay.

static rpl_dodag_conf_t rpl_conf = {
    // Lifetime values
    .default_lifetime = 120,
    .lifetime_unit = 60,
    .objective_code_point = 1, // MRHOF algorithm used
    .authentication = 0,
    .path_control_size = 7,
    .dag_max_rank_increase = 2048,
    .min_hop_rank_increase = 256,
    // DIO configuration
    .dio_interval_min = WS_RPL_DIO_IMIN,
    .dio_interval_doublings = WS_RPL_DIO_DOUBLING,
    .dio_redundancy_constant = WS_RPL_DIO_REDUNDANCY
};

void ws_bbr_rpl_config(uint8_t imin, uint8_t doubling, uint8_t redundancy)
{
    if (imin == 0 || doubling == 0) {
        // use default values
        imin = WS_RPL_DIO_IMIN;
        doubling = WS_RPL_DIO_DOUBLING;
        redundancy = WS_RPL_DIO_REDUNDANCY;
    }
    if (rpl_conf.dio_interval_min == imin &&
            rpl_conf.dio_interval_doublings == doubling &&
            rpl_conf.dio_redundancy_constant == redundancy) {
        // Same values no update needed
        return;
    }
    rpl_conf.dio_interval_min = imin;
    rpl_conf.dio_interval_doublings = doubling;
    rpl_conf.dio_redundancy_constant = redundancy;
    if (protocol_6lowpan_rpl_root_dodag) {
        rpl_control_update_dodag_config(protocol_6lowpan_rpl_root_dodag, &rpl_conf);
        rpl_control_increment_dodag_version(protocol_6lowpan_rpl_root_dodag);
    }
}

static void ws_bbr_rpl_root_start(uint8_t *dodag_id)
{
    tr_info("RPL root start");
    rpl_data_init_root();

    if (protocol_6lowpan_rpl_root_dodag) {
        rpl_control_delete_dodag_root(protocol_6lowpan_rpl_domain, protocol_6lowpan_rpl_root_dodag);
        protocol_6lowpan_rpl_root_dodag = NULL;
    }

    protocol_6lowpan_rpl_root_dodag = rpl_control_create_dodag_root(protocol_6lowpan_rpl_domain, RPL_INSTANCE_ID, dodag_id, &rpl_conf, rpl_conf.min_hop_rank_increase, RPL_GROUNDED | RPL_MODE_NON_STORING | RPL_DODAG_PREF(0));
    if (!protocol_6lowpan_rpl_root_dodag) {
        tr_err("RPL dodag init failed");
        return;
    }
    memcpy(static_dodag_id, dodag_id, 16);

    // RPL memory limits set larger for Border router
    rpl_control_set_memory_limits(64 * 1024, 0);

    uint8_t t_flags = PIO_A;

    rpl_control_update_dodag_prefix(protocol_6lowpan_rpl_root_dodag, dodag_id, 64, t_flags, 7200, 7200, false);
    rpl_control_update_dodag_route(protocol_6lowpan_rpl_root_dodag, dodag_id, 64, 0x18, 7200, false);
}

static void ws_bbr_rpl_root_stop(void)
{
    tr_info("RPL root stop");
    rpl_control_delete_dodag_root(protocol_6lowpan_rpl_domain, protocol_6lowpan_rpl_root_dodag);
    protocol_6lowpan_rpl_root_dodag = NULL;
    memset(static_ula_address, 0, 16);
    memset(static_dodag_id, 0, 16);
    memset(global_dodag_id, 0, 16);
}

static int ws_border_router_proxy_validate(int8_t interface_id, uint8_t *address)
{

    /* Could also check route type, but I don't think it really matters */
    ipv6_route_t *route;
    route = ipv6_route_choose_next_hop(address, interface_id, NULL);
    if (!route || route->prefix_len < 128) {
        return -1;
    }

    return 0;
}

int ws_border_router_proxy_state_update(int8_t caller_interface_id, int8_t handler_interface_id, bool status)
{
    (void)caller_interface_id;

    protocol_interface_info_entry_t *cur = protocol_stack_interface_info_get_by_id(handler_interface_id);
    if (!cur) {
        tr_error("No Interface");
        return -1;
    }

    if (status) {
        tr_debug("Border router Backhaul link ready");
    } else {
        tr_debug("Border router Backhaul link down");
    }
    return 0;
}

static int ws_bbr_static_ula_create(protocol_interface_info_entry_t *cur)
{
    if (memcmp(static_ula_address, ADDR_UNSPECIFIED, 16) != 0) {
        // address generated
        return 0;
    }
    tr_info("BBR generate ula prefix");

    // This address is only used if no other address available.
    if_address_entry_t *add_entry = icmpv6_slaac_address_add(cur, static_dodag_prefix, 64, 0xffffffff, 0, true, SLAAC_IID_FIXED);
    if (!add_entry) {
        return -1;
    }
    memcpy(static_ula_address, add_entry->address, 16);
    tr_info("BBR generate ula prefix addr %s", trace_ipv6(static_ula_address));

    return 0;
}

/*
 * 0 static non rooted self generated own address
 * 1 static address with backbone connectivity
 */
static int ws_bbr_static_dodag_get(protocol_interface_info_entry_t *cur, uint8_t *dodag_id_ptr)
{

    protocol_interface_info_entry_t *bb_interface = protocol_stack_interface_info_get_by_id(backbone_interface_id);

    if (bb_interface && bb_interface->ipv6_configure->ipv6_stack_mode == NET_IPV6_BOOTSTRAP_STATIC) {
        // static configuration for ethernet available
        ns_list_foreach(if_address_entry_t, add_entry, &cur->ip_addresses) {
            if (memcmp(add_entry->address, bb_interface->ipv6_configure->static_prefix64, 8) == 0) {
                //tr_info("BBR static config available");
                if (dodag_id_ptr) {
                    memcpy(dodag_id_ptr, add_entry->address, 16);
                }
                return 1;
            }
        }
    }
    ws_bbr_static_ula_create(cur);

    // only own generated prefix available
    if (dodag_id_ptr) {
        memcpy(dodag_id_ptr, static_ula_address, 16);
    }

    return 0;
}


static int ws_bbr_dodag_get(protocol_interface_info_entry_t *cur, uint8_t *static_dodag_id_ptr, uint8_t *dodag_id_ptr)
{
    uint8_t global_address[16];

    if (static_dodag_id_ptr) {
        memset(static_dodag_id_ptr, 0, 16);
    }

    if (dodag_id_ptr) {
        memset(dodag_id_ptr, 0, 16);
    }

    if (ws_bbr_static_dodag_get(cur, static_dodag_id_ptr) < 0) {
        // no static configuration available
        return -1;
    }

    if (arm_net_address_get(backbone_interface_id, ADDR_IPV6_GP, global_address) != 0) {
        // No global prefix available
        return 0;
    }
    if (memcmp(global_address, dodag_id_ptr, 8) == 0) {
        // static address is same
        return 0;
    }
    memcpy(dodag_id_ptr, global_address, 16);
    return 0;
}
static void wisun_bbr_na_send(int8_t interface_id, const uint8_t target[static 16])
{
    protocol_interface_info_entry_t *cur = protocol_stack_interface_info_get_by_id(interface_id);
    if (!cur) {
        return;
    }

    buffer_t *buffer = icmpv6_build_na(cur, false, true, true, target, NULL, ADDR_UNSPECIFIED);
    protocol_push(buffer);
    return;

}

static bool wisun_dhcp_address_add_cb(int8_t interfaceId, dhcp_address_cache_update_t *address_info, void *route_src)
{
    (void)route_src;
    protocol_interface_info_entry_t *curPtr = protocol_stack_interface_info_get_by_id(interfaceId);
    if (!curPtr) {
        return false;
    }

    // When address is allocated we send NA to backbone to notify the new address and flush from other BBRs
    wisun_bbr_na_send(backbone_interface_id, address_info->allocatedAddress);
    return true;
}


static void ws_bbr_rpl_status_check(protocol_interface_info_entry_t *cur)
{

    uint8_t static_id[16] = {0};
    uint8_t global_id[16] = {0};

    //tr_info("BBR status check");

    ws_bbr_dodag_get(cur, static_id, global_id);

    if (memcmp(static_dodag_id, static_id, 16) != 0) {
        // Static id updated or first setup
        ws_bbr_rpl_root_start(static_id);
    }
    if (memcmp(global_dodag_id, global_id, 16) != 0) {
        // Global prefix changed
        if (memcmp(global_dodag_id, ADDR_UNSPECIFIED, 16) != 0) {
            // TODO remove old global prefix
            tr_info("RPL GUA deactivate %s", trace_ipv6(global_dodag_id));

            rpl_control_update_dodag_prefix(protocol_6lowpan_rpl_root_dodag, static_dodag_id, 64, PIO_A, 7200, 7200, false);
            rpl_control_update_dodag_route(protocol_6lowpan_rpl_root_dodag, static_dodag_id, 64, 0x18, 7200, false);

            // Old backbone information is deleted after 120 seconds
            rpl_control_update_dodag_route(protocol_6lowpan_rpl_root_dodag, NULL, 0, 0, 120, true);
            rpl_control_update_dodag_prefix(protocol_6lowpan_rpl_root_dodag, global_dodag_id, 64, 0, 120, 0, true);
            rpl_control_update_dodag_route(protocol_6lowpan_rpl_root_dodag, global_dodag_id, 64, 0, 120, true);
            ipv6_route_add_with_info(global_dodag_id, 64, backbone_interface_id, NULL, ROUTE_THREAD_BBR, NULL, 0, 120, 0);
            DHCPv6_server_service_delete(cur->id, global_dodag_id, false);

            // Set old addresses to deferred and timeout
            ws_dhcp_client_address_delete(cur, global_dodag_id);
        }
        // TODO add global prefix
        if (memcmp(global_id, ADDR_UNSPECIFIED, 16) != 0) {
            //DHCPv6 Server set here
            //Interface LL64 address
            uint8_t ll[16];
            memcpy(ll, ADDR_LINK_LOCAL_PREFIX, 8);
            memcpy(&ll[8], cur->mac, 8);
            ll[8] ^= 2;

            if (DHCPv6_server_service_init(cur->id, global_id, cur->mac, DHCPV6_DUID_HARDWARE_IEEE_802_NETWORKS_TYPE) != 0) {
                tr_error("DHCPv6 Server create fail");
                return;
            }
            DHCPv6_server_service_callback_set(cur->id, global_id, NULL, wisun_dhcp_address_add_cb);

            DHCPv6_server_service_set_address_autonous_flag(cur->id, global_id, true);
            DHCPv6_server_service_set_address_validlifetime(cur->id, global_id, 7200);

            tr_info("RPL GUA activate %s", trace_ipv6(global_id));
            ws_dhcp_client_address_request(cur, global_id, ll);

            rpl_control_update_dodag_route(protocol_6lowpan_rpl_root_dodag, NULL, 0, 0, 7200, false);
            rpl_control_update_dodag_prefix(protocol_6lowpan_rpl_root_dodag, static_dodag_id, 64, PIO_A, 7200, 0, false);
            rpl_control_update_dodag_route(protocol_6lowpan_rpl_root_dodag, static_dodag_id, 64, 0x18, 7200, false);
            rpl_control_update_dodag_prefix(protocol_6lowpan_rpl_root_dodag, global_id, 64, 0, 7200, 7200, false);
            rpl_control_update_dodag_route(protocol_6lowpan_rpl_root_dodag, global_id, 64, 0, 7200, false);
            ipv6_route_add_with_info(global_id, 64, backbone_interface_id, NULL, ROUTE_THREAD_BBR, NULL, 0, 0xffffffff, 0);
        }
        memcpy(global_dodag_id, global_id, 16);
        rpl_control_increment_dodag_version(protocol_6lowpan_rpl_root_dodag);
        nd_proxy_downstream_interface_register(cur->id, ws_border_router_proxy_validate, ws_border_router_proxy_state_update);
    }

}

void ws_bbr_seconds_timer(protocol_interface_info_entry_t *cur, uint32_t seconds)
{
    (void)seconds;

    if (!ws_info(cur)) {
        return;
    }
    if (cur->bootsrap_mode != ARM_NWK_BOOTSRAP_MODE_6LoWPAN_BORDER_ROUTER) {
        // Not a border router
        return;
    }
    if (!cur->rpl_domain) {
        // RPL not started
        return;
    }

    if (bbr_delay_timer > seconds) {
        bbr_delay_timer -= seconds;
    } else {
        bbr_delay_timer = 20; // 20 second interval between status checks

        // prequisists
        // Wi-SUN network configuration started without RPL

        // RPL configured simple
        // 1. Wait for backend connection
        // 2. When address becomes available in backend start RPL dodag
        // 3. if address removed remove dodag

        // RPL configured Advanced
        // 1. Add ULA DODAG and and start ROOT even without backend
        //   a. If static prefix configured use it.
        //   b. generate random ULA and publish it to backend
        // 2. if GUA prefix becomes available in backend add new prefix to DODAG
        // 3. if GUA prefix is removed remove the prefix.

        if (protocol_6lowpan_rpl_root_dodag) {
            // Border router is active
            if (0 != protocol_interface_address_compare(static_dodag_id)) {
                // Dodag has become invalid need to delete
                tr_info("RPL static dodag not valid anymore %s", trace_ipv6(static_dodag_id));
                ws_bbr_rpl_root_stop();
            } else {

            }
        }
        ws_bbr_rpl_status_check(cur);

    }
    // Normal BBR operation
    if (protocol_6lowpan_rpl_root_dodag) {
        if (cur->ws_info->pan_version_timer > seconds) {
            cur->ws_info->pan_version_timer -= seconds;
        } else {
            // Border router has timed out
            tr_debug("Border router version number update");
            cur->ws_info->pan_version_timer = PAN_VERSION_LIFETIME;
            cur->ws_info->pan_information.pan_version++;
            // Inconsistent for border router to make information distribute faster
            ws_bootstrap_configuration_trickle_reset(cur);

            if (cur->ws_info->network_size_config == NETWORK_SIZE_AUTOMATIC) {
                ws_common_network_size_configure(cur, cur->ws_info->pan_information.pan_size);
            }
            // We update the RPL version in same time to allow nodes to reselect parent
            // As configuration is made so that devices cant move downward in dodag this allows it
            // TODO think the correct rate for this
            if (cur->ws_info->pan_information.pan_version && cur->ws_info->pan_information.pan_version % RPL_VERSION_LIFETIME / PAN_VERSION_LIFETIME == 0) {
                // Third the rate of configuration version change at default 5 hours
                rpl_control_increment_dodag_version(protocol_6lowpan_rpl_root_dodag);
            }
        }

    }

}

uint16_t test_pan_size_override = 0xffff;

uint16_t ws_bbr_pan_size(protocol_interface_info_entry_t *cur)
{
    uint16_t result = 0;
    if (test_pan_size_override != 0xffff) {
        return test_pan_size_override;
    }

    rpl_control_get_instance_dao_target_count(cur->rpl_domain, RPL_INSTANCE_ID, NULL, &result);
    return result;
}


#endif //HAVE_WS_BORDER_ROUTER

/* Public APIs
 *
 */

int ws_bbr_start(int8_t interface_id, int8_t bb_interface_id)
{
#ifdef HAVE_WS_BORDER_ROUTER

    (void)interface_id;
    protocol_interface_info_entry_t *bb_interface = protocol_stack_interface_info_get_by_id(bb_interface_id);

    if (!bb_interface) {
        return -1;
    }
    // TODO make bb configurations

    backbone_interface_id = bb_interface_id;

    return 0;
#else
    (void)interface_id;
    (void)bb_interface_id;
    return -1;
#endif
}
void ws_bbr_stop(int8_t interface_id)
{
#ifdef HAVE_WS_BORDER_ROUTER

    (void)interface_id;
    backbone_interface_id = -1;

    if (!protocol_6lowpan_rpl_domain) {
        return;
    }

    rpl_control_delete_dodag_root(protocol_6lowpan_rpl_domain, protocol_6lowpan_rpl_root_dodag);
    protocol_6lowpan_rpl_root_dodag = NULL;

#else
    (void)interface_id;
#endif
}

int ws_bbr_node_keys_remove(int8_t interface_id, uint8_t *eui64)
{
    (void) interface_id;
    (void) eui64;

    return -1;
}

int ws_bbr_node_access_revoke_start(int8_t interface_id)
{
    (void) interface_id;

    return -1;
}
