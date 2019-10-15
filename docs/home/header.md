
# Introduction of the GAP8 IoT application processor

GAP8 is a RISC-V and PULP (Parallel Ultra-Low-Power Processing Platform) open-source platform based IoT application processor. It enables cost-effective development, deployment and autonomous operation of intelligent devices that capture, analyze, classify and act on the fusion of rich data sources such as images, sounds or vibrations. In particular, GAP8 is uniquely optimized to execute a large spectrum of image and audio algorithms including convolutional neural network inference with extreme energy efficiency. This allows industrial and consumer product manufacturers to integrate artificial intelligence and advanced classification into new classes of wireless edge devices for IoT applications including image recognition, counting people and objects, machine health monitoring, home security, speech recognition, consumer robotics and smart toys.

## GAP8 Micro-architecture

GAP8's hierarchical, demand-driven architecture enables ultra-low-power operation by combining:

+ A series of highly autonomous smart I/O peripherals for connection to cameras, microphones and other capture and control devices.
+ A fabric controller core for control, communications and security functions.
+ A cluster of 8 cores with an architecture optimized for the execution of vectorized and parallelized algorithms combined with a specialized Convolutional Neutral Network accelerator (HWCE).

All cores and peripherals are power switchable and voltage and frequency adjustable on demand. DC/DC regulators and clock generators with ultra fast reconfiguration times are integrated. This allows GAP8 to adapt extremely quickly to the processing/energy requirements of a running application. All elements share access to a L2 memory area. The cluster cores and HWCE share access to a L1 memory area and instruction cache. Multiple DMA units allow autonomous, fast, low power transfers between memory areas. A memory protection unit is included to allow secured execution of applications on GAP8.

All 9 cores share the same extended RISC-V instruction set architecture. The I (integer), C (compressed instruction), M (Multiplication and division) extensions and a portion of the supervisor ISA subsets are supported. These are extended with specific instructions to optimize the algorithms that GAP8 is targeted at. These extensions include zero overhead hardware loops, pointer post/pre modified memory accesses, instructions mixing control flow with computation (min, max, etc), multiply/subtract and accumulate, vector operations, fixed point operations, bit manipulation and dot product. All of these instruction extensions are optimized by the compiler or can be used 'by hand'.

## GAP8 Features

+ 1+8 high performance extended RISC-V ISA based cores.
	+ 1 - A high performance micro-controller
	+ 8 - 8 cores that execute in parallel for compute intensive tasks
+ A hardware Convolution Engine (HWCE) for Convolutional Neural Networks based applications.
+ A level 2 Memory (512KB) for all the cores
+ A level 1 Memory (64 KB) shared by all the cores in Cluster
+ A level 1 memory (8  KB) owned by FC
+ A smart, lightweight and completely autonomous DMA (micro-DMA) capable of handling complex I/O schemes.
+ A multi-channel 1D/2D cluster-DMA controls the transactions between the L2 Memory and L1 Memory.
+ A rich set of peripheral interface
+ 2 programmable clocks
+ Memory Protection Unit

![GAP8 Layout](./images/GAP8Layout.png){ width=480px }

As shown in the GAP8 block diagram above, GAP8 has a rich set of I/O interfaces, which includes:

 Interface | Number | Description
--- | --- | ---
 LVDS      | 1 | A 128 Mb/s interface for RF
 ORCA	   | 1 | A low debit interface for RF.
 I2C	   | 2 | Standard I2C interfaces.
 I2S	   | 2 | Standard I2S interface for connecting digital audio devices.
 CPI 	   | 1 | A parallel interface for connecting camera
 HyperBus  | 1 | A high speed memory bus interface
 SPI-M     | 2 | A quad SPI-M and an additional SPI-M
 SPI-S     | 1 | A SPI Slave
 UART      | 1 | A standard UART interface
 GPIOs     | 32| General Purpose Input Output

Other peripherals:

 Peripherals | Number | Description
|---|:---:|---|
 RTC 		 | 1      | 32K real-time clock
 PWM 		 | 4      | PWMs, 12 Output channels


## The Fabric Controller and The Cluster

As mentioned in previous section, GAP8 has 8+1 high performance cores, which play two different roles. The cluster contains 8 cores that can execute in parallel, and provide high performance calculation for image processing, audio processing or signal modulation, etc. The single core, referred to as the “Fabric Controller” or "FC", is used as micro-controller. It is in charge of controlling all the operations of GAP8, like the micro-DMA to capture 1 image from the CPI interface, starting up the cluster and dispatching a job to it, etc. You can think of the cluster as a 'peripheral' of the FC.

Cores are identified using two identification numbers. The cluster ID which identifies the group of cores that the core belongs to and the core ID which identifies the core in that group. The IDs can be used to start a particular task on a core.

ENTITY | CLUSTER ID | CORE ID
|---|:---:|:---:|
CORE0  |    0x00    |   0x00
CORE1  |    0x00    |   0x01
CORE2  |    0x00    |   0x02
CORE3  |    0x00    |   0x03
CORE4  |    0x00    |   0x04
CORE5  |    0x00    |   0x05
CORE6  |    0x00    |   0x06
CORE7  |    0x00    |   0x07
FC     |    0x20    |   0x00

The table above shows:

 + Core 0-7: which have the same cluster ID, but have different core ID.
 + FC: which has the same core ID as the cluster core 0, but its cluster ID is equal to 0x20.

By default, the cluster is powered down and cannot be used. It must first be powered-up by the FC. Once the cluster is awake, its core 0 plays the role of "master". Core 0 is in charge of the following jobs:

 + The communication with the Fabric Controller. For example, getting a task from the FC, sending a data request to the FC, getting synchronized with the FC, etc.
 + Dispatching tasks/applications to other cores.

Before receiving a task from the core 0, the rest of the cores stay at a dispatch barrier which clock gates them (i.e. they are stopped and use only a low leakage current).

When a task/application is finished on the cluster, it should be shut down to save power. IT IS IMPORTANT TO MOVE DATA IN THE SHARED L1 MEMORY TO THE L2 MEMORY BEFORE YOU SHUTDOWN THE CLUSTER.

## Memory areas

There are 2 different levels of memory internal to GAP8. A larger level 2 area of 512KB which is accessible by all processors and DMA units and two smaller level 1 areas, one for the FC (16KB) and one shared by all the cluster cores (64KB). The shared level 1 memory is banked and cluster cores can usually access their bank in a single cycle. GAP8 can also access external memory areas over the HyperBus (Flash or RAM) or quad-SPI (Flash) peripherals. We refer to RAM accessed over the HyperBus interface as level 3 memory. Since the energy cost and performance cost of accessing external RAM over the HyperBus is very high compared to the internal memory generally this should be avoided as much as possible. Code is generally located in the L2 memory area. The instruction caches of the FC (4KB) and cluster (16KB) will automatically cache instructions as needed. The cluster instruction cache is shared between all the cores in the cluster. Generally the cluster cores will be executing the same area of code on different data so the shared cluster instruction cache exploits this to reduce memory accesses for loading instructions.

## Micro-DMA and cluster-DMA

To reduce power consumption GAP8 does not include data caches in its memory hierarchy. Instead GAP8 uses autonomous DMA units that can be used to transfer data to and from peripherals and in between internal memory areas.

Good management of memory is absolutely crucial to extracting the most energy efficiency from GAP8. GreenWaves supplies a tool, the GAP8 auto-tiler, which can significantly aid in managing memory transfers between the different memory areas.

The micro-DMA unit is used to transfer data to and from peripherals including level 3 memory. At the end of a transaction the FC can be woken up to queue a new task. To allow the micro-DMA to continue working at the end of a transaction up to 2 transfers can be queued for each peripheral. The micro-DMA schedules active transfers based on signals from the peripherals in a round-robin fashion. Generally the micro-DMA is not used directly by a programmer. It is used by the drivers for each of the peripherals.

As the micro-DMA, the cluster-DMA is a smart, lightweight and completely autonomous unit. It is used to transfer data between the L2 and L1 memory areas. It supports both 1D and 2D transfers and can queue up to 16 requests. The commands for the cluster-DMA unit are extremely short which minimizes SW overhead and avoids instruction cache pollution.
