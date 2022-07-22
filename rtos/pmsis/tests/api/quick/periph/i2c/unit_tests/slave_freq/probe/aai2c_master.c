/*=========================================================================
| (c) 2003-2007  Total Phase, Inc.
|--------------------------------------------------------------------------
| Project : Aardvark Sample Code
| File    : aai2c_eeprom.c
|--------------------------------------------------------------------------
| Perform simple read and write operations to an I2C EEPROM device.
|--------------------------------------------------------------------------
| Redistribution and use of this file in source and binary forms, with
| or without modification, are permitted.
|
| THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
| "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
| LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
| FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
| COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
| INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
| BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
| LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
| CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
| LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
| ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
| POSSIBILITY OF SUCH DAMAGE.
 ========================================================================*/

//=========================================================================
// INCLUDES
//=========================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aardvark.h"


//=========================================================================
// CONSTANTS
//=========================================================================
#define PAGE_SIZE   8
#define BUS_TIMEOUT 150  // ms

#define DATA_SIZE 8

#define ERROR_CHECK 0
#define WRITE_DATA 0

//=========================================================================
// STATICS
//=========================================================================

static u08 data[DATA_SIZE] = {0};

//=========================================================================
// FUNCTION
//=========================================================================

static void generate_data(void)
{
    for (int i = 0; i < DATA_SIZE; i++)
    {
        data[i] = (data[i] + i) % 255;
    }
}

static void print_status_code(int status)
{
    switch(status)
    {
        case AA_I2C_STATUS_OK:
            printf("Status: OK\n");
            break;
        case AA_I2C_STATUS_BUS_ERROR:
            printf("Status: BUS ERROR\n");
            break;
        case AA_I2C_STATUS_SLA_ACK:
            printf("Status: SLA_ACK\n");
            break;
        case AA_I2C_STATUS_SLA_NACK:
            printf("Status: SLA_NACK\n");
            break;
        case AA_I2C_STATUS_DATA_NACK:
            printf("Status: DATA_NACK\n");
            break;
        case AA_I2C_STATUS_ARB_LOST:
            printf("Status: ARB_LOST\n");
            break;
        case AA_I2C_STATUS_BUS_LOCKED:
            printf("Status: BUS_LOCKED\n");
            break;
        case AA_I2C_STATUS_LAST_DATA_ACK:
            printf("Status: LAST_DATA_ACK\n");
            break;
        default:
            printf("Status: ERROR - Unknown status %d\n", status);
            break;
    }
}


static int _writeReadMemory (Aardvark handle, u08 device, u08 addr, u08* data)
{
    u08 data_in[DATA_SIZE];
    u08 addr_read[2] = { 3, 0 };
    u16 count = 0;
    u16 count_in = 0;
    int status = 0;
    int nb_errors = 0;

#if WRITE_DATA
    u08 data_out[128];
    data_out[0] = 4; // LSB addr
    data_out[1] = 0; // MSB addr
    for (int i = 2; i<DATA_SIZE + 2; i++)
    {
        data_out[i] = data[i-2];
    }

    status = aa_i2c_write_ext(handle, device, AA_I2C_NO_FLAGS, DATA_SIZE + 2, data_out, &count);
    print_status_code(status);
    if (count != DATA_SIZE + 2)
    {
        printf("ERROR: Expected to write %d bytes, instead wrote %d\n", DATA_SIZE + 2, count);
    }
#endif

    // Write the address
    //aa_i2c_write(handle, device, AA_I2C_NO_STOP, 1, &addr);

    // Read from EEPROM current address
    status = aa_i2c_write_read(handle, device, AA_I2C_NO_FLAGS, 2, addr_read, &count, DATA_SIZE, data_in, &count_in);
    print_status_code(status);
    if (count_in < 0) {
        printf("error: %s\n", aa_status_string(count_in));
        return 1;
    }
    if (count_in == 0) {
        printf("error: no bytes read\n");
        printf("  are you sure you have the right slave address?\n");
        return 1;
    }
    else if (count_in != DATA_SIZE) {
        printf("error: read %d bytes (expected %d)\n", count_in, DATA_SIZE);
        return 1;
    }

    // Dump the data to the screen
    printf("\nData read from device:");
    for (int i = 0; i < count_in; ++i) {
        if ((i&0x0f) == 0)      printf("\n%04x:  ", addr+i);
        printf("%02x ", data_in[i] & 0xff);
#if ERROR_CHECK
        if (data_in[i] != data[i])
        {
            printf("ERROR: expected %2x, got %2x\n", data[i], data_in[i]);
            nb_errors++;
        }
#endif
        if (((i+1)&0x07) == 0)  printf(" ");
    }
    printf("\n");
    return nb_errors;
}


//=========================================================================
// MAIN PROGRAM
//=========================================================================
int main (int argc, char *argv[]) {
    Aardvark handle;
    int port    = 0;
    int bitrate = 100;
    u08 device;
    u08 addr;
    int bus_timeout;

    if (argc < 7) {
        printf("usage: aai2c_eeprom PORT BITRATE read  SLAVE_ADDR OFFSET LENGTH\n");
        printf("usage: aai2c_eeprom PORT BITRATE write SLAVE_ADDR OFFSET LENGTH\n");
        printf("usage: aai2c_eeprom PORT BITRATE zero  SLAVE_ADDR OFFSET LENGTH\n");
        return 1;
    }

    port    = atoi(argv[1]);
    bitrate = atoi(argv[2]);
    device  = (u08)strtol(argv[4], 0, 0);
    addr    = (u08)strtol(argv[5], 0, 0);

    // Verify that device is available
    u16 devices[port+1];

    aa_find_devices(port+1,devices);
    if (devices[port] != port)
    {
        printf("ERROR: device (port %d) is already in use\n", port);
        return 1;
    }

    // Open the device
    handle = aa_open(port);
    if (handle <= 0) {
        printf("Unable to open Aardvark device on port %d\n", port);
        printf("Error code = %d\n", handle);
        return 1;
    }

    // Ensure that the I2C subsystem is enabled
    aa_configure(handle,  AA_CONFIG_SPI_I2C);

    // Enable the I2C bus pullup resistors (2.2k resistors).
    // This command is only effective on v2.0 hardware or greater.
    // The pullup resistors on the v1.02 hardware are enabled by default.
    aa_i2c_pullup(handle, AA_I2C_PULLUP_BOTH);

    // Power the EEPROM using the Aardvark adapter's power supply.
    // This command is only effective on v2.0 hardware or greater.
    // The power pins on the v1.02 hardware are not enabled by default.
    aa_target_power(handle, AA_TARGET_POWER_BOTH);

    // Set the bitrate
    bitrate = aa_i2c_bitrate(handle, bitrate);
    printf("Bitrate set to %d kHz\n", bitrate);

    // Set the bus lock timeout
    bus_timeout = aa_i2c_bus_timeout(handle, BUS_TIMEOUT);
    //bus_timeout = aa_i2c_bus_timeout(handle, 1000);
    printf("Bus lock timeout set to %d ms\n", bus_timeout);

    //BURST
    //TODO also mix aa_i2c_write_read instead
    int nb_errors = 0;
    for (int i = 0; i < 10000; i++)
    {
        printf("try #%d\n",i);
        generate_data();
        // pass data as argument to write
        nb_errors += _writeReadMemory(handle, device, addr, data);
        //aa_sleep_ms(20);
    }
    printf("Total of errors: %d\n",nb_errors);

    // Close the device and exit
    aa_close(handle);
    return 0;
}
