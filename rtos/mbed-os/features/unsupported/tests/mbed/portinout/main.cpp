#include "test_env.h"

#if !DEVICE_PORTINOUT
  #error [NOT_SUPPORTED] PortInOut is not supported
#endif

#if defined(TARGET_K64F)
#define P1_1    (1 << 16)
#define P1_2    (1 << 17)
#define PORT_1  PortC

#define P2_1    (1 << 2)
#define P2_2    (1 << 3)
#define PORT_2  PortC

#elif defined(TARGET_K66F)
#define P1_1    (1 << 3)
#define P1_2    (1 << 4)
#define PORT_1  PortC

#define P2_1    (1 << 5)
#define P2_2    (1 << 2)
#define PORT_2  PortC

#elif defined(TARGET_LPC11U24)
#define P1_1    (1 <<  9) // p0.9
#define P1_2    (1 <<  8) // p0.8
#define PORT_1  Port0

#define P2_1    (1 << 24) // p1.24
#define P2_2    (1 << 25) // p1.25
#define PORT_2  Port1

#elif defined(TARGET_LPC1768) || defined(TARGET_LPC2368)
#define P1_1    (1 << 9)  // p0.9  -> p5
#define P1_2    (1 << 8)  // p0.8  -> p6
#define PORT_1  Port0

#define P2_1    (1 << 1)  // p2.1 -> p25
#define P2_2    (1 << 0)  // p2.0 -> p26
#define PORT_2  Port2

#elif defined(TARGET_LPC4088)
#define P1_1    (1 << 7)  // p0.7  -> p13
#define P1_2    (1 << 6)  // p0.6  -> p14
#define PORT_1  Port0

#define P2_1    (1 << 2)  // p1.2 -> p30
#define P2_2    (1 << 3)  // p1.3 -> p29
#define PORT_2  Port1

#elif defined(TARGET_LPC1114)
#define P1_1    (1 <<  9) // p0.9
#define P1_2    (1 <<  8) // p0.8
#define PORT_1  Port0

#define P2_1    (1 << 1) // p1.1
#define P2_2    (1 << 0) // p1.0
#define PORT_2  Port1

#elif defined(TARGET_KL25Z)
#define P1_1    (1 << 4)  // PTA4
#define P1_2    (1 << 5)  // PTA5
#define PORT_1  PortA

#define P2_1    (1 << 5)  // PTC5
#define P2_2    (1 << 6)  // PTC6
#define PORT_2  PortC

#elif defined(TARGET_nRF51822)
#define P1_1    (1 << 4)  // p4
#define P1_2    (1 << 5)  // p5
#define PORT_1  Port0

#define P2_1    (1 << 24)  // p24
#define P2_2    (1 << 25)  // p25
#define PORT_2  Port0

#elif defined(TARGET_MAXWSNENV)
#define P1_1    (1 << 0)
#define P1_2    (1 << 1)
#define PORT_1  Port0

#define P2_1    (1 << 0)
#define P2_2    (1 << 1)
#define PORT_2  Port1

#elif defined(TARGET_MAX32600MBED)
#define P1_1    (1 << 0)  // P1_0
#define P1_2    (1 << 1)  // P1_1
#define PORT_1  Port1

#define P2_1    (1 << 7)  // P4_7
#define P2_2    (1 << 6)  // P4_6
#define PORT_2  Port4

#elif defined(TARGET_NUCLEO_F030R8) || \
      defined(TARGET_NUCLEO_F070RB) || \
      defined(TARGET_NUCLEO_F072RB) || \
      defined(TARGET_NUCLEO_F091RC) || \
      defined(TARGET_NUCLEO_F103RB) || \
      defined(TARGET_NUCLEO_F302R8) || \
      defined(TARGET_NUCLEO_F303RE) || \
      defined(TARGET_NUCLEO_F334R8) || \
      defined(TARGET_NUCLEO_F401RE) || \
      defined(TARGET_NUCLEO_F410RB) || \
      defined(TARGET_NUCLEO_F411RE) || \
      defined(TARGET_NUCLEO_L053R8) || \
      defined(TARGET_NUCLEO_L073RZ) || \
      defined(TARGET_NUCLEO_L476RG) || \
      defined(TARGET_NUCLEO_F446RE) || \
      defined(TARGET_NUCLEO_L152RE)
#define P1_1    (1 << 3)  // PB_3  (D3)
#define P1_2    (1 << 10)  // PB_10 (D6)
#define PORT_1  PortB

#define P2_1    (1 << 10)  // PA_10 (D2)
#define P2_2    (1 << 8)  // PA_8  (D7)
#define PORT_2  PortA

#elif defined(TARGET_NUCLEO_F767ZI) || \
      defined(TARGET_NUCLEO_F303ZE) || \
      defined(TARGET_NUCLEO_F207ZG) || \
      defined(TARGET_NUCLEO_F746ZG)
#define P1_1    (1 << 13)  // PE_13 (D3)
#define P1_2    (1 << 11)  // PE_11 (D5)
#define PORT_1  PortE

#define P2_1    (1 << 15)  // PF_15 (D2)
#define P2_2    (1 << 14)  // PF_14 (D4)
#define PORT_2  PortF

#elif defined(TARGET_EFM32LG_STK3600) || defined(TARGET_EFM32GG_STK3700) || defined(TARGET_EFM32WG_STK3800)
#define P1_1    (1 << 0)  // PD0
#define P1_2    (1 << 1)  // PD1
#define PORT_1  PortD

#define P2_1    (1 << 3)  // PC3
#define P2_2    (1 << 4)  // PC4
#define PORT_2  PortC

#elif defined(TARGET_EFM32ZG_STK3200)
#define P1_1    (1 << 7)  // PD7
#define P1_2    (1 << 6)  // PD6
#define PORT_1  PortD

#define P2_1    (1 << 1)  // PC1
#define P2_2    (1 << 2)  // PC2
#define PORT_2  PortC

#elif defined(TARGET_EFM32HG_STK3400)
#define P1_1    (1 << 10)  // PE10
#define P1_2    (1 << 11)  // PE11
#define PORT_1  PortE

#define P2_1    (1 << 1)  // PC1
#define P2_2    (1 << 2)  // PC2
#define PORT_2  PortC

#elif defined(TARGET_EFM32PG_STK3401)
#define P1_1    (1 << 6)  // PC6
#define P1_2    (1 << 7)  // PC7
#define PORT_1  PortC

#define P2_1    (1 << 3)  // PA3
#define P2_2    (1 << 4)  // PA4
#define PORT_2  PortA

#elif defined(TARGET_SAMR21G18A) || defined(TARGET_SAMD21J18A)
#define P1_1    (1 << 6)  /*PA06*/
#define P1_2    (1 << 7)  /*PA07*/
#define PORT_1  PortA

#define P2_1    (1 << 2)  /*PB02*/
#define P2_2    (1 << 3)  /*PB03*/
#define PORT_2  PortB

#elif defined(TARGET_SAMD21G18A)
#define P1_1    (1 << 2)  /*PA02*/
#define P1_2    (1 << 3)  /*PA03*/
#define PORT_1  PortA

#define P2_1    (1 << 2)  /*PB02*/
#define P2_2    (1 << 3)  /*PB03*/
#define PORT_2  PortB

#elif defined(TARGET_SAML21J18A)
#define P1_1    (1 << 4)  /*PA04*/
#define P1_2    (1 << 5)  /*PA05*/
#define PORT_1  PortA

#define P2_1    (1 << 0)  /*PB00*/
#define P2_2    (1 << 1)  /*PB01*/
#define PORT_2  PortB

#elif defined(TARGET_SAMG55J19)
#define P1_1    (1 << 17)  /*PA17*/
#define P1_2    (1 << 18)  /*PA18*/
#define PORT_1  PortA

#define P2_1    (1 << 10)  /*PB10*/
#define P2_2    (1 << 11)  /*PB11*/
#define PORT_2  PortB

#elif defined(TARGET_VK_RZ_A1H)
#define P1_1    (1 << 2)  /*P3_2*/
#define P1_2    (1 << 7)  /*P3_7*/
#define PORT_1  Port3

#define P2_1    (1 << 6)  /*P5_6*/
#define P2_2    (1 << 1)  /*P5_1*/
#define PORT_2  Port5

#else
#error [NOT_SUPPORTED] This test is not defined on this target
#endif

#define MASK_1   (P1_1 | P1_2)
#define MASK_2   (P2_1 | P2_2)

PortInOut port1(PORT_1, MASK_1);
PortInOut port2(PORT_2, MASK_2);

int main() {
    MBED_HOSTTEST_TIMEOUT(20);
    MBED_HOSTTEST_SELECT(default_auto);
    MBED_HOSTTEST_DESCRIPTION(PortInOut);
    MBED_HOSTTEST_START("MBED_A11");

    bool check = true;

    port1.output();
    port2.input();

    port1 = MASK_1; wait(0.1);
    if (port2 != MASK_2) check = false;

    port1 = 0; wait(0.1);
    if (port2 != 0) check = false;

    port1.input();
    port2.output();

    port2 = MASK_2; wait(0.1);
    if (port1 != MASK_1) check = false;

    port2 = 0; wait(0.1);
    if (port1 != 0) check = false;

    MBED_HOSTTEST_RESULT(check);
}
