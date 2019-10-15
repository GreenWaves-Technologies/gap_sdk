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

#include "gdb-server/rsp-packet-codec.hpp"

static size_t deescape(char * buf, size_t len)
{
  size_t i = 0, cur = 0;
  bool escaped = false;
  while (i < len) {
    if (escaped) {
      escaped = false;
      buf[cur++] = buf[i++] ^ 0x20;
    } else if (buf[i] == '}') {
      escaped = true;
      i++;
    } else {
      if (i != cur) buf[cur] = buf[i];
      i++; cur++;
    }
  }
  return cur;
}

static bool scan_for_hash(const char *pkt, size_t *cur, bool *escaped, size_t last)
{
  while (*cur < last) {
    if (*escaped) {
      *escaped = false;
    } else if (pkt[*cur] == '}') {
      *escaped = true;
    } else if (pkt[*cur] == '#') {
      return true;
    }
    (*cur)++;
  }
  return false;
}

static bool verify_checksum(const char * buf, size_t buf_len, const char * crc_buf)
{
  unsigned int checksum = 0;
  for(size_t i = 0; i < buf_len; i++) {
    checksum += buf[i];
  }
  checksum = checksum % 256;
  char checksum_str[3];
  snprintf(checksum_str, 3, "%02x", checksum);
  return (crc_buf[0] == checksum_str[0] && crc_buf[1] == checksum_str[1]);
}

static void flush_seq(char c, size_t seq_len, char *raw, size_t *raw_idx, uint32_t *crc)
{
    while (seq_len > 0) {
        if (seq_len < 3) {
            for (size_t i=0; i<seq_len; i++) {
                raw[(*raw_idx)++] = c;
                *crc += c;
            }
            return;
        } else {
            size_t run_len = seq_len;
            if (run_len == 6 or run_len == 7)
                run_len = 5;
            else if (run_len + 29 > 126)
                run_len = 126 - 29;
            seq_len -= run_len;
            run_len += 29;
            raw[(*raw_idx)++] = c;
            *crc += c;
            raw[(*raw_idx)++] = '*';
            *crc += '*';
            raw[(*raw_idx)++] = run_len;
            *crc += run_len;
        }
    }
}

static void escape(const char *data, size_t len, char *raw, size_t *raw_idx, uint32_t *crc, bool dont_encode_runs)
{
    size_t i, seq_len = 0;
    char seq_c;

    *crc = 0;
    for (i = 0; i < len; i++) {
        char c = data[i];
        // check if escaping needed
        if (c == '#' || c == '%' || c == '}' || c == '*') {
            if (seq_len > 0) {
                flush_seq(seq_c, seq_len, raw, raw_idx, crc);
                seq_len = 0;
            }
            raw[(*raw_idx)++] = '}';
            raw[(*raw_idx)++] = c ^ 0x20;
            *crc += '}';
            *crc += c ^ 0x20;
        } else {
            // encode runs
            if (dont_encode_runs) {
                raw[(*raw_idx)++] = c;
                *crc += c;
            } else {
                if (seq_len > 0) {
                    if (c != seq_c) {
                        flush_seq(seq_c, seq_len, raw, raw_idx, crc);
                        seq_len = 1;
                        seq_c = c;
                    } else {
                        seq_len++;
                    }
                } else {
                    seq_c = c;
                    seq_len = 1;
                }
            }
        }
    }
    if (seq_len > 0) flush_seq(seq_c, seq_len, raw, raw_idx, crc);
}

RspPacketCodec::RspPacketCodec(EventLoop::SpEventLoop event_loop, std::chrono::milliseconds decode_timeout) : 
    m_timeout(decode_timeout), m_log("RSPC") {
    m_te = event_loop->getTimerEvent(std::bind(&RspPacketCodec::decode_timeout, this));
    reset_state();
}

int64_t RspPacketCodec::decode_timeout() {
    reset_state();
    m_pkt_error("decode of packet timed out");
    return kEventLoopTimerDone;
}

void RspPacketCodec::reset_state() {
    m_cur = m_last = m_pkt_len = m_crc_start = 0;
    m_decode_state = STATE_LEADIN;
    m_escaped = false;
    m_te->setTimeout(kEventLoopTimerDone);
}

void RspPacketCodec::decode(const std::shared_ptr<CircularCharBuffer> &circ_buf) {
    while (decoder(circ_buf)) {}
}

bool RspPacketCodec::decoder(const std::shared_ptr<CircularCharBuffer> &circ_buf) {
    char * buf = NULL;
    size_t buf_len = 0, buf_idx = 0, start_idx;
    // printf("state %d\n", m_decode_state);
    switch(m_decode_state) {
        case STATE_INIT:
            reset_state();
            return true;
            
        case STATE_LEADIN:
            buf_idx = 0;
            circ_buf->read_block((void**)&buf, &buf_len);
            if (buf_len == 0) return false;
            while (buf_idx < buf_len) {
                switch (buf[buf_idx++]) {
                    case RSP_CTRLC_CHAR:
                        circ_buf->commit_read(buf_idx);
                        m_pkt_ctrlc();
                        return true;
                    case RSP_ACK_CHAR:
                        circ_buf->commit_read(buf_idx);
                        m_pkt_ack();
                        return true;
                    case RSP_PKT_START_CHAR:
                        circ_buf->commit_read(buf_idx);
                        m_decode_state = STATE_BODY;
                        m_te->setTimeout(m_timeout.count() * 1000);
                        return true;
                }
            }
            // could get second part of circular buffer
            circ_buf->commit_read(buf_idx);
            return true;

        case STATE_BODY:
            if (buf_idx >= buf_len)
                circ_buf->read_block((void**)&buf, &buf_len);

            if (buf_len > 0) {
                // Look for # skipping escaped characters. Maintain the escaped state in
                // case a packet is split on the } character

                start_idx = buf_idx;
                bool hash_found = scan_for_hash(buf, &buf_idx, &m_escaped, buf_len);
                memcpy(&m_pkt[m_cur], &buf[start_idx], buf_idx - start_idx);
                m_cur += buf_idx - start_idx;
                if (hash_found) {
                    // step past the hash
                    circ_buf->commit_read(++buf_idx);
                    m_decode_state = STATE_CRC;
                    return true;
                } else {
                    circ_buf->commit_read(buf_idx);
                }
            }
            if (m_cur >= RSP_PACKET_MAX_LEN) {
                m_decode_state = STATE_INIT;
                m_pkt_error("too many characters received");
                return true;
            }
            return (buf_len>0);

        case STATE_CRC:
            if (buf_len - buf_idx < 2)
                circ_buf->read_block((void**)&buf, &buf_len);
            // Look for enough characters to process the CRC
            if (buf_len - buf_idx >= 2) {
                if (verify_checksum(m_pkt, m_cur, &buf[buf_idx])) {
                    circ_buf->commit_read(2);
                    m_pkt_len = deescape(m_pkt, m_cur);
                    // clear the rest of the buffer
                    memset(&(m_pkt[m_pkt_len]), 0, RSP_PACKET_MAX_LEN - m_pkt_len);
                    m_log.protocol("received packet: %s\n", m_pkt);
                    m_decode_state = STATE_INIT;
                    m_pkt_decoded(m_pkt, m_pkt_len);
                } else {
                    circ_buf->commit_read(2);
                    m_decode_state = STATE_INIT;

                    m_pkt_error("packet crc error");
                }
            }
            m_te->setTimeout(-1);
            return false;

    }
    return false;
}


bool RspPacketCodec::encode(const char * buf, size_t len, const std::shared_ptr<CircularCharBuffer> &circ_buf, bool dont_encode_runs) {
    size_t out_len = 0;
    uint32_t checksum = 0;

    s_out_pkt[out_len++] = '$';

    escape(buf, len, s_out_pkt, &out_len, &checksum, dont_encode_runs);

    // add checksum
    checksum = checksum % 256;
    char checksum_str[3];
    snprintf(checksum_str, 3, "%02x", checksum);

    s_out_pkt[out_len++] = '#';
    s_out_pkt[out_len++] = checksum_str[0];
    s_out_pkt[out_len++] = checksum_str[1];
    if (m_log.is_protocol_lvl()) {
        s_out_pkt[out_len] = 0;
        m_log.protocol("sending packet: %s\n", s_out_pkt);
    }

    return circ_buf->write_copy(s_out_pkt, out_len) == out_len;
}

bool RspPacketCodec::encode_ack(const std::shared_ptr<CircularCharBuffer> &circ_buf) {
    m_log.protocol("sending: ack packet\n");
    return (circ_buf->write_copy(RSP_ACK_STR, strlen(RSP_ACK_STR)) == strlen(RSP_ACK_STR));
}