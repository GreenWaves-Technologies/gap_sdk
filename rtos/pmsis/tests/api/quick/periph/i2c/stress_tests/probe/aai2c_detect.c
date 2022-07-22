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
#define BUS_TIMEOUT 150  // ms

//=========================================================================
// STATICS
//=========================================================================

//=========================================================================
// FUNCTION
//=========================================================================

static void print_status_code(int status)
{
    (void) status;
#if 0
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
#endif
}


static void _scanAddr(Aardvark handle, u08 addr)
{
    u08 data_out[1] = {0};
    u16 count = 0;
    int status = 0;

    status = aa_i2c_write_ext(handle, addr, AA_I2C_NO_FLAGS, 1, data_out, &count);
    print_status_code(status);
    if (count == 1)
    {
        printf("Found slave at address 0x%X\n", addr);
    }
}


//=========================================================================
// MAIN PROGRAM
//=========================================================================
int main (int argc, char *argv[]) {
    Aardvark handle;
    int port    = 0;
    int bitrate = 100;
    int bus_timeout;

    if (argc < 3) {
        printf("usage: aai2c_detect PORT BITRATE\n");
        printf("usage: aai2c_detect PORT BITRATE\n");
        printf("usage: aai2c_detect PORT BITRATE\n");
        return 1;
    }

    port    = atoi(argv[1]);
    bitrate = atoi(argv[2]);

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

    for (int i = 0; i < 128; i++)
    {
        _scanAddr(handle, i);
    }

    // Close the device and exit
    aa_close(handle);
    return 0;
}
