/*
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
 * Authors: Andreas Traber
 */

/*
 * Olimex ARM-USB-OCD, ARM-USB-OCD-H, ARM-USB-TINY and ARM-USB-TINY-H pinout
 *
 * GPIO     Function      Comment
 *  0       TCK           Set as output but do not drive manually
 *  1       TDI           Set as output but do not drive manually
 *  2       TDO           Set as input, should not be driven
 *  3       TMS           Set as output but do not drive manually
 *  4       ??            GPIOL0 of FT2232H, set as output 
 *  5       ??            GPIOL1 of FT2232H, set as input, should not be driven
 *  6       ??            GPIOL2 of FT2232H, set as input, should not be driven
 *  7       RTCK          GPIOL3 of FT2232H, set as input, should not be driven
 *  8       TRST*)        Set as output
 *  9       SRST**)       Set as output
 * 10       ??            GPIOH2 of FT2232H, set as output
 * 11       Red LED       Set as output
 * 12       ??            GPIOH4 of FT2232H, set as input, should not be driven
 * 13       ??            GPIOH5 of FT2232H, set as input, should not be driven
 * 14       ??            GPIOH6 of FT2232H, set as input, should not be driven
 * 15       ??            GPIOH7 of FT2232H, set as input, should not be driven
 *
 *  *)  Pin is driven by Olimex in an active-high way, i.e. 0: driven to GND by Olimex
 *                                                          1: driven to VRef by Olimex
 *  **) Pin is driven by Olimex in an active-low way, i.e.  0: not driven by Olimex -> needs to be driven to high from DUT side (V_reset)
 *                                                          1: driven to GND by Olimex
 *
 * Use command SET_BITS_LOW for GPIO 0 - 7 (byte0: value, byte1: direction)
 * Use command SET_BITS_HIGH for GPIO 8 - 15 (byte0: value, byte1: direction)
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <string>

#include "ftdi/ftdi.hpp"
#include "log.hpp"

// Macros to make config commands clearer

#define SET_DATA_BUFFER(__buf, __idx, __value) __buf[__idx++] = __value
#define SET_DATA_BUFFER2(__buf, __idx, __cmd, __arg1, __arg2) \
  do { \
    SET_DATA_BUFFER(__buf, __idx, __cmd); \
    SET_DATA_BUFFER(__buf, __idx, __arg1); \
    SET_DATA_BUFFER(__buf, __idx, __arg2); \
  } while (0)
#define SET_DATA_BITS_LOW_BYTE(__buf, __idx, __value, __direction) SET_DATA_BUFFER2(__buf, __idx, SET_BITS_LOW, __value, __direction)
#define SET_DATA_BITS_HIGH_BYTE(__buf, __idx, __value, __direction) SET_DATA_BUFFER2(__buf, __idx, SET_BITS_HIGH, __value, __direction)
#define SET_TCK_DIVISOR(__buf, __idx, __valueL, __valueH) SET_DATA_BUFFER2(__buf, __idx, TCK_DIVISOR, __valueL, __valueH)


#ifndef min
#define min(X,Y) ((X) < (Y) ? (X) : (Y))
#endif

//-----------------------------------------------------------------------------

Ftdi::Ftdi(js::config *config, FTDIDeviceID id, cable_cb_t cable_state_cb) : Cable(config), log ("FDTI"), m_id (id), cable_state_cb(cable_state_cb)
{
  // add all our known devices to the map
  m_descriptors[Olimex].push_back(device_desc(0x15ba, 0x002a));
  m_descriptors[Olimex].push_back(device_desc(0x15ba, 0x002b));

  m_descriptors[Digilent].push_back(device_desc(0x0403, 0x6010)); // ftdi2232 Gapuino
  m_descriptors[Digilent].push_back(device_desc(0x1d6b, 0x0002)); // ftdi2232 Gapuino
  memset(&m_params, 0, sizeof(m_params));
}

Ftdi::~Ftdi()
{
  ftdi_usb_close(&m_ftdic);
  ftdi_deinit(&m_ftdic);

  if (m_params.send_buf) free(m_params.send_buf);
  if (m_params.recv_buf) free(m_params.recv_buf);
}

void Ftdi::fatal_error(const char *str, ...)
{
  va_list va;
  va_start(va, str);
  log.print(LOG_ERROR, str, va);
  va_end(va);
  if (current_state == CABLE_CONNECTED) {
    current_state = CABLE_DISCONNECTED;
    if (cable_state_cb) cable_state_cb(CABLE_DISCONNECTED);
  }
}

bool
Ftdi::connect(js::config *config)
{
  unsigned char buf[256];
  int rst_len = 0;
  std::list<struct device_desc> dev_desc = m_descriptors[m_id];
  int error;
  const char *description = NULL;
  struct ftdi_version_info version;
  
  if (config && config->get("description") != NULL)
  {
    description = config->get("description")->get_str().c_str();
  }

  m_params.send_buf_len   = FTDX_MAXSEND;
  m_params.send_buffered  = 0;
  m_params.send_buf       = (char*)malloc(m_params.send_buf_len);
  m_params.recv_buf_len   = FTDI_MAXRECV;
  m_params.to_recv        = 0;
  m_params.recv_write_idx = 0;
  m_params.recv_read_idx  = 0;
  m_params.recv_buf       = (char*)malloc(m_params.recv_buf_len);

  if (!m_params.send_buf || !m_params.recv_buf) {
    log.error("ftdi2232: Can't allocate memory for ftdi context structures\n");
    goto fail;
  }

  error = ftdi_init(&m_ftdic);
  if (error < 0) {
    log.error("ftdi2232: Unable to initialize LibFTDI context: %d\n", error);
    goto fail;
  }

  version = ftdi_get_library_version();
  log.user("ftdi2232 libftdi version %s (major: %d, minor: %d, micro: %d, snapshot ver: %s)\n",
      version.version_str, version.major, version.minor, version.micro,
      version.snapshot_str);

  // TODO - Should we ftdi_set_interface here? It works without but probably because the default is the first

  //---------------------------------------------------------------------------
  // Device Selection
  if (description == NULL) {
    std::list<struct device_desc> dev_available;
    struct device_desc *dev = nullptr;

    for (std::list<struct device_desc>::iterator it = dev_desc.begin();
         it != dev_desc.end(); it++) {
      struct ftdi_device_list* devlist;
      int n = ftdi_usb_find_all(&m_ftdic, &devlist, it->vid, it->pid);

      log.debug("iterating %d devices\n", n);
      if (n > 0) {
        for(int i = 0; i < n; ++i) {
          log.debug("try opening i:0x%X:0x%X:%d\n", it->vid, it->pid, i);
          if (dev_try_open(it->vid, it->pid, i)) {
            log.user("Found ftdi device i:0x%X:0x%X:%d\n",
                       it->vid, it->pid, i);
            dev_available.push_back(device_desc(it->vid, it->pid, (unsigned int)i));
            break;
          }
        }
      }

      ftdi_list_free2(devlist);
    }

    if (dev_available.size() == 0) {
      log.error("ft2232: Connection failed\n");
      goto fail;
    }

    dev = &dev_available.front();
    log.user("Connecting to ftdi device i:0x%X:0x%X:%d\n",
               dev->vid, dev->pid, dev->index);
    error = ftdi_usb_open_desc_index(&m_ftdic, dev->vid, dev->pid, NULL, NULL, dev->index);
  } else {
    log.user("Connecting to ftdi device %s\n", description);
    error = ftdi_usb_open_string(&m_ftdic, description);
  }

  if (error != 0) {
    if      (error ==  -1) log.debug("usb_find_busses() failed\n");
    else if (error ==  -2) log.debug("usb_find_devices() failed\n");
    else if (error ==  -3) log.debug("usb device not found\n");
    else if (error ==  -4) log.debug("unable to open device\n");
    else if (error ==  -5) log.debug("unable to claim device\n");
    else if (error ==  -6) log.debug("reset failed\n");
    else if (error ==  -7) log.debug("set baudrate failed\n");
    else if (error ==  -8) log.debug("get product description failed\n");
    else if (error ==  -9) log.debug("get serial number failed\n");
    else if (error == -10) log.debug("unable to close device\n");
    else if (error == -11) log.debug("ftdi context invalid\n");
    else if (error == -12) log.debug("libusb_get_device_list() failed\n");
    else if (error == -13) log.debug("libusb_get_device_descriptor() failed\n");

    log.warning("ft2232: Connection failed\n");

    goto fail;
  }

  if(!ft2232_mpsse_open()) {
    log.error("ft2232: Open MPSSE mode failed\n");
    goto fail;
  }

  log.debug("Connected to libftdi driver.\n");

  //---------------------------------------------------------------------------
  // Setup layout for different devices

  if (m_id == Olimex)
  {
    bits_value = 0x1 << 8;
    // This is superfluous - remove if you have the ability to test
    set_bit_direction(8, 1);
    set_bit_direction(9, 1);

    SET_DATA_BITS_LOW_BYTE(buf, rst_len, 0x00, 0x1b); // Set value & direction of ADBUS lines, direction (1 == output)
    SET_TCK_DIVISOR(buf, rst_len, 0x01, 0x00); // We cannot go below that on fulmine
    SET_DATA_BUFFER(buf, rst_len, SEND_IMMEDIATE);
  }
  else if (m_id == Digilent)
  {
    // these are used as the base for set_bit_value
    bits_value = 0x7 << 4;
    bits_direction = 0x7b;

    SET_DATA_BITS_LOW_BYTE(buf, rst_len, bits_value, bits_direction); // direction (1 == output)  0111 1011 values 0111 0000
    // SET_DATA_BUFFER(buf, rst_len, EN_DIV_5); // Enable clock divide by 5
    SET_TCK_DIVISOR(buf, rst_len, 0x02, 0x00); // The divisor has been put to 2 as is not reliable on gap board with less
    // SET_DATA_BUFFER(buf, rst_len, DIS_ADAPTIVE); // Make sure adaptive clocking is disabled
    SET_DATA_BUFFER(buf, rst_len, SEND_IMMEDIATE);
  }
  else
  {
    log.error("ft2232: Unsupported device\n");
    goto fail;
  }

  if(ft2232_write((char *)buf, rst_len, 0) != rst_len) {
    log.error("ft2232: Initial write failed\n");
    goto fail;
  }

  flush();
  current_state = CABLE_CONNECTED;
  cable_state_cb(CABLE_CONNECTED);
  return true;

fail:
  if (m_params.send_buf) {
    free(m_params.send_buf);
    m_params.send_buf = NULL;
  }
  if (m_params.recv_buf) {
    free(m_params.recv_buf);
    m_params.recv_buf = NULL;
  }

  return false;
}

bool Ftdi::purge()
{
  int ret = 0;
  m_params.send_buffered  = 0;
  m_params.to_recv        = 0;
  m_params.recv_write_idx = 0;
  m_params.recv_read_idx  = 0;
  ret = ftdi_usb_purge_buffers(&m_ftdic);
  if (ret < 0) {
    fatal_error("ft2232: ftdi_usb_purge_buffers() failed - %s\n", ftdi_get_error_string(&m_ftdic));
    return false;
  }
  return true;
}

bool Ftdi::chip_reset(bool active)
{
  log.debug("ft2232: chip reset (active: %d)\n", active);
  if (m_id == Olimex)
  {
    std::string chip = this->config->get("**/chip/name")->get_str();

    if (chip == "vivosoc2" || chip == "vivosoc2_1")
    {
      if (active)
      {
        unsigned char buf[256];
        buf[0] = SET_BITS_HIGH;
        buf[1] = ~0x01;
        buf[2] = 0x3;
        buf[3] = SET_BITS_HIGH;
        buf[4] = ~0x02;
        buf[5] = 0x3;
        buf[6] = SEND_IMMEDIATE;

        if(ft2232_write((char *)buf, 7, 0) != 7) return false;
      }
      return true;
    }
    else
    {
      // Bit 9 is chip reset and active high.
      return set_bit_value(9, active);
    }
  } else if (m_id == Digilent) { // ftdi2232 Gapuino
    // Bit 4 is chip reset and active high.
    return set_bit_value(4, !active);
  } else {
    // not supported
    return false;
  }
}

bool Ftdi::chip_setVQPS(bool high)
{
	if (high)
		log.debug("ft223: chip VQPS set to 2.5V\n");
	else
		log.debug("ft223: chip VQPS set to 0V\n");

	if (m_id == Digilent)
	{
   		std::string chip = this->config->get("**/chip/name")->get_str();
		if (chip == "gap")
		{
      log.debug("ft223: setting for gap\n");
			return set_bit_value(5, !high) ;
		}
		else
		{
      log.debug("ft223: error cannot set VQPS on this chip\n");
			return false ;
		}
	}
	else
		return false ;
}

int
Ftdi::ft2232_seq_purge(int /* purge_rx */, int /* purge_tx */) {
  int ret = 0;
  unsigned char buf;

  ret = ftdi_usb_purge_buffers(&m_ftdic);
  if (ret < 0) {
    fatal_error("ft2232: ftdi_usb_purge_buffers() failed - %s\n", ftdi_get_error_string(&m_ftdic));
    return -1;
  }

  ret = ftdi_read_data(&m_ftdic, &buf, 1);
  if (ret < 0) {
    fatal_error("ft2232: ftdi_read_data() failed - %s\n", ftdi_get_error_string(&m_ftdic));
    return -1;
  }

  return 0;
}

int
Ftdi::ft2232_seq_reset() {
  if (ftdi_usb_reset(&m_ftdic) < 0) {
    fatal_error("ft2232: ftdi_usb_reset() failed - %s\n", ftdi_get_error_string(&m_ftdic));
    return -1;
  }

  if(ft2232_seq_purge(1, 1) < 0) {
    fatal_error("ft2232: Could not purge\n");
    return -1;
  }

  return 0;
}

int
Ftdi::flush() {
  int xferred;
  int recvd = 0;

  if (m_params.send_buffered == 0)
    return 0;

  if ((xferred = ftdi_write_data(&m_ftdic, (uint8_t*)m_params.send_buf, m_params.send_buffered)) < 0) {
    fatal_error("ft2232: ftdi_write_data() failed - %s (%d)\n", ftdi_get_error_string(&m_ftdic), xferred);
    return -1;
  }

  if ((unsigned int) xferred < m_params.send_buffered) {
    log.warning("Written fewer bytes than requested.\n");
    return -1;
  }

  m_params.send_buffered = 0;

  /* now read all scheduled receive bytes */
  if (m_params.to_recv) {
    if (m_params.recv_write_idx + m_params.to_recv > m_params.recv_buf_len) {
      /* extend receive buffer */
      m_params.recv_buf_len = m_params.recv_write_idx + m_params.to_recv;
      if (m_params.recv_buf)
        m_params.recv_buf = (char*)realloc(m_params.recv_buf, m_params.recv_buf_len);
    }

    assert(m_params.recv_buf != NULL);

    while (recvd == 0) {
      recvd = ftdi_read_data(&m_ftdic, (uint8_t*)&(m_params.recv_buf[m_params.recv_write_idx]), m_params.to_recv);
      if (recvd < 0)
        fatal_error("Error from ftdi_read_data() - %s (%d)\n", ftdi_get_error_string(&m_ftdic), recvd);
    }

    if ((unsigned int) recvd < m_params.to_recv)
      log.warning("Received less bytes than requested.\n");

    m_params.to_recv -= recvd;
    m_params.recv_write_idx += recvd;
  }

  return xferred < 0 ? -1 : xferred;
}

int
Ftdi::ft2232_read(char* buf, int len) {
  int cpy_len;
  int recvd = 0;

  /* flush send buffer to get all scheduled receive bytes */
  if (flush() < 0) {
    log.warning("ft2232: Could not read any data after a flush\n");
    return -1;
  }

  if (len == 0) {
    log.warning("ft2232: Please don't read 0 bits\n");
    return 0;
  }

  /* check for number of remaining bytes in receive buffer */
  cpy_len = m_params.recv_write_idx - m_params.recv_read_idx;

  if (cpy_len > len)
    cpy_len = len;

  len -= cpy_len;

  if (cpy_len > 0) {
    // get data from the receive buffer
    memcpy(buf, &(m_params.recv_buf[m_params.recv_read_idx]), cpy_len);
    m_params.recv_read_idx += cpy_len;

    if (m_params.recv_read_idx == m_params.recv_write_idx)
      m_params.recv_read_idx = m_params.recv_write_idx = 0;
  }

  if (len > 0) {
    /* need to get more data directly from the device */
    while (recvd == 0) {
      recvd = ftdi_read_data(&m_ftdic, (uint8_t*)&(buf[cpy_len]), len);
      if (recvd < 0)
        fatal_error("ft2232: Error from ftdi_read_data() - %s (%d)\n", ftdi_get_error_string(&m_ftdic), recvd);
    }
  }

  return recvd < 0 ? -1 : (cpy_len + len);
}

int
Ftdi::ft2232_write(char *buf, int len, int recv) {
  int xferred = 0;
  // this write function will try to buffer write data
  // buffering will be ceased and a flush triggered in two cases.

  // Case A: max number of scheduled receive bytes will be exceeded
  // with this write
  // Case B: max number of scheduled send bytes has been reached
  if ((m_params.to_recv + recv > FTDI_MAXRECV) || ((m_params.send_buffered > FTDX_MAXSEND) && (m_params.to_recv == 0)))
    xferred = flush();

  if (xferred < 0) {
    log.warning("ft2232: Flush before write failed\n");
    return -1;
  }

  // now buffer this write
  if (m_params.send_buffered + len > m_params.send_buf_len) {
    m_params.send_buf_len = m_params.send_buffered + len;

    if (m_params.send_buf)
      m_params.send_buf = (char*)realloc(m_params.send_buf, m_params.send_buf_len);
  }

  assert(m_params.send_buf);

  memcpy( &(m_params.send_buf[m_params.send_buffered]), buf, len);
  m_params.send_buffered += len;
  if (recv > 0)
    m_params.to_recv += recv;

  if (recv < 0) {
    // immediate write requested, so flush the buffered data
    xferred = flush();
  }

  return xferred < 0 ? -1 : len;
}

bool
Ftdi::ft2232_mpsse_open() {
  unsigned char buf[3];
  int ret;

  // This sequence might seem weird and containing superfluous stuff.
  // However, it's built after the description of JTAG_InitDevice
  // Ref. FTCJTAGPG10.pdf
  // Intermittent problems will occur when certain steps are skipped.

  ret = ft2232_seq_reset();
  if (ret < 0)
    goto fail;

  ret = ft2232_seq_purge(1, 0);
  if (ret < 0)
    goto fail;

  ret = ftdi_write_data_set_chunksize(&m_ftdic, FTDX_MAXSEND_MPSSE);
  if (ret < 0) {
    log.warning("ft2232: Got error %s\n", ftdi_get_error_string(&m_ftdic));
    goto fail;
  }

  ret = ftdi_read_data_set_chunksize(&m_ftdic, FTDX_MAXSEND_MPSSE);
  if (ret < 0) {
    log.warning("ft2232: Got error %s\n", ftdi_get_error_string(&m_ftdic));
    goto fail;
  }

  /* set a reasonable latency timer value
     if this value is too low then the chip will send intermediate result data
     in short packets (suboptimal performance) */
  ret = ftdi_set_latency_timer(&m_ftdic, 1);
  if (ret < 0) {
    log.warning("ft2232: ftdi_set_latency_timer() failed\n");
    goto fail;
  }

  ret = ftdi_set_bitmode(&m_ftdic, 0x00, BITMODE_RESET);
  if (ret < 0) {
    log.warning("ft2232: ftdi_set_bitmode(BITMODE_RESET) failed\n");
    goto fail;
  }

  ret = ftdi_set_bitmode(&m_ftdic, 0x0b, BITMODE_MPSSE);
  if (ret < 0) {
    log.warning("ft2232: ftdi_set_bitmode(BITMODE_MPSSE) failed\n");
    goto fail;
  }

  ret = ftdi_usb_reset(&m_ftdic);
  if (ret < 0) {
    log.warning("ft2232: ftdi_usb_reset() failed\n");
    goto fail;
  }

  ret = ft2232_seq_purge(1, 0);
  if (ret < 0) {
    log.warning("ft2232: Could not purge\n");
    goto fail;
  }

  // set TCK Divisor
  buf[0] = TCK_DIVISOR;
  buf[1] = 0x2;
  buf[2] = 0x0;
  ret = ft2232_write((char *)buf, 3, 0);
  if (ret < 0) {
    log.warning("ft2232: Failed to set TCK divisor\n");
    goto fail;
  }

  // switch off loopback
  buf[0] = LOOPBACK_END;
  ret = ft2232_write((char *)buf, 1, 0);
  if (ret < 0) {
    log.warning("ft2232: Failed to switch off loopback\n");
    goto fail;
  }

  ret = flush();
  if (ret < 0) {
    goto fail;
  }

  ret = ftdi_usb_reset(&m_ftdic);
  if (ret < 0) {
    log.warning("ft2232: ftdi_usb_reset() failed\n");
    goto fail;
  }

  ret = ft2232_seq_purge(1, 0);
  if (ret < 0) {
    log.warning("ft2232: Could not purge\n");
    goto fail;
  }

  usleep(SETUP_DELAY);
  return true;

fail:
  log.warning("ft2232: Opening device in mpsse mode failed\n");

  ftdi_usb_close(&m_ftdic);
  ftdi_deinit(&m_ftdic);

  return false;
}

bool
Ftdi::dev_try_open(unsigned int vid, unsigned int pid, unsigned int index) const
{
  struct ftdi_context ftdic;
  int error;

  ftdi_init(&ftdic);

  error = ftdi_usb_open_desc_index(&ftdic, vid, pid, NULL, NULL, index);
  if (error != 0) {
    ftdi_deinit(&ftdic);
    return false;
  }

  ftdi_usb_close(&ftdic);
  ftdi_deinit(&ftdic);

  return true;
}

bool
Ftdi::bit_out(char outbit, bool last)
{
  return stream_out_internal(&outbit, 1, false, last);
}

bool
Ftdi::bit_inout(char* inbit, char outbit, bool last)
{
  return stream_inout(inbit, &outbit, 1, last);
}

bool
Ftdi::stream_out(char* outstream, unsigned int n_bits, bool last)
{
  return stream_out_internal(outstream, n_bits, false, last);
}

bool
Ftdi::stream_out_internal(char* outstream, unsigned int n_bits, bool postread, bool last)
{
  unsigned int len_bytes;
  unsigned int len_bits;
  unsigned int len_tms_bits;
  char buf;

  len_tms_bits = last ? 1 : 0;
  len_bytes    = (n_bits - len_tms_bits) / 8;
  len_bits     = (n_bits - len_tms_bits) % 8;

  if(len_bytes > 0) {
    if (ft2232_write_bytes(outstream, len_bytes, postread) < 0) {
      fatal_error("ft2232: ftdi_stream_out has failed\n");
      return false;
    }
  }

  if(len_bits > 0) {
    if (ft2232_write_bits(&(outstream[len_bytes]), len_bits, postread, 0) < 0) {
      fatal_error("ft2232: ftdi_stream_out has failed\n");
      return false;
    }
  }

  if(len_tms_bits > 0) {
    buf = outstream[len_bytes] >> len_bits;
    if (ft2232_write_bits(&buf, 1, postread, 1) < 0) {
      fatal_error("ft2232: ftdi_stream_out has failed\n");
      return false;
    }
  }

  return true;
}

bool
Ftdi::stream_in(char* instream, unsigned int n_bits, bool last)
{
  int len_bytes;
  int len_bits;
  int len_tms_bits;

  len_tms_bits = last ? 1 : 0;
  len_bytes    = (n_bits - len_tms_bits) / 8;
  len_bits     = (n_bits - len_tms_bits) % 8;

  if(len_bytes > 0) {
    if (ft2232_read_packed_bits(instream, len_bytes, 8, 0) < 0) {
      fatal_error("ft2232: fdti_stream_in has failed\n");
      return false;
    }
  }

  if(len_bits > 0) {
    if (ft2232_read_packed_bits(instream, 1, len_bits, len_bytes * 8) < 0) {
      fatal_error("ft2232: fdti_stream_in has failed\n");
      return false;
    }
  }

  if(len_tms_bits > 0) {
    if (ft2232_read_packed_bits(instream, 1, 1, (len_bits + (len_bytes * 8))) < 0) {
      fatal_error("ft2232: fdti_stream_in has failed\n");
      return false;
    }
  }

  return true;
}

bool
Ftdi::stream_inout(char* instream, char* outstream, unsigned int n_bits, bool last)
{
  if (outstream)
  {
    if (!stream_out_internal(outstream, n_bits, instream != NULL, last)) {
      log.warning("ft2232: ftdi_stream_inout has failed\n");
      return false;
    }
  }
  else
  {
    int bytes = (n_bits + 7) / 8;
    std::vector<char> buffer(bytes, 0);
    if (!stream_out_internal(&(buffer[0]), n_bits, instream != NULL, last)) {
      log.warning("ft2232: ftdi_stream_inout has failed\n");
      return false;
    }
  }

  if (instream && !stream_in(instream, n_bits, last)) {
    log.warning("ft2232: ftdi_stream_inout has failed\n");
    return false;
  }

  return true;
}

int
Ftdi::ft2232_write_bytes(char *buf, int len, bool postread)
{
  int cur_command_size;
  int max_command_size;
  int cur_chunk_len;
  int recv;
  int xferred;

  if(len == 0)
    return 0;

  recv = 0;
  max_command_size = min(len, 65536)+3;
  std::vector<char> mybuf(max_command_size);

  /// Command OPCODE: write bytes
  mybuf[0] = MPSSE_DO_WRITE | MPSSE_LSB | MPSSE_WRITE_NEG;
  if(postread) // if postread is enabled it will buffer incoming bytes
    mybuf[0] = mybuf[0] | MPSSE_DO_READ;

  // We divide the transmitting stream of bytes in chunks with a maximun length of 65536 bytes each.
  while(len > 0) {
    cur_chunk_len = min(len, 65536);
    len = len - cur_chunk_len;
    cur_command_size = cur_chunk_len + 3;

    /// Low and High bytes of the length field
    mybuf[1] = (unsigned char) ( cur_chunk_len - 1);
    mybuf[2] = (unsigned char) ((cur_chunk_len - 1) >> 8);

    /// The rest of the command is filled with the bytes that will be transferred
    memcpy(&(mybuf[3]), buf, cur_chunk_len );
    buf = buf + cur_chunk_len;

    /// Finally we can transmit the command
    xferred = ft2232_write(&mybuf[0], cur_command_size, (postread ? cur_chunk_len : 0) );
    if(xferred != cur_command_size) {
      fatal_error("ft2232: could not transmit command\n");
      return -1;
    }

    // If OK, the update the number of incoming bytes that are being buffered for a posterior read
    if(postread)
      recv = recv + cur_chunk_len;
  }

  // Returns the number of buffered incoming bytes
  return recv;
}

int
Ftdi::ft2232_write_bits(char *buf, int len, bool postread, bool with_tms)
{
  int max_command_size;
  int max_chunk_len;
  int cur_chunk_len;
  int recv;
  int xferred;
  int i;
  char mybuf[3];

  if(len == 0)
    return 0;

  max_command_size = 3;

  if(!with_tms) {
    /// Command OPCODE: write bits (can write up to 8 bits in a single command)
    max_chunk_len = 8;
    mybuf[0] = MPSSE_DO_WRITE | MPSSE_LSB | MPSSE_WRITE_NEG | MPSSE_BITMODE;
  }
  else {
    /// Command OPCODE: 0x4B write bit with tms (can write up to 1 bits in a single command)
    max_chunk_len = 1;
    mybuf[0] = MPSSE_WRITE_TMS|MPSSE_LSB|MPSSE_BITMODE|MPSSE_WRITE_NEG;
  }

  if(postread) // (OPCODE += 0x20) if postread is enabled it will buffer incoming bits
    mybuf[0] = mybuf[0] | MPSSE_DO_READ;

  // We divide the transmitting stream of bytes in chunks with a maximun length of max_chunk_len bits each.
  i = 0;
  recv = 0;
  while(len > 0) {
    cur_chunk_len = min(len, max_chunk_len);
    len = len - cur_chunk_len;

    // Bit length field
    mybuf[1] = (char) (cur_chunk_len - 1);


    if(!with_tms) {
      /// The last byte of the command is filled with the bits that will be transferred
      mybuf[2] = buf[i/8];
      i += 8;
    }
    else {
      mybuf[2] = 0x01 | ((buf[(i/8)] >> (i%8)) << 7);
      i++;
    }

    // Finally we can transmit the command
    xferred = ft2232_write(mybuf, max_command_size, (postread ? 1 : 0) );
    if(xferred != max_command_size) {
      fatal_error("ft2232: ftdi write has failed\n");
      return -1;
    }

    // If OK, the update the number of incoming bytes that are being buffered for a posterior read
    if(postread)
      recv = recv + 1;
  }

  if (flush() < 0)
    return -1;

  return recv;
}

int
Ftdi::ft2232_read_packed_bits(char *buf, int packet_len, int bits_per_packet, int offset)
{
  unsigned char dst_mask;
  unsigned char src_mask;
  int row_offset;
  int dst_row;
  int dst_col;
  int src_row;
  int src_col;
  int i;

  if(packet_len == 0 || bits_per_packet == 0)
    return 0;

  if (offset == 0 && bits_per_packet == 8)
  {
    if(ft2232_read(buf, packet_len) < 0) {
      fatal_error("Read failed\n");
      return -1;
    }
  }
  else
  {
    std::vector<char> mybuf(packet_len);

    if(ft2232_read(&mybuf[0], packet_len) < 0) {
      fatal_error("Read failed\n");
      return -1;
    }

    if(bits_per_packet < 8) {
      for(i=0; i < packet_len; i++) { // rotate bits to the left side
        mybuf[i] = (mybuf[i] >> (8-bits_per_packet));
      }

      for(i = offset; i < (packet_len*bits_per_packet+offset); i++) {
        dst_row = i / 8;
        dst_col = i % 8;
        src_row = (i-offset) / bits_per_packet;
        src_col = (i-offset) % bits_per_packet;
        dst_mask = ~(1 << dst_col);
        src_mask = (1 << src_col);
        buf[dst_row] = (buf[dst_row] & dst_mask) | ((mybuf[src_row] & src_mask) >> (dst_col - src_col));
      }
    } else if(bits_per_packet == 8) {
      row_offset = offset / 8;
      memcpy( &(buf[row_offset]), &mybuf[0], packet_len);
    } else {
      return -1;
    }
  }

  return 0;
}

bool
Ftdi::set_bit_value(int bit, int value)
{
  unsigned char buf[4];
  int cmd_len = 0;
  bits_value = (bits_value & ~(1<<bit)) | (value << bit);
  if (bit >= 8)
  {
    SET_DATA_BITS_HIGH_BYTE(buf, cmd_len, bits_value>>8, bits_direction>>8);
    SET_DATA_BUFFER(buf, cmd_len, SEND_IMMEDIATE);
  }
  else
  {
    SET_DATA_BITS_LOW_BYTE(buf, cmd_len, bits_value, bits_direction);
    SET_DATA_BUFFER(buf, cmd_len, SEND_IMMEDIATE);
  }

  if (ft2232_write((char *)buf, 4, 0) != 4) return false;
  flush();
  return true;
}

bool
Ftdi::set_bit_direction(int bit, int isout)
{
  bits_direction = (bits_direction & ~(1<<bit)) | (isout << bit);
  return true;
}

bool
Ftdi::jtag_reset(bool active)
{
  if (m_id == Olimex) {
    std::string chip = this->config->get("**/chip/name")->get_str();

    if (chip == "vivosoc2" || chip == "vivosoc2_1")
    {
      if (active)
      {
        unsigned char buf[256];
        int cmd_len = 0;
        SET_DATA_BITS_HIGH_BYTE(buf, cmd_len, ~0x01, 0x3);
        SET_DATA_BITS_HIGH_BYTE(buf, cmd_len, ~0x02, 0x3);
        SET_DATA_BUFFER(buf, cmd_len, SEND_IMMEDIATE);

        if(ft2232_write((char *)buf, cmd_len, 0) != cmd_len) return false;
      }
      return true;
    }    
    else if(chip == "vivosoc3")
    {
      bool result = set_bit_value(9, active);
      return result;
    }
    else
    {
      bool result = set_bit_value(8, !active);
      return result;
    }
  } else if (m_id == Digilent) {
    bool result = set_bit_value(6, !active);
    return result;
  } else {
    // not supported
    return false;
  }
}

