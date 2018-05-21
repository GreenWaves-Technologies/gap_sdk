# Introduction

Arm® Mbed™ OS is an open source embedded operating system designed specifically for the "things" in the Internet of Things developed by Arm for the Cortex-M series of micro-controllers. It includes all the features you need to develop a connected product including security, connectivity, an RTOS and drivers for sensors and I/O devices.

GreenWaves has ported Mbed OS to the RISC-V based GAP8 IoT Application Processor.

# Porting Arm® Mbed™ OS to GAP8

Arm® Mbed™ OS is based on Cortex Micro-controller Software Interface Standard (CMSIS) which provides a ground-up software framework for embedded applications that run on Cortex-M based micro-controllers. CMSIS was started in 2008 and the initiative is in close cooperation with various silicon and software vendors. CMSIS enables consistent and simple software interfaces to the processor and the peripherals, simplifying software reuse and reducing the learning curve for micro-controller developers.

GreenWaves Technologies has ported Arm® Mbed™ OS to help developers familiar with CMSIS shorten the time spent developing and deploying applications onto GAP8. Included in the port are the CMSIS-HAL, CMSIS-Driver and CMSIS-RTOS API sets. The GAP8 port is released under the same open source license as Arm® Mbed™ OS. The included GAP8 CMSIS implementation can also be used as a basis for ports of other RTOS's to GAP8.

# Introduction
 Here is the global software struct of the Arm® Mbed™ OS, from now we provide developpers all resources except communication interfaces (under developing and testing).
![Mbed OS stack](images/mbed_os_stack.png){ width=500px }

The mbed library provides abstractions for the microcontroller (MCU) hardware (in particular drivers for the MCU peripherals) and it is divided in the following [software layers and APIs](https://os.mbed.com/handbook/mbed-library-internals):
![Mbed layer](images/mbed_layers.png){ width=400px }
To port the mbed library to a GAP8, we provide the two software layers marked as "MCU dependent" and one basic software layer for RISC-V 32-bit based "MCU Registers" in the above diagram. So if users are also interested in othe RTOS like freeRTOS, you can reuse the three lowest layers to your own project. In addiction, you can even reuse the "MCU Registers" for other RISC-V 32-bit based MCU like (SIFIVE E310).

## Porting details
###[Memory model](https://docs.mbed.com/docs/mbed-os-handbook/en/5.1/concepts/memory_model/)
As we can see in the gap8 architectrue below, it has three main memories:
 - Shared L1 TCDM (right side in the CLUSTER domain) with one cycles access time, start address is 0x10000000, 64KB;
 - Shared L2 RAM (left side above in the SoC domain) with several cycles access time, start address is 0x1C000000, 512KB;
 - FC L1 TCDM (left side below in the SoC domain) with one cycles access time, start address is 0x1B000000, 16KB.
![GAP8 architecture](images/gap8_archi.png){ width=500px }

To use the heap of these memory space:
| Memory Type | Allocator | De-Allocator
|------------- |---------- |-------
| L1 TCDM | L1_Malloc() | L1_MallocFree()
| FC TCDM | FC_Malloc() | FC_MallocFree()
| L2 RAM | malloc() | free()

So how to use these memory resources in Arm® Mbed™ OS to create an efficient software system is our objective, here we give a suggestion for each thread stack configuration for OS.

| STACK | Memory Type
|------------------ |-----------------
| L1_each_core_stack | L1 TCDM
| OS_stack | FC TCDM
| Main_thread_stack | FC TCDM
| Idle_thread_stack | L2 RAM
| Timer_thread_stack | L2 RAM
| APP_thread_stack | L2 RAM

# Drivers
## Drivers support situation for GAP8 (1st release)
| Driver type | CMSIS_Driver | Mbed API (C) | Mbed API (C++) | Example
|------------ |------------- |------- |-------- |---------------
| LVDS | NO | - | - | NO
| ORCA | NO | - | - | NO
| SPIM | YES | YES | YES | YES
| HYPERBUS | YES | YES | YES | YES
| UART | YES | YES | YES | YES
| I2C | YES | YES | YES | YES
| TCDM | NO | - | - | NO
| I2S | YES | - | - | YES
| CPI | YES | - | - | YES
| RTC | YES | YES | YES | YES
| SPIS | YES | - | - | YES


## Driver APIs' Differences
In GAP8, all the external peripherals are controlled by a unit we call the micro-DMA (UDMA). This means that all transmissions are asynchronous and explicit. For example, the classic loop waiting for incoming characters from a UART cannot be used in an application running on GAP8. This causes some changes in the standard Mbed OS APIs which need to be noted.

1. SPI C, C++ API

Arm® Mbed™ OS standard C API :

```
/** Write a byte out in master mode and receive a value
 *
 * @param[in] obj   The SPI peripheral to use for sending
 * @param[in] value The value to send
 * @return Returns the value received during send
 */
int  spi_master_write(spi_t *obj, int value);
```

GAP8 version, separated into two functions :

```
/** Write a byte out in master mode
 *
 * @param[in] obj   The SPI peripheral to use for sending
 * @param[in] value The value to send
 * @return Returns status
 */
int  spi_master_write(spi_t *obj, int value);
```
```
/** Read a byte in master mode
 *
 * @param[in] obj   The SPI peripheral to use for sending
 * @param[in] cmd   The SPI send command to read
 * @return Returns the value received
 */
int spi_master_read(spi_t *obj, int cmd);
```

Arm® Mbed™ OS standard C++ API :

```
/** Write to the SPI Slave and return the response
 *
 *  @param value Data to be sent to the SPI slave
 *
 *  @returns
 *    Response from the SPI slave
*/
virtual int write(int value);
```

GAP8 version, separated into two functions :

```
/** Write to the SPI Slave
 *
 *  @param value Data to be sent to the SPI slave
 *
 *  @returns
 *    Response from the SPI slave
 */
virtual int write(int value);
```
```
/** Read response
 *
 *  Here we use explicit transfer, so just write something to SPI slave
 *  without return. But read means write a command to read a response.
 *
 *  @param value Data to be sent to the SPI slave
 *
 *  @returns
 *    Response from the SPI slave
 */
virtual int read(int value);
```

GAP8's SPI master 0 supports Quad-SPI mode, so we have added some extension APIs to support QSPI.

For some devices where you need polling status, GAP8 SPI and QSPI interfaces also provide an auto polling mechanism.

```
/** Specify I/O width of SPI transaction (Quad SPI or not)
 *
 * @param[in] obj   The SPI peripheral to use for sending
 * @param[in] qpi   Choose Quad SPI or normal SPI
 */
int spi_master_qspi(spi_t *obj, spi_qpi_t qpi);
```
```
/** SPI auto polling
 *
 * @param[in] obj   The SPI peripheral to use for sending
 * @param[in] conf  The configuration of auto polling
 */
int spi_master_auto_polling(spi_t *obj, spi_polling_config_t *conf);
```
```
/** SPI blocking sequence transaction
 *
 * This function will create udma control sequence according to sequence configuration data
 * And then do blocking transaction read or write
 *
 * @param[in] obj   The SPI peripheral to use for sending
 * @param[in] seq   The command sequence configuration data
 */
int spi_master_transfer_command_sequence(spi_t *obj, spi_command_sequence_t* seq);
```

2. HYPERBUS C, C++ API

Cypress HyperBus Memory is a portfolio of high-speed, low-pin-count memory products that uses our HyperBus interface technology.  The HyperBus interface draws upon the legacy features of both parallel and serial interface memories, while enhancing system performance, ease of design, and system cost reduction.  The 12-pin, HyperBus interface operates at Double Data Rate (DDR) and can scale up to 333 MB/s throughput making it an ideal solution for automotive, industrial and IoT applications that require “instant-on” capability.

GAP8 uses HyperBus to support external flash and RAM memory. We have added new C and C++ APIs to allow use of HyperBus in Arm® Mbed™ OS in the /hal and /driver directories.

3. For all other APIs and more informations about Arm® Mbed™ OS  - Please refer to the Mbed documentation at https://www.mbed.com

# Running an Arm® Mbed™ OS application on GAP8

## TEST Support
In directory ./gap_sdk/examples/mbed-examples, you can find various tests :
| Test type | Description
|------------ |-------------
| test_os | Arm® Mbed™ OS C APIs tests
| test_driver | Arm® Mbed™ OS C Driver tests
| test_os_c++ | Arm® Mbed™ OS C++ Rtos tests
| test_driver_c++ | Arm® Mbed™ OS C++ Driver tests
| test_event | Arm® Mbed™ OS Event Queue C++ tests
| test_features | Arm® Mbed™ OS Features C or C++ tests
| test_application | Arm® Mbed™ OS GAP8 Apllications tests
| test_autotiler | Arm® Mbed™ OS GAP8 Autotiler (CNN tools) tests

## Two Methods to compile and run your tests
### Use Makefile
Change directory to an example and run as for mbed-os examples.

````shell
cd ./gap_sdk/examples/mbed-examples/test_features/test_Cluster_HelloWorld
make clean all run
````

After compilation and application load to your GAPUINO board by JTAG, Here is the result:

````
Fabric controller code execution for mbed_os Cluster Power On test
Hello World from cluster core 0!
Hello World from cluster core 6!
Hello World from cluster core 1!
Hello World from cluster core 4!
Hello World from cluster core 2!
Hello World from cluster core 7!
Hello World from cluster core 5!
Hello World from cluster core 3!
Test success
Detected end of application, exiting with status: 0
````

###Use [Mbed CLI](https://github.com/ARMmbed/mbed-cli)
Now, we only support compilation.


# Trademark

Arm® and Arm® Mbed™ OS are registered trademarks or trademarks of Arm Limited (or its subsidiaries) in the USand/or elsewhere.