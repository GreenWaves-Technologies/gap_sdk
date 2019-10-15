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

#ifndef __RSP_PACKET_CODEC_H__
#define __RSP_PACKET_CODEC_H__

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <string>
#include <exception>
#include <functional>
#include <chrono>

#include "log/log.hpp"
#include "events/events.hpp"
#include "events/tcp-events.hpp"

#define RSP_PACKET_MAX_LEN 4096u
#define RSP_CTRLC_CHAR 0x03
#define RSP_ACK_STR "+"
#define RSP_ACK_CHAR '+'
#define RSP_PKT_START_CHAR '$'
#define RSP_PKT_END_CHAR '#'

class RspPacketCodecException : public std::exception {
public:
    RspPacketCodecException(const std::string& msg) : msg(msg) {}
    ~RspPacketCodecException() {}

    const char* what() const throw() {
        return(msg.c_str());
    }
private:
    std::string msg;
};

class RspPacketCodec {
public:
    using PacketDecodedProc = std::function<void(char * pkt, size_t pkt_len)>;
    using PacketErrorProc = std::function<void(const char * err_str)>;
    using PacketCtrlCProc = std::function<void()>;
    using PacketAckProc = std::function<void()>;

    enum RspPacketDecoderStates {
        STATE_INIT = 0,
        STATE_LEADIN,
        STATE_BODY,
        STATE_CRC
    };

    RspPacketCodec(EventLoop::SpEventLoop event_loop, std::chrono::milliseconds decode_timeout);
    virtual ~RspPacketCodec() {}

    void decode(const std::shared_ptr<CircularCharBuffer> &circ_buf);
    void reset_state();

    void on_packet(const PacketDecodedProc &cb) { m_pkt_decoded = cb; }
    void on_error(const PacketErrorProc &cb) { m_pkt_error = cb; }
    void on_ctrlc(const PacketCtrlCProc &cb) { m_pkt_ctrlc = cb; }
    void on_ack(const PacketAckProc &cb) { m_pkt_ack = cb; }

    bool encode(const char * buf, size_t len, const std::shared_ptr<CircularCharBuffer> &circ_buf, bool dont_encode_runs);
    bool encode_ack(const std::shared_ptr<CircularCharBuffer> &circ_buf);
private:
    char s_out_pkt[RSP_PACKET_MAX_LEN];

    bool decoder(const std::shared_ptr<CircularCharBuffer> &circ_buf);
    int64_t decode_timeout();

    std::chrono::milliseconds m_timeout;
    
    RspPacketDecoderStates m_decode_state;
    char m_pkt[RSP_PACKET_MAX_LEN];
    size_t m_cur, m_last, m_pkt_len, m_crc_start;
    bool m_escaped;
    EventLoop::SpTimerEvent m_te;
    PacketDecodedProc m_pkt_decoded;
    PacketErrorProc m_pkt_error;
    PacketCtrlCProc m_pkt_ctrlc;
    PacketAckProc m_pkt_ack;
    Log m_log;
};

#endif