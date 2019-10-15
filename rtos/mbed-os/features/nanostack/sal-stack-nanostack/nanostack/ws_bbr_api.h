/*
 * Copyright (c) 2017-2019, Arm Limited and affiliates.
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

/**
 * \file ws_bbr_api.h
 * \brief Wi-SUN backbone border router (BBR) application interface.
 *
 * This is Wi-SUN backbone Border router service.
 * When started the module takes care of starting the
 * components that enables default border router functionality in Wi-SUN network.
 *
 */

#ifndef WS_BBR_API_H_
#define WS_BBR_API_H_

#include "ns_types.h"

/**
 * Start backbone border router service.
 *
 * if backbone interface is enabled and allows routing.
 *    Enables ND proxy for address found from backbone
 *
 * \param interface_id Wi-SUN network interface id.
 * \param backbone_interface_id backbone interface id.
 *
 * \return 0 on success
 * \return <0 in case of errors
 *
 */
int ws_bbr_start(int8_t interface_id, int8_t backbone_interface_id);

/**
 * Stop backbone Border router.
 *
 * \param interface_id interface ID of the Wi-SUN network
 *
 * \return 0 on success
 * \return <0 in case of errors
 *
 */
void ws_bbr_stop(int8_t interface_id);

/**
 * Remove node's keys from border router
 *
 * Removes node's keys from border router i.e. Pairwise Master Key (PMK)
 * and Pairwise Transient Key (PTK). This function is used on revocation of
 * node's access procedure after authentication service is configured
 * to reject authentication attempts of the node (e.g. node's certificate is
 * revoked). Sub sequential calls to function can be used to remove several
 * nodes from border router.
 *
 * \param interface_id Network interface ID.
 * \param eui64 EUI-64 of revoked node
 *
 * \return 0, Node's keys has been removed
 * \return <0 Node's key remove has failed (e.g. unknown address)
 */
int ws_bbr_node_keys_remove(int8_t interface_id, uint8_t *eui64);

/**
 * Start revocation of node's access
 *
 * Starts revocation of node's access procedure on border router. Before
 * the call to this function, authentication service must be configured to
 * reject authentication attempts of the removed nodes (e.g. certificates
 * of the nodes are revoked). Also the keys for the nodes must be removed
 * from the border router.
 *
 * \param interface_id Network interface ID.
 *
 * \return 0, Revocation started OK.
 * \return <0 Revocation start failed.
 */
int ws_bbr_node_access_revoke_start(int8_t interface_id);

#endif /* WS_BBR_API_H_ */
