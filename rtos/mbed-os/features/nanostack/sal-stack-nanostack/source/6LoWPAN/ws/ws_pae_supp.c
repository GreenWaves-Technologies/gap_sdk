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

#include "nsconfig.h"
#include <string.h>
#include "ns_types.h"
#include "ns_list.h"
#include "ns_trace.h"
#include "nsdynmemLIB.h"
#include "fhss_config.h"
#include "eventOS_event.h"
#include "eventOS_scheduler.h"
#include "eventOS_event_timer.h"
#include "ns_address.h"
#include "NWK_INTERFACE/Include/protocol.h"
#include "RPL/rpl_protocol.h"
#include "RPL/rpl_control.h"
#include "RPL/rpl_data.h"
#include "6LoWPAN/ws/ws_config.h"
#include "Security/kmp/kmp_addr.h"
#include "Security/kmp/kmp_api.h"
#include "Security/protocols/sec_prot_certs.h"
#include "Security/protocols/sec_prot_keys.h"
#include "Security/protocols/key_sec_prot/key_sec_prot.h"
#include "Security/protocols/eap_tls_sec_prot/supp_eap_tls_sec_prot.h"
#include "Security/protocols/tls_sec_prot/tls_sec_prot.h"
#include "Security/protocols/fwh_sec_prot/supp_fwh_sec_prot.h"
#include "Security/protocols/gkh_sec_prot/supp_gkh_sec_prot.h"
#include "6LoWPAN/ws/ws_pae_controller.h"
#include "6LoWPAN/ws/ws_pae_supp.h"
#include "6LoWPAN/ws/ws_pae_lib.h"
#include "6LoWPAN/ws/ws_pae_nvm_store.h"
#include "6LoWPAN/ws/ws_pae_nvm_data.h"
#include "6LoWPAN/MAC/mpx_api.h"
#include "6LoWPAN/ws/ws_eapol_pdu.h"
#include "Security/kmp/kmp_eapol_pdu_if.h"

#ifdef HAVE_WS
#ifdef HAVE_PAE_SUPP

#define TRACE_GROUP "wsps"

#define PAE_TASKLET_INIT                       1
#define PAE_TASKLET_EVENT                      2
#define PAE_TASKLET_TIMER                      3

// Wait for for authenticator to continue with authentication (e.g. after EAP-TLS to initiate 4WH)
#define WAIT_FOR_AUTHENTICATION_TICKS          30 * 10  // 30 seconds

// How many times in maximum stored keys are used for authentication
#define STORED_KEYS_MAXIMUM_USE_COUNT           2

const char *NW_INFO_FILE = "pae_nw_info";
const char *KEYS_FILE = "pae_keys";

typedef struct {
    char network_name[33];                                 /**< Network name for keys */
    sec_prot_gtk_keys_t *gtks;                             /**< Link to GTKs */
    uint16_t pan_id;                                       /**< PAN ID for keys */
    bool updated : 1;                                      /**< Network info has been updated */
} sec_prot_keys_nw_info_t;

typedef struct {
    ns_list_link_t link;                                   /**< Link */
    kmp_service_t *kmp_service;                            /**< KMP service */
    protocol_interface_info_entry_t *interface_ptr;        /**< Interface */
    ws_pae_supp_auth_completed *auth_completed;            /**< Authentication completed callback, continue bootstrap */
    ws_pae_supp_key_insert *key_insert;                    /**< Key insert callback */
    supp_entry_t entry;                                    /**< Supplicant data */
    kmp_addr_t target_addr;                                /**< EAPOL target (parent) address */
    trickle_t auth_trickle_timer;                          /**< Trickle timer for re-sending initial EAPOL-key */
    sec_prot_gtk_keys_t gtks;                              /**< GTKs */
    uint8_t new_br_eui_64[8];                              /**< Border router EUI-64 indicated by bootstrap */
    uint8_t ptk_eui_64[8];                                 /**< Border router EUI-64 used on PTK generation */
    sec_prot_keys_nw_info_t sec_keys_nw_info;              /**< Security keys network information */
    uint8_t nw_keys_used_cnt;                              /**< How many times bootstrap has been tried with current keys */
    bool auth_trickle_running : 1;                         /**< Trickle timer running */
    bool auth_requested : 1;                               /**< Authentication has been requested */
    bool timer_running : 1;                                /**< Timer is running */
    bool new_br_eui_64_set : 1;                            /**< Border router address has been set */
} pae_supp_t;

// Wi-SUN specification states initial retransmission to be around 5 minutes and maximum 60 minutes
static const trickle_params_t auth_trickle_params = {
    .Imin = 30 * 10,          /* 30s; ticks are 100ms */
    .Imax = 60 * 10,          /* 60s */
    .k = 0,                   /* infinity - no consistency checking */
    .TimerExpirations = 3
};

static void ws_pae_supp_free(pae_supp_t *pae_supp);
static void ws_pae_supp_authenticate_response(pae_supp_t *pae_supp, bool success);
static int8_t ws_pae_supp_initial_key_send(pae_supp_t *pae_supp);
static void ws_pae_supp_nvm_update(pae_supp_t *pae_supp);
static int8_t ws_pae_supp_nw_keys_valid_check(pae_supp_t *pae_supp, uint16_t pan_id);
static int8_t ws_pae_supp_nvm_nw_info_write(pae_supp_t *pae_supp);
static int8_t ws_pae_supp_nvm_keys_write(pae_supp_t *pae_supp);
static pae_supp_t *ws_pae_supp_get(protocol_interface_info_entry_t *interface_ptr);
static int8_t ws_pae_supp_event_send(kmp_service_t *service, void *data);
static void ws_pae_supp_tasklet_handler(arm_event_s *event);
static int8_t ws_pae_supp_timer_if_start(kmp_service_t *service, kmp_api_t *kmp);
static int8_t ws_pae_supp_timer_if_stop(kmp_service_t *service, kmp_api_t *kmp);
static int8_t ws_pae_supp_timer_start(pae_supp_t *pae_supp);
static int8_t ws_pae_supp_timer_stop(pae_supp_t *pae_supp);
static bool ws_pae_supp_timer_running(pae_supp_t *pae_supp);
static void ws_pae_supp_kmp_service_addr_get(kmp_service_t *service, kmp_api_t *kmp, kmp_addr_t *local_addr, kmp_addr_t *remote_addr);
static kmp_api_t *ws_pae_supp_kmp_service_api_get(kmp_service_t *service, kmp_api_t *kmp, kmp_type_e type);
static kmp_api_t *ws_pae_supp_kmp_incoming_ind(kmp_service_t *service, kmp_type_e type, const kmp_addr_t *addr);
static kmp_api_t *ws_pae_supp_kmp_create_and_start(kmp_service_t *service, kmp_type_e type, pae_supp_t *pae_supp);
static int8_t ws_pae_supp_eapol_pdu_address_check(protocol_interface_info_entry_t *interface_ptr, const uint8_t *eui_64);

static void ws_pae_supp_kmp_api_create_confirm(kmp_api_t *kmp, kmp_result_e result);
static void ws_pae_supp_kmp_api_create_indication(kmp_api_t *kmp, kmp_type_e type, kmp_addr_t *addr);
static void ws_pae_supp_kmp_api_finished_indication(kmp_api_t *kmp, kmp_result_e result, kmp_sec_keys_t *sec_keys);
static void ws_pae_supp_kmp_api_finished(kmp_api_t *kmp);

static const eapol_pdu_recv_cb_data_t eapol_pdu_recv_cb_data = {
    .priority = EAPOL_PDU_RECV_HIGH_PRIORITY,
    .addr_check = ws_pae_supp_eapol_pdu_address_check,
    .receive = kmp_eapol_pdu_if_receive
};

static int8_t tasklet_id = -1;
static NS_LIST_DEFINE(pae_supp_list, pae_supp_t, link);

int8_t ws_pae_supp_authenticate(protocol_interface_info_entry_t *interface_ptr, uint16_t dest_pan_id, uint8_t *dest_eui_64)
{
    pae_supp_t *pae_supp = ws_pae_supp_get(interface_ptr);
    if (!pae_supp) {
        return -1;
    }

    if (ws_pae_supp_nw_keys_valid_check(pae_supp, dest_pan_id) >= 0) {
        pae_supp->auth_completed(interface_ptr, true);
        return 0;
    }

    // Delete GTKs
    sec_prot_keys_gtks_init(pae_supp->sec_keys_nw_info.gtks);

    // Prepare to receive new border router address
    pae_supp->new_br_eui_64_set = false;

    // Stores target/parent address
    kmp_address_init(KMP_ADDR_EUI_64, &pae_supp->target_addr, dest_eui_64);
    // Sets target address in use
    pae_supp->entry.addr = (kmp_addr_t *) &pae_supp->target_addr;

    // Sends initial EAPOL-Key message
    if (ws_pae_supp_initial_key_send(pae_supp) < 0) {
        pae_supp->auth_completed(interface_ptr, false);
    }

    // Starts trickle
    trickle_start(&pae_supp->auth_trickle_timer, &auth_trickle_params);
    pae_supp->auth_trickle_running = true;

    // Starts supplicant timer
    ws_pae_supp_timer_start(pae_supp);

    pae_supp->auth_requested = true;

    tr_debug("PAE active");

    return 1;
}

int8_t ws_pae_supp_nw_info_set(protocol_interface_info_entry_t *interface_ptr, uint16_t pan_id, char *network_name)
{
    pae_supp_t *pae_supp = ws_pae_supp_get(interface_ptr);
    if (!pae_supp) {
        return -1;
    }

    // PAN ID has been modified
    if (pan_id != 0xffff && pan_id != pae_supp->sec_keys_nw_info.pan_id) {
        pae_supp->sec_keys_nw_info.pan_id = pan_id;
        pae_supp->sec_keys_nw_info.updated = true;
    }

    // Network name has been modified
    if (network_name && strncmp(pae_supp->sec_keys_nw_info.network_name, network_name, 33) != 0) {
        strncpy(pae_supp->sec_keys_nw_info.network_name, network_name, 32);
        pae_supp->sec_keys_nw_info.updated = true;
    }


    return 0;
}

int8_t ws_pae_supp_border_router_addr_write(protocol_interface_info_entry_t *interface_ptr, const uint8_t *eui_64)
{
    pae_supp_t *pae_supp = ws_pae_supp_get(interface_ptr);
    if (!pae_supp) {
        return -1;
    }

    memcpy(pae_supp->new_br_eui_64, eui_64, 8);
    pae_supp->new_br_eui_64_set = true;

    return 0;
}

int8_t ws_pae_supp_border_router_addr_read(protocol_interface_info_entry_t *interface_ptr, uint8_t *eui_64)
{
    pae_supp_t *pae_supp = ws_pae_supp_get(interface_ptr);
    if (!pae_supp) {
        return -1;
    }

    if (!pae_supp->entry.sec_keys.ptk_eui_64 || !pae_supp->entry.sec_keys.ptk_eui_64_set) {
        return -1;
    }

    memcpy(eui_64, pae_supp->entry.sec_keys.ptk_eui_64, 8);

    return 0;
}

int8_t ws_pae_supp_nw_key_valid(protocol_interface_info_entry_t *interface_ptr)
{
    pae_supp_t *pae_supp = ws_pae_supp_get(interface_ptr);
    if (!pae_supp) {
        return -1;
    }

    // Stored keys are valid
    pae_supp->nw_keys_used_cnt = 0;

    // Update NVM if data has been changed
    ws_pae_supp_nvm_update(pae_supp);

    return 0;
}

static void ws_pae_supp_nvm_update(pae_supp_t *pae_supp)
{
    // Check if NW info or GTKs have been changed
    if (pae_supp->sec_keys_nw_info.updated || sec_prot_keys_gtks_are_updated(pae_supp->sec_keys_nw_info.gtks)) {
        ws_pae_supp_nvm_nw_info_write(pae_supp);
        pae_supp->sec_keys_nw_info.updated = false;
        sec_prot_keys_gtks_updated_reset(pae_supp->sec_keys_nw_info.gtks);
    }

    // Check if pairwise security keys have been changed
    if (sec_prot_keys_are_updated(&pae_supp->entry.sec_keys)) {
        ws_pae_supp_nvm_keys_write(pae_supp);
        sec_prot_keys_updated_reset(&pae_supp->entry.sec_keys);
    }
}

static int8_t ws_pae_supp_nvm_nw_info_write(pae_supp_t *pae_supp)
{
    nvm_tlv_list_t tlv_list;
    ns_list_init(&tlv_list);

    nvm_tlv_entry_t *tlv_entry = ws_pae_nvm_store_nw_info_tlv_create(pae_supp->sec_keys_nw_info.pan_id,
                                                                     pae_supp->sec_keys_nw_info.network_name,
                                                                     &pae_supp->gtks);
    ns_list_add_to_end(&tlv_list, tlv_entry);

    ws_pae_nvm_store_tlv_file_write(NW_INFO_FILE, &tlv_list);
    ns_list_remove(&tlv_list, tlv_entry);
    ns_dyn_mem_free(tlv_entry);

    return 0;
}

static int8_t ws_pae_supp_nvm_nw_info_read(pae_supp_t *pae_supp)
{
    nvm_tlv_list_t tlv_list;
    ns_list_init(&tlv_list);

    ws_pae_nvm_store_tlv_file_read(NW_INFO_FILE, &tlv_list);

    ns_list_foreach_safe(nvm_tlv_entry_t, entry, &tlv_list) {
        ws_pae_nvm_store_nw_info_tlv_read(entry, &pae_supp->sec_keys_nw_info.pan_id,
                                          pae_supp->sec_keys_nw_info.network_name,
                                          &pae_supp->gtks);
        ns_list_remove(&tlv_list, entry);
        ns_dyn_mem_free(entry);
    }

    return 0;
}

static int8_t ws_pae_supp_nvm_keys_write(pae_supp_t *pae_supp)
{
    nvm_tlv_list_t tlv_list;
    ns_list_init(&tlv_list);

    nvm_tlv_entry_t *tlv_entry = ws_pae_nvm_store_keys_tlv_create(&pae_supp->entry.sec_keys);
    ns_list_add_to_end(&tlv_list, tlv_entry);

    ws_pae_nvm_store_tlv_file_write(KEYS_FILE, &tlv_list);
    ns_list_remove(&tlv_list, tlv_entry);
    ns_dyn_mem_free(tlv_entry);

    return 0;
}

static int8_t ws_pae_supp_nvm_keys_read(pae_supp_t *pae_supp)
{
    nvm_tlv_list_t tlv_list;
    ns_list_init(&tlv_list);

    ws_pae_nvm_store_tlv_file_read(KEYS_FILE, &tlv_list);

    ns_list_foreach_safe(nvm_tlv_entry_t, entry, &tlv_list) {
        ws_pae_nvm_store_keys_tlv_read(entry, &pae_supp->entry.sec_keys);
        ns_list_remove(&tlv_list, entry);
        ns_dyn_mem_free(entry);
    }

    return 0;
}

static void ws_pae_supp_authenticate_response(pae_supp_t *pae_supp, bool success)
{
    pae_supp->auth_trickle_running = false;
    if (pae_supp->auth_requested && pae_supp->auth_completed) {
        pae_supp->auth_requested = false;
        pae_supp->auth_completed(pae_supp->interface_ptr, success);
    }
}

static int8_t ws_pae_supp_initial_key_send(pae_supp_t *pae_supp)
{
    kmp_api_t *kmp = ws_pae_supp_kmp_create_and_start(pae_supp->kmp_service, IEEE_802_1X_MKA_KEY, pae_supp);
    if (!kmp) {
        return -1;
    }

    kmp_api_create_request(kmp, IEEE_802_1X_MKA_KEY, pae_supp->entry.addr, &pae_supp->entry.sec_keys);

    return 0;
}

static int8_t ws_pae_supp_nw_keys_valid_check(pae_supp_t *pae_supp, uint16_t pan_id)
{
    // Checks how many times authentication has been tried with current network keys
    if (pae_supp->nw_keys_used_cnt >= STORED_KEYS_MAXIMUM_USE_COUNT) {
        tr_debug("Keys not valid, delete GTKs");

        // Delete GTKs
        sec_prot_keys_gtks_init(pae_supp->sec_keys_nw_info.gtks);
        sec_prot_keys_gtks_updated_set(pae_supp->sec_keys_nw_info.gtks);
        ws_pae_supp_nvm_update(pae_supp);

        pae_supp->nw_keys_used_cnt = 0;
        return -1;
    }

    // First attempt to authenticate, checks if keys exists
    if (pae_supp->nw_keys_used_cnt == 0 && pan_id == pae_supp->sec_keys_nw_info.pan_id) {
        sec_prot_gtk_keys_t *gtks = pae_supp->sec_keys_nw_info.gtks;

        bool key_inserted = false;

        for (uint8_t i = 0; i < GTK_NUM; i++) {
            uint8_t *gtk = sec_prot_keys_gtk_get(gtks, i);
            if (gtk) {
                // Insert also non-live keys since GTK hash information not yet received
                pae_supp->key_insert(pae_supp->interface_ptr, i, gtk);
                key_inserted = true;
            }
        }

        if (key_inserted) {
            tr_debug("Keys inserted");
            pae_supp->nw_keys_used_cnt++;
            return 0;
        }
    }

    if (pae_supp->nw_keys_used_cnt == 0) {
        return -1;
    } else {
        tr_debug("Existing keys used, counter %i", pae_supp->nw_keys_used_cnt);
        pae_supp->nw_keys_used_cnt++;
        return 0;
    }
}

static void ws_pae_supp_keys_nw_info_init(sec_prot_keys_nw_info_t *sec_keys_nw_info, sec_prot_gtk_keys_t *gtks)
{
    if (!sec_keys_nw_info) {
        return;
    }

    memset(sec_keys_nw_info, 0, sizeof(sec_prot_keys_nw_info_t));

    sec_keys_nw_info->gtks = gtks;
    sec_keys_nw_info->updated = false;
}

void ws_pae_supp_cb_register(protocol_interface_info_entry_t *interface_ptr, ws_pae_supp_auth_completed *completed, ws_pae_supp_key_insert *key_insert)
{
    pae_supp_t *pae_supp = ws_pae_supp_get(interface_ptr);
    if (!pae_supp) {
        return;
    }

    pae_supp->auth_completed = completed;
    pae_supp->key_insert = key_insert;
}

int8_t ws_pae_supp_init(protocol_interface_info_entry_t *interface_ptr, const sec_prot_certs_t *certs)
{
    if (!interface_ptr) {
        return -1;
    }

    if (ws_pae_supp_get(interface_ptr) != NULL) {
        return 0;
    }

    pae_supp_t *pae_supp = ns_dyn_mem_alloc(sizeof(pae_supp_t));
    if (!pae_supp) {
        return -1;
    }

    pae_supp->interface_ptr = interface_ptr;
    pae_supp->auth_completed = 0;
    pae_supp->key_insert = 0;
    pae_supp->auth_trickle_running = false;
    pae_supp->nw_keys_used_cnt = 0;

    ws_pae_lib_supp_init(&pae_supp->entry);

    ws_pae_supp_keys_nw_info_init(&pae_supp->sec_keys_nw_info, &pae_supp->gtks);

    kmp_address_init(KMP_ADDR_EUI_64, &pae_supp->target_addr, 0);

    sec_prot_keys_gtks_init(&pae_supp->gtks);
    sec_prot_keys_init(&pae_supp->entry.sec_keys, &pae_supp->gtks, certs);
    memset(pae_supp->new_br_eui_64, 0, 8);
    memset(pae_supp->ptk_eui_64, 0, 8);
    sec_prot_keys_ptk_eui_64_set(&pae_supp->entry.sec_keys, pae_supp->ptk_eui_64);

    pae_supp->kmp_service = kmp_service_create();
    if (!pae_supp->kmp_service) {
        goto error;
    }

    if (kmp_service_cb_register(pae_supp->kmp_service, ws_pae_supp_kmp_incoming_ind, ws_pae_supp_kmp_service_addr_get, ws_pae_supp_kmp_service_api_get) < 0) {
        goto error;
    }

    if (kmp_service_event_if_register(pae_supp->kmp_service, ws_pae_supp_event_send)) {
        goto error;
    }

    if (kmp_service_timer_if_register(pae_supp->kmp_service, ws_pae_supp_timer_if_start, ws_pae_supp_timer_if_stop)) {
        goto error;
    }

    if (kmp_eapol_pdu_if_register(pae_supp->kmp_service, interface_ptr) < 0) {
        goto error;
    }

    if (ws_eapol_pdu_cb_register(interface_ptr, &eapol_pdu_recv_cb_data) < 0) {
        goto error;
    }

    if (key_sec_prot_register(pae_supp->kmp_service) < 0) {
        goto error;
    }

    if (supp_eap_tls_sec_prot_register(pae_supp->kmp_service) < 0) {
        goto error;
    }

    if (client_tls_sec_prot_register(pae_supp->kmp_service) < 0) {
        goto error;
    }

    if (supp_fwh_sec_prot_register(pae_supp->kmp_service) < 0) {
        goto error;
    }

    if (supp_gkh_sec_prot_register(pae_supp->kmp_service) < 0) {
        goto error;
    }

    if (tasklet_id < 0) {
        tasklet_id = eventOS_event_handler_create(ws_pae_supp_tasklet_handler, PAE_TASKLET_INIT);
        if (tasklet_id < 0) {
            goto error;
        }
    }

    if (ws_pae_supp_timer_stop(pae_supp) < 0) {
        goto error;
    }

    ws_pae_supp_nvm_nw_info_read(pae_supp);
    ws_pae_supp_nvm_keys_read(pae_supp);

    ns_list_add_to_end(&pae_supp_list, pae_supp);

    return 0;

error:
    ws_pae_supp_free(pae_supp);

    return -1;
}

int8_t ws_pae_supp_delete(protocol_interface_info_entry_t *interface_ptr)
{
    if (!interface_ptr) {
        return -1;
    }

    pae_supp_t *pae_supp = ws_pae_supp_get(interface_ptr);
    if (!pae_supp) {
        return -1;
    }

    ws_pae_supp_free(pae_supp);
    return 0;
}

static void ws_pae_supp_free(pae_supp_t *pae_supp)
{
    if (!pae_supp) {
        return;
    }

    ws_pae_lib_supp_delete(&pae_supp->entry);

    kmp_eapol_pdu_if_unregister(pae_supp->kmp_service);

    ws_eapol_pdu_cb_unregister(pae_supp->interface_ptr, &eapol_pdu_recv_cb_data);

    kmp_service_delete(pae_supp->kmp_service);

    ns_list_remove(&pae_supp_list, pae_supp);
    ns_dyn_mem_free(pae_supp);
}

static pae_supp_t *ws_pae_supp_get(protocol_interface_info_entry_t *interface_ptr)
{
    ns_list_foreach(pae_supp_t, entry, &pae_supp_list) {
        if (entry->interface_ptr == interface_ptr) {
            return entry;
        }
    }

    return NULL;
}

static pae_supp_t *ws_pae_supp_by_kmp_service_get(kmp_service_t *service)
{
    ns_list_foreach(pae_supp_t, entry, &pae_supp_list) {
        if (entry->kmp_service == service) {
            return entry;
        }
    }

    return NULL;
}

static int8_t ws_pae_supp_event_send(kmp_service_t *service, void *data)
{
    pae_supp_t *pae_supp = ws_pae_supp_by_kmp_service_get(service);
    if (!pae_supp) {
        return -1;
    }

    arm_event_s event = {
        .receiver = tasklet_id,
        .sender = 0,
        .event_id = pae_supp->interface_ptr->id,
        .data_ptr = data,
        .event_type = PAE_TASKLET_EVENT,
        .priority = ARM_LIB_LOW_PRIORITY_EVENT,
    };

    if (eventOS_event_send(&event) != 0) {
        return -1;
    }

    return 0;
}

static void ws_pae_supp_tasklet_handler(arm_event_s *event)
{
    if (event->event_type == PAE_TASKLET_INIT) {

    } else if (event->event_type == PAE_TASKLET_EVENT) {
        pae_supp_t *pae_supp = NULL;

        ns_list_foreach(pae_supp_t, entry, &pae_supp_list) {
            if (entry->interface_ptr->id == event->event_id) {
                pae_supp = entry;
                break;
            }
        }

        if (pae_supp) {
            kmp_service_event_if_event(pae_supp->kmp_service, event->data_ptr);
        }
    }
}

void ws_pae_supp_timer(uint16_t ticks)
{
    ns_list_foreach(pae_supp_t, pae_supp, &pae_supp_list) {
        if (!ws_pae_supp_timer_running(pae_supp)) {
            continue;
        }

        // Checks whether initial EAPOL-Key message needs to be re-send
        if (pae_supp->auth_trickle_running) {
            if (trickle_timer(&pae_supp->auth_trickle_timer, &auth_trickle_params, ticks)) {
                ws_pae_supp_initial_key_send(pae_supp);
            }
            // Maximum number of trickle expires, authentication fails
            if (!trickle_running(&pae_supp->auth_trickle_timer, &auth_trickle_params)) {
                ws_pae_supp_authenticate_response(pae_supp, false);
            }
        }

        // Updates KMP timers and supplicant authentication ongoing timer
        bool running = ws_pae_lib_supp_timer_update(&pae_supp->entry, ticks, kmp_service_timer_if_timeout);

        // Checks whether timer needs to be active
        if (!pae_supp->auth_trickle_running && !running) {

            tr_debug("PAE idle");
            // Sets target/parent address to null
            pae_supp->entry.addr = NULL;

            // If not already completed, restart bootstrap
            ws_pae_supp_authenticate_response(pae_supp, false);

            ws_pae_supp_timer_stop(pae_supp);
        }
    }
}

static int8_t ws_pae_supp_timer_if_start(kmp_service_t *service, kmp_api_t *kmp)
{
    pae_supp_t *pae_supp = ws_pae_supp_by_kmp_service_get(service);
    if (!pae_supp) {
        return -1;
    }

    if (ws_pae_supp_timer_start(pae_supp) < 0) {
        return -1;
    }

    kmp_entry_t *entry = kmp_api_data_get(kmp);
    if (!entry) {
        return -1;
    }
    ws_pae_lib_kmp_timer_start(&pae_supp->entry.kmp_list, entry);
    return 0;
}

static int8_t ws_pae_supp_timer_if_stop(kmp_service_t *service, kmp_api_t *kmp)
{
    pae_supp_t *pae_supp = ws_pae_supp_by_kmp_service_get(service);
    if (!pae_supp) {
        return -1;
    }

    kmp_entry_t *entry = kmp_api_data_get(kmp);
    if (!entry) {
        return -1;
    }
    ws_pae_lib_kmp_timer_stop(&pae_supp->entry.kmp_list, entry);
    return 0;
}

static int8_t ws_pae_supp_timer_start(pae_supp_t *pae_supp)
{
    pae_supp->timer_running = true;
    return 0;
}

static int8_t ws_pae_supp_timer_stop(pae_supp_t *pae_supp)
{
    pae_supp->timer_running = false;
    return 0;
}

static bool ws_pae_supp_timer_running(pae_supp_t *pae_supp)
{
    return pae_supp->timer_running;
}

static int8_t ws_pae_supp_eapol_pdu_address_check(protocol_interface_info_entry_t *interface_ptr, const uint8_t *eui_64)
{
    pae_supp_t *pae_supp = ws_pae_supp_get(interface_ptr);
    if (!pae_supp) {
        return -1;
    }

    // Message from EAPOL target node, route to self
    if (pae_supp->entry.addr) {
        if (memcmp(eui_64, kmp_address_eui_64_get(pae_supp->entry.addr), 8) == 0) {
            return 0;
        }
    }

    rpl_dodag_info_t dodag_info;
    struct rpl_instance *instance = rpl_control_enumerate_instances(interface_ptr->rpl_domain, NULL);
    if (instance && rpl_control_read_dodag_info(instance, &dodag_info)) {
        // Get parent
        const uint8_t *parent_ll_addr = rpl_control_preferred_parent_addr(instance, false);

        // Message from RPL parent, route to self
        if (parent_ll_addr && memcmp(&parent_ll_addr[8], eui_64, 8) == 0) {
            return 0;
        }

        return -1;
    }

    return 0;
}

static void ws_pae_supp_kmp_service_addr_get(kmp_service_t *service, kmp_api_t *kmp, kmp_addr_t *local_addr, kmp_addr_t *remote_addr)
{
    (void) kmp;

    pae_supp_t *pae_supp = ws_pae_supp_by_kmp_service_get(service);
    if (!pae_supp) {
        return;
    }

    // Get own EUI-64
    link_layer_address_s mac_params;
    if (arm_nwk_mac_address_read(pae_supp->interface_ptr->id, &mac_params) >= 0) {
        kmp_address_eui_64_set(local_addr, mac_params.mac_long);
    }

    if (pae_supp->new_br_eui_64_set) {
        kmp_address_eui_64_set(remote_addr, pae_supp->new_br_eui_64);
    } else if (pae_supp->entry.sec_keys.ptk_eui_64_set) {
        kmp_address_eui_64_set(remote_addr, pae_supp->entry.sec_keys.ptk_eui_64);
    } else {
        memset(remote_addr, 0, 8);
        tr_error("No border router EUI-64");
    }
}

static kmp_api_t *ws_pae_supp_kmp_service_api_get(kmp_service_t *service, kmp_api_t *kmp, kmp_type_e type)
{
    (void) kmp;

    pae_supp_t *pae_supp = ws_pae_supp_by_kmp_service_get(service);
    if (!pae_supp) {
        return NULL;
    }

    return ws_pae_lib_kmp_list_type_get(&pae_supp->entry.kmp_list, type);
}

static kmp_api_t *ws_pae_supp_kmp_incoming_ind(kmp_service_t *service, kmp_type_e type, const kmp_addr_t *addr)
{
    // Should be MKA, 4WH or GKH and never initial EAPOL-key for supplicant
    if (type > IEEE_802_1X_INITIAL_KEY) {
        return NULL;
    }

    pae_supp_t *pae_supp = ws_pae_supp_by_kmp_service_get(service);
    if (!pae_supp) {
        return NULL;
    }

    if (!pae_supp->entry.addr) {
        // Does no longer wait for authentication, ignores message
        return NULL;
    }

    // No longer runs trickle timer for re-sending initial EAPOL-key
    pae_supp->auth_trickle_running = false;

    // Updates parent address
    kmp_address_copy(pae_supp->entry.addr, addr);

    // Check if ongoing
    kmp_api_t *kmp = ws_pae_lib_kmp_list_type_get(&pae_supp->entry.kmp_list, type);
    if (kmp) {
        return kmp;
    }

    // Create new instance
    kmp = ws_pae_supp_kmp_create_and_start(service, type, pae_supp);

    // For EAP-TLS create also TLS in addition to EAP-TLS
    if (type == IEEE_802_1X_MKA) {
        if (ws_pae_lib_kmp_list_type_get(&pae_supp->entry.kmp_list, TLS_PROT) != NULL) {
            // TLS already exists, wait for it to be deleted
            ws_pae_lib_kmp_list_delete(&pae_supp->entry.kmp_list, kmp);
            return NULL;
        }
        // Create TLS instance */
        if (ws_pae_supp_kmp_create_and_start(service, TLS_PROT, pae_supp) == NULL) {
            ws_pae_lib_kmp_list_delete(&pae_supp->entry.kmp_list, kmp);
            return NULL;
        }
    }

    return kmp;
}

static kmp_api_t *ws_pae_supp_kmp_create_and_start(kmp_service_t *service, kmp_type_e type, pae_supp_t *pae_supp)
{
    // Create new instance
    kmp_api_t *kmp = kmp_api_create(service, type);
    if (!kmp) {
        return NULL;
    }

    // Updates parent address
    kmp_api_addr_set(kmp, pae_supp->entry.addr);

    // Sets security keys to KMP
    kmp_api_sec_keys_set(kmp, &pae_supp->entry.sec_keys);

    kmp_api_cb_register(
        kmp, ws_pae_supp_kmp_api_create_confirm,
        ws_pae_supp_kmp_api_create_indication,
        ws_pae_supp_kmp_api_finished_indication,
        ws_pae_supp_kmp_api_finished);

    kmp_entry_t *kmp_entry = ws_pae_lib_kmp_list_add(&pae_supp->entry.kmp_list, kmp);
    if (!kmp_entry) {
        kmp_api_delete(kmp);
        return NULL;
    }

    kmp_api_data_set(kmp, kmp_entry);

    if (kmp_api_start(kmp) < 0) {
        ws_pae_lib_kmp_list_delete(&pae_supp->entry.kmp_list, kmp);
        return NULL;
    }

    return kmp;
}

static void ws_pae_supp_kmp_api_create_confirm(kmp_api_t *kmp, kmp_result_e result)
{
    kmp_service_t *service = kmp_api_service_get(kmp);
    pae_supp_t *pae_supp = ws_pae_supp_by_kmp_service_get(service);
    if (!pae_supp) {
        return;
    }

    // KMP-CREATE.request has failed, authentication error
    if (result != KMP_RESULT_OK) {
        ws_pae_supp_authenticate_response(pae_supp, false);
    }
}

static void ws_pae_supp_kmp_api_create_indication(kmp_api_t *kmp, kmp_type_e type, kmp_addr_t *addr)
{
    (void) addr;
    (void) type;

    // For now, accept every KMP-CREATE.indication
    kmp_api_create_response(kmp, KMP_RESULT_OK);
}

static void ws_pae_supp_kmp_api_finished_indication(kmp_api_t *kmp, kmp_result_e result, kmp_sec_keys_t *sec_keys)
{
    kmp_service_t *service = kmp_api_service_get(kmp);
    pae_supp_t *pae_supp = ws_pae_supp_by_kmp_service_get(service);
    if (!pae_supp) {
        return;
    }

    kmp_type_e type = kmp_api_type_get(kmp);

    // Whenever EAP-TLS, 4WH or GKH completes increase timer for wait for authentication
    if (type < IEEE_802_1X_INITIAL_KEY && result == KMP_RESULT_OK) {
        ws_pae_lib_supp_timer_ticks_set(&pae_supp->entry, WAIT_FOR_AUTHENTICATION_TICKS);
    }

    sec_prot_keys_t *keys = sec_keys;
    // Key is to be inserted
    if (keys) {
        uint8_t gtk_index;
        uint8_t *gtk = sec_prot_keys_get_gtk_to_insert(keys->gtks, &gtk_index);
        if (gtk) {
            pae_supp->key_insert(pae_supp->interface_ptr, gtk_index, gtk);
            sec_prot_keys_gtk_insert_index_clear(keys->gtks);
        }
    }

    if ((type == IEEE_802_11_4WH || type == IEEE_802_11_GKH) && result == KMP_RESULT_OK) {
        ws_pae_supp_authenticate_response(pae_supp, true);
    }
}

static void ws_pae_supp_kmp_api_finished(kmp_api_t *kmp)
{
    kmp_service_t *service = kmp_api_service_get(kmp);
    pae_supp_t *pae_supp = ws_pae_supp_by_kmp_service_get(service);
    if (!pae_supp) {
        return;
    }

    // Delete KMP
    ws_pae_lib_kmp_list_delete(&pae_supp->entry.kmp_list, kmp);
}

#endif /* HAVE_PAE_SUPP */
#endif /* HAVE_WS */

