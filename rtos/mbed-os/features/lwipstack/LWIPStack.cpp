/* LWIP implementation of NSAPI NetworkStack
 * Copyright (c) 2017 ARM Limited
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
#include "nsapi.h"
#include "mbed_interface.h"
#include "mbed_assert.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "lwip/tcpip.h"
#include "lwip/tcp.h"
#include "lwip/ip.h"
#include "lwip/mld6.h"
#include "lwip/igmp.h"
#include "lwip/dns.h"
#include "lwip/udp.h"
#include "lwip/raw.h"
#include "lwip/lwip_errno.h"
#include "lwip-sys/arch/sys_arch.h"

#include "LWIPStack.h"

#ifndef LWIP_SOCKET_MAX_MEMBERSHIPS
#define LWIP_SOCKET_MAX_MEMBERSHIPS 4
#endif

void LWIP::socket_callback(struct netconn *nc, enum netconn_evt eh, u16_t len)
{
    // Filter send minus events
    if (eh == NETCONN_EVT_SENDMINUS && nc->state == NETCONN_WRITE) {
        return;
    }

    LWIP &lwip = LWIP::get_instance();

    lwip.adaptation.lock();

    for (int i = 0; i < MEMP_NUM_NETCONN; i++) {
        if (lwip.arena[i].in_use
                && lwip.arena[i].conn == nc
                && lwip.arena[i].cb) {
            lwip.arena[i].cb(lwip.arena[i].data);
        }
    }

    lwip.adaptation.unlock();
}

#if !LWIP_IPV4 || !LWIP_IPV6
static bool all_zeros(const uint8_t *p, int len)
{
    for (int i = 0; i < len; i++) {
        if (p[i]) {
            return false;
        }
    }

    return true;
}
#endif

static bool convert_lwip_addr_to_mbed(nsapi_addr_t *out, const ip_addr_t *in)
{
#if LWIP_IPV6
    if (IP_IS_V6(in)) {
        out->version = NSAPI_IPv6;
        SMEMCPY(out->bytes, ip_2_ip6(in), sizeof(ip6_addr_t));
        return true;
    }
#endif
#if LWIP_IPV4
    if (IP_IS_V4(in)) {
        out->version = NSAPI_IPv4;
        SMEMCPY(out->bytes, ip_2_ip4(in), sizeof(ip4_addr_t));
        return true;
    }
#endif
#if LWIP_IPV6 && LWIP_IPV4
    return false;
#endif
}

static bool convert_mbed_addr_to_lwip(ip_addr_t *out, const nsapi_addr_t *in)
{
#if LWIP_IPV6
    if (in->version == NSAPI_IPv6) {
        IP_SET_TYPE(out, IPADDR_TYPE_V6);
        SMEMCPY(ip_2_ip6(out), in->bytes, sizeof(ip6_addr_t));
        return true;
    }
#if !LWIP_IPV4
    /* For bind() and other purposes, need to accept "null" of other type */
    /* (People use IPv4 0.0.0.0 as a general null) */
    if (in->version == NSAPI_UNSPEC ||
            (in->version == NSAPI_IPv4 && all_zeros(in->bytes, 4))) {
        ip_addr_set_zero_ip6(out);
        return true;
    }
#endif
#endif

#if LWIP_IPV4
    if (in->version == NSAPI_IPv4) {
        IP_SET_TYPE(out, IPADDR_TYPE_V4);
        SMEMCPY(ip_2_ip4(out), in->bytes, sizeof(ip4_addr_t));
        return true;
    }
#if !LWIP_IPV6
    /* For symmetry with above, accept IPv6 :: as a general null */
    if (in->version == NSAPI_UNSPEC ||
            (in->version == NSAPI_IPv6 && all_zeros(in->bytes, 16))) {
        ip_addr_set_zero_ip4(out);
        return true;
    }
#endif
#endif

#if LWIP_IPV4 && LWIP_IPV6
    if (in->version == NSAPI_UNSPEC) {
#if IP_VERSION_PREF == PREF_IPV4
        ip_addr_set_zero_ip4(out);
#else
        ip_addr_set_zero_ip6(out);
#endif
        return true;
    }
#endif

    return false;
}

void LWIP::tcpip_init_irq(void *eh)
{
    LWIP *lwip = static_cast<LWIP *>(eh);
    lwip->tcpip_inited.release();
    sys_tcpip_thread_set();
}

/* LWIP network stack implementation */
LWIP::LWIP()
{
    default_interface = NULL;

    // Seed lwip random
    lwip_seed_random();

    // Initialise TCP sequence number
    uint32_t tcp_isn_secret[4];
    for (int i = 0; i < 4; i++) {
        tcp_isn_secret[i] = LWIP_RAND();
    }
    lwip_init_tcp_isn(0, (u8_t *) &tcp_isn_secret);

    tcpip_init(&LWIP::tcpip_init_irq, this);
    tcpip_inited.wait(0);

    // Zero out socket set
    arena_init();
}

nsapi_error_t LWIP::get_dns_server(int index, SocketAddress *address, const char *interface_name)
{
    int dns_entries = 0;

    for (int i = 0; i < DNS_MAX_SERVERS; i++) {
        const ip_addr_t *ip_addr = dns_getserver(i, interface_name);
        if (!ip_addr_isany(ip_addr)) {
            if (index == dns_entries) {
                nsapi_addr_t addr;
                convert_lwip_addr_to_mbed(&addr, ip_addr);
                address->set_addr(addr);
                return NSAPI_ERROR_OK;
            }
            dns_entries++;
        }
    }
    return NSAPI_ERROR_NO_ADDRESS;
}

void LWIP::tcpip_thread_callback(void *ptr)
{
    lwip_callback *cb = static_cast<lwip_callback *>(ptr);

    if (cb->delay) {
        sys_timeout(cb->delay, LWIP::tcpip_thread_callback, ptr);
        cb->delay = 0;
    } else {
        cb->callback();
        delete cb;
    }
}

nsapi_error_t LWIP::call_in(int delay, mbed::Callback<void()> func)
{
    lwip_callback *cb = new (std::nothrow) lwip_callback;
    if (!cb) {
        return NSAPI_ERROR_NO_MEMORY;
    }

    cb->delay = delay;
    cb->callback = func;

    if (tcpip_callback_with_block(LWIP::tcpip_thread_callback, cb, 1) != ERR_OK) {
        return NSAPI_ERROR_NO_MEMORY;
    }

    return NSAPI_ERROR_OK;
}

LWIP::call_in_callback_cb_t LWIP::get_call_in_callback()
{
    call_in_callback_cb_t cb(this, &LWIP::call_in);
    return cb;
}

const char *LWIP::get_ip_address()
{
    if (!default_interface) {
        return NULL;
    }

    const ip_addr_t *addr = get_ip_addr(true, &default_interface->netif);

    if (!addr) {
        return NULL;
    }
#if LWIP_IPV6
    if (IP_IS_V6(addr)) {
        return ip6addr_ntoa_r(ip_2_ip6(addr), ip_address, sizeof(ip_address));
    }
#endif
#if LWIP_IPV4
    if (IP_IS_V4(addr)) {
        return ip4addr_ntoa_r(ip_2_ip4(addr), ip_address, sizeof(ip_address));
    }
#endif
#if LWIP_IPV6 && LWIP_IPV4
    return NULL;
#endif
}

nsapi_error_t LWIP::socket_open(nsapi_socket_t *handle, nsapi_protocol_t proto)
{
    // check if network is connected
//    if (lwip_connected == NSAPI_STATUS_DISCONNECTED) {
//        return NSAPI_ERROR_NO_CONNECTION;
//    }

    // allocate a socket
    struct mbed_lwip_socket *s = arena_alloc();
    if (!s) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    enum netconn_type lwip_proto = proto == NSAPI_TCP ? NETCONN_TCP : NETCONN_UDP;

#if LWIP_IPV6
    // Enable IPv6 (or dual-stack)
    lwip_proto = (enum netconn_type)(lwip_proto | NETCONN_TYPE_IPV6);
#endif

    s->conn = netconn_new_with_callback(lwip_proto, &LWIP::socket_callback);

    if (!s->conn) {
        arena_dealloc(s);
        return NSAPI_ERROR_NO_SOCKET;
    }

    netconn_set_recvtimeout(s->conn, 1);
    *(struct mbed_lwip_socket **)handle = s;
    return 0;
}

nsapi_error_t LWIP::socket_close(nsapi_socket_t handle)
{
    struct mbed_lwip_socket *s = (struct mbed_lwip_socket *)handle;

    netbuf_delete(s->buf);
    err_t err = netconn_delete(s->conn);
    arena_dealloc(s);
    return err_remap(err);
}

nsapi_error_t LWIP::socket_bind(nsapi_socket_t handle, const SocketAddress &address)
{
    struct mbed_lwip_socket *s = (struct mbed_lwip_socket *)handle;
    ip_addr_t ip_addr;

    if (
#if LWIP_TCP
        (NETCONNTYPE_GROUP(s->conn->type) == NETCONN_TCP && s->conn->pcb.tcp->local_port != 0) ||
#endif
        (NETCONNTYPE_GROUP(s->conn->type) == NETCONN_UDP && s->conn->pcb.udp->local_port != 0)) {
        return NSAPI_ERROR_PARAMETER;
    }

    nsapi_addr_t addr = address.get_addr();
    if (!convert_mbed_addr_to_lwip(&ip_addr, &addr)) {
        return NSAPI_ERROR_PARAMETER;
    }

    if (!ip_addr_isany_val(ip_addr) && !is_local_addr(&ip_addr)) {
        return NSAPI_ERROR_PARAMETER;
    }

    err_t err = netconn_bind(s->conn, &ip_addr, address.get_port());
    return err_remap(err);
}

nsapi_error_t LWIP::socket_listen(nsapi_socket_t handle, int backlog)
{
#if LWIP_TCP
    struct mbed_lwip_socket *s = (struct mbed_lwip_socket *)handle;

    if (s->conn->pcb.tcp->local_port == 0) {
        return NSAPI_ERROR_PARAMETER;
    }

    err_t err = netconn_listen_with_backlog(s->conn, backlog);
    return err_remap(err);
#else
    return NSAPI_ERROR_UNSUPPORTED;
#endif
}

nsapi_error_t LWIP::socket_connect(nsapi_socket_t handle, const SocketAddress &address)
{
    struct mbed_lwip_socket *s = (struct mbed_lwip_socket *)handle;
    ip_addr_t ip_addr;

    nsapi_addr_t addr = address.get_addr();
    if (!convert_mbed_addr_to_lwip(&ip_addr, &addr)) {
        return NSAPI_ERROR_PARAMETER;
    }

    netconn_set_nonblocking(s->conn, false);
    err_t err = netconn_connect(s->conn, &ip_addr, address.get_port());
    netconn_set_nonblocking(s->conn, true);

    return err_remap(err);
}

nsapi_error_t LWIP::socket_accept(nsapi_socket_t server, nsapi_socket_t *handle, SocketAddress *address)
{
#if LWIP_TCP
    struct mbed_lwip_socket *s = (struct mbed_lwip_socket *)server;
    struct mbed_lwip_socket *ns = arena_alloc();
    if (!ns) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    if (s->conn->pcb.tcp->state != LISTEN) {
        return NSAPI_ERROR_PARAMETER;
    }

    err_t err = netconn_accept(s->conn, &ns->conn);
    if (err != ERR_OK) {
        arena_dealloc(ns);
        return err_remap(err);
    }

    netconn_set_recvtimeout(ns->conn, 1);
    *(struct mbed_lwip_socket **)handle = ns;

    ip_addr_t peer_addr;
    nsapi_addr_t addr;
    u16_t port;
    (void) netconn_peer(ns->conn, &peer_addr, &port);
    convert_lwip_addr_to_mbed(&addr, &peer_addr);

    if (address) {
        address->set_addr(addr);
        address->set_port(port);
    }

    netconn_set_nonblocking(ns->conn, true);

    return 0;
#else
    return NSAPI_ERROR_UNSUPPORTED;
#endif
}

nsapi_size_or_error_t LWIP::socket_send(nsapi_socket_t handle, const void *data, nsapi_size_t size)
{
    struct mbed_lwip_socket *s = (struct mbed_lwip_socket *)handle;
    size_t bytes_written = 0;

    err_t err = netconn_write_partly(s->conn, data, size, NETCONN_COPY, &bytes_written);
    if (err != ERR_OK) {
        return err_remap(err);
    }

    return (nsapi_size_or_error_t)bytes_written;
}

nsapi_size_or_error_t LWIP::socket_recv(nsapi_socket_t handle, void *data, nsapi_size_t size)
{
    struct mbed_lwip_socket *s = (struct mbed_lwip_socket *)handle;

    if (!s->buf) {
        err_t err = netconn_recv(s->conn, &s->buf);
        s->offset = 0;

        if (err != ERR_OK) {
            return err_remap(err);
        }
    }

    u16_t recv = netbuf_copy_partial(s->buf, data, (u16_t)size, s->offset);
    s->offset += recv;

    if (s->offset >= netbuf_len(s->buf)) {
        netbuf_delete(s->buf);
        s->buf = 0;
    }

    return recv;
}

nsapi_size_or_error_t LWIP::socket_sendto(nsapi_socket_t handle, const SocketAddress &address, const void *data, nsapi_size_t size)
{
    struct mbed_lwip_socket *s = (struct mbed_lwip_socket *)handle;
    ip_addr_t ip_addr;

    nsapi_addr_t addr = address.get_addr();
    if (!convert_mbed_addr_to_lwip(&ip_addr, &addr)) {
        return NSAPI_ERROR_PARAMETER;
    }

    struct netbuf *buf = netbuf_new();

    err_t err = netbuf_ref(buf, data, (u16_t)size);
    if (err != ERR_OK) {
        netbuf_free(buf);
        return err_remap(err);
    }

    err = netconn_sendto(s->conn, buf, &ip_addr, address.get_port());
    netbuf_delete(buf);
    if (err != ERR_OK) {
        return err_remap(err);
    }

    return size;
}

nsapi_size_or_error_t LWIP::socket_recvfrom(nsapi_socket_t handle, SocketAddress *address, void *data, nsapi_size_t size)
{
    struct mbed_lwip_socket *s = (struct mbed_lwip_socket *)handle;
    struct netbuf *buf;

    err_t err = netconn_recv(s->conn, &buf);
    if (err != ERR_OK) {
        return err_remap(err);
    }

    if (address) {
        nsapi_addr_t addr;
        convert_lwip_addr_to_mbed(&addr, netbuf_fromaddr(buf));
        address->set_addr(addr);
        address->set_port(netbuf_fromport(buf));
    }

    u16_t recv = netbuf_copy(buf, data, (u16_t)size);
    netbuf_delete(buf);

    return recv;
}

int32_t LWIP::find_multicast_member(const struct mbed_lwip_socket *s, const nsapi_ip_mreq_t *imr)
{
    uint32_t count = 0;
    uint32_t index = 0;
    // Set upper limit on while loop, should break out when the membership pair is found
    while (count < s->multicast_memberships_count) {
        index = next_registered_multicast_member(s, index);

        if (memcmp(&s->multicast_memberships[index].imr_multiaddr, &imr->imr_multiaddr, sizeof(nsapi_addr_t)) == 0 &&
                memcmp(&s->multicast_memberships[index].imr_interface, &imr->imr_interface, sizeof(nsapi_addr_t)) == 0) {
            return index;
        }
        count++;
        index++;
    }

    return -1;
}

nsapi_error_t LWIP::setsockopt(nsapi_socket_t handle, int level, int optname, const void *optval, unsigned optlen)
{
    struct mbed_lwip_socket *s = (struct mbed_lwip_socket *)handle;

    switch (optname) {
        case NSAPI_BIND_TO_DEVICE:
            if (optlen > NSAPI_INTERFACE_NAME_MAX_SIZE) {
                return NSAPI_ERROR_UNSUPPORTED;
            }
#if LWIP_TCP
            if (NETCONNTYPE_GROUP(s->conn->type) == NETCONN_TCP) {
                s->conn->pcb.tcp->interface_name = (const char *)optval;
            }
#endif
            if (NETCONNTYPE_GROUP(s->conn->type) == NETCONN_UDP) {
                s->conn->pcb.udp->interface_name = (const char *)optval;
            }
#if LWIP_RAW
            if (NETCONNTYPE_GROUP(s->conn->type) == NETCONN_RAW) {
                s->conn->pcb.raw->interface_name = (const char *)optval;
            }
#endif
            return 0;
#if LWIP_TCP
        case NSAPI_KEEPALIVE:
            if (optlen != sizeof(int) || NETCONNTYPE_GROUP(s->conn->type) != NETCONN_TCP) {
                return NSAPI_ERROR_UNSUPPORTED;
            }

            s->conn->pcb.tcp->so_options |= SOF_KEEPALIVE;
            return 0;

        case NSAPI_KEEPIDLE:
            if (optlen != sizeof(int) || NETCONNTYPE_GROUP(s->conn->type) != NETCONN_TCP) {
                return NSAPI_ERROR_UNSUPPORTED;
            }

            s->conn->pcb.tcp->keep_idle = *(int *)optval;
            return 0;

        case NSAPI_KEEPINTVL:
            if (optlen != sizeof(int) || NETCONNTYPE_GROUP(s->conn->type) != NETCONN_TCP) {
                return NSAPI_ERROR_UNSUPPORTED;
            }

            s->conn->pcb.tcp->keep_intvl = *(int *)optval;
            return 0;
#endif

        case NSAPI_REUSEADDR:
            if (optlen != sizeof(int)) {
                return NSAPI_ERROR_UNSUPPORTED;
            }

            if (*(int *)optval) {
                ip_set_option(s->conn->pcb.ip, SOF_REUSEADDR);
            } else {
                ip_reset_option(s->conn->pcb.ip, SOF_REUSEADDR);
            }
            return 0;

        case NSAPI_ADD_MEMBERSHIP:
        case NSAPI_DROP_MEMBERSHIP: {
            if (optlen != sizeof(nsapi_ip_mreq_t)) {
                return NSAPI_ERROR_PARAMETER;
            }
            err_t igmp_err;
            const nsapi_ip_mreq_t *imr = static_cast<const nsapi_ip_mreq_t *>(optval);

            /* Check interface address type matches group, or is unspecified */
            if (imr->imr_interface.version != NSAPI_UNSPEC && imr->imr_interface.version != imr->imr_multiaddr.version) {
                return NSAPI_ERROR_PARAMETER;
            }

            ip_addr_t if_addr;
            ip_addr_t multi_addr;

            /* Convert the group address */
            if (!convert_mbed_addr_to_lwip(&multi_addr, &imr->imr_multiaddr)) {
                return NSAPI_ERROR_PARAMETER;
            }

            /* Convert the interface address, or make sure it's the correct sort of "any" */
            if (imr->imr_interface.version != NSAPI_UNSPEC) {
                if (!convert_mbed_addr_to_lwip(&if_addr, &imr->imr_interface)) {
                    return NSAPI_ERROR_PARAMETER;
                }
            } else {
                ip_addr_set_any(IP_IS_V6(&if_addr), &if_addr);
            }

            igmp_err = ERR_USE; // Maps to NSAPI_ERROR_UNSUPPORTED
            int32_t member_pair_index = find_multicast_member(s, imr);

            if (optname == NSAPI_ADD_MEMBERSHIP) {
                if (!s->multicast_memberships) {
                    // First multicast join on this socket, allocate space for membership tracking
                    s->multicast_memberships = (nsapi_ip_mreq_t *)malloc(sizeof(nsapi_ip_mreq_t) * LWIP_SOCKET_MAX_MEMBERSHIPS);
                    if (!s->multicast_memberships) {
                        return NSAPI_ERROR_NO_MEMORY;
                    }
                } else if (s->multicast_memberships_count == LWIP_SOCKET_MAX_MEMBERSHIPS) {
                    return NSAPI_ERROR_NO_MEMORY;
                }

                if (member_pair_index != -1) {
                    return NSAPI_ERROR_ADDRESS_IN_USE;
                }

                member_pair_index = next_free_multicast_member(s, 0);

                adaptation.lock();

#if LWIP_IPV4
                if (IP_IS_V4(&if_addr)) {
                    igmp_err = igmp_joingroup(ip_2_ip4(&if_addr), ip_2_ip4(&multi_addr));
                }
#endif
#if LWIP_IPV6
                if (IP_IS_V6(&if_addr)) {
                    igmp_err = mld6_joingroup(ip_2_ip6(&if_addr), ip_2_ip6(&multi_addr));
                }
#endif

                adaptation.unlock();

                if (igmp_err == ERR_OK) {
                    set_multicast_member_registry_bit(s, member_pair_index);
                    s->multicast_memberships[member_pair_index] = *imr;
                    s->multicast_memberships_count++;
                }
            } else {
                if (member_pair_index == -1) {
                    return NSAPI_ERROR_NO_ADDRESS;
                }

                clear_multicast_member_registry_bit(s, member_pair_index);
                s->multicast_memberships_count--;

                adaptation.lock();

#if LWIP_IPV4
                if (IP_IS_V4(&if_addr)) {
                    igmp_err = igmp_leavegroup(ip_2_ip4(&if_addr), ip_2_ip4(&multi_addr));
                }
#endif
#if LWIP_IPV6
                if (IP_IS_V6(&if_addr)) {
                    igmp_err = mld6_leavegroup(ip_2_ip6(&if_addr), ip_2_ip6(&multi_addr));
                }
#endif

                adaptation.unlock();
            }

            return err_remap(igmp_err);
        }

        default:
            return NSAPI_ERROR_UNSUPPORTED;
    }
}

nsapi_error_t LWIP::getsockopt(nsapi_socket_t handle, int level, int optname, void *optval, unsigned *optlen)
{
    return NSAPI_ERROR_UNSUPPORTED;
}


void LWIP::socket_attach(nsapi_socket_t handle, void (*callback)(void *), void *data)
{
    struct mbed_lwip_socket *s = (struct mbed_lwip_socket *)handle;

    s->cb = callback;
    s->data = data;
}

LWIP &LWIP::get_instance()
{
    static LWIP lwip;
    return lwip;
}

// This works as long as it's not ever set to something which corresponds to
// a macro defined as a non-integer. Eg `#define Nanostack "Foo"`
#define LWIP 0x11991199
#if MBED_CONF_NSAPI_DEFAULT_STACK == LWIP
#undef LWIP
OnboardNetworkStack &OnboardNetworkStack::get_default_instance()
{
    return LWIP::get_instance();
}
#endif
