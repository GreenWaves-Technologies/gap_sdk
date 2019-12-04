/*
 * Copyright 2019 GreenWaves Technologies, SAS
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

#include "Gap8.h"
#include "pad.h"

#define SO(Size, Off)		((((Size)-1)<<5)|(Off))
#define READ_REG(Base, Off)		__builtin_pulp_OffsetedRead((unsigned int *) (Base), (Off))
#define	WRITE_REG(Val, Base, Off)	__builtin_pulp_OffsetedWrite((Val), (unsigned int *) (Base), (Off))

/*
 * return the 1st bit number of the PADFUN register to configure the padN alternate
 * @param padN is the number of the pad/pin of gap8 IC from 0 to 63
 * Aternate configuration is on 2 bits, here the 1st one location on 32 bits registers is return
 * use gap8GiveRegPadfun to get which register is associated with padN
 */
static int gap8GiveRegPadfunBit(unsigned int padN)
{
  unsigned int val;
  val = (padN - 8) * 2;
  if (val > 63)
    return val - 64;
  else if (val > 31)
    return val - 32;
  else
    return val;
}

/*
 * return the PADFUN register number from 0 to 3 in which to configure the alternate function of padN
 * @param padN is the number of the pad/pin of gap8 IC from 0 to 63
 */
int gap8GiveRegPadfun(unsigned int padN)
{
#ifndef __RELEASE__
  if ((padN < 8) || (padN > 51))
    return PAD_NO_ALTERNATE;
#endif
  return (padN - 8) >> 4;
}

/*
 * return the DS & PE config Index
 * DS means Drive Strength
 * PE means Pull-up/down Enable
 */
static int gap8GiveRegPadCfgIndex(unsigned int padN)
{
  unsigned int val;
  val = padN - 8;
  if ((padN>7) && (padN < 51))
    return val;
  else if ((padN > 52) && (padN < 58))
     return val - 2;
  else
    return PAD_NO_CONFIG_INDEX;
}

void gap8SetOnePadAlternate(unsigned int PadN, PadAlternateT Alt)
{
  unsigned int bit, val=0;
  int regN;
  regN = gap8GiveRegPadfun(PadN);
  bit = gap8GiveRegPadfunBit(PadN);
  val = READ_REG(ARCHI_APB_SOC_CTRL_ADDR, PADFUN0_OFFSET + (regN<<2));
  val = __builtin_pulp_binsert_r(val, (int) Alt, SO(ONE_PADFUN_SIZE, bit&0x1F));
  WRITE_REG(val, ARCHI_APB_SOC_CTRL_ADDR, PADFUN0_OFFSET + (regN<<2));
}

PadAlternateT gap8GetOnePadAlternate(unsigned int PadN)
{
  unsigned int bit, val=0;
  int regN;
  regN = gap8GiveRegPadfun(PadN);
  bit = gap8GiveRegPadfunBit(PadN);
  val = READ_REG(ARCHI_APB_SOC_CTRL_ADDR, PADFUN0_OFFSET + (regN<<2));
  return __builtin_pulp_bextractu_r(val, SO(ONE_PADFUN_SIZE, (bit&0x1F)));
}

void gap8SetAllPadAlternate(unsigned int Alt[])
{
  WRITE_REG(Alt[0], ARCHI_APB_SOC_CTRL_ADDR, PADFUN0_OFFSET);
  WRITE_REG(Alt[1], ARCHI_APB_SOC_CTRL_ADDR, PADFUN1_OFFSET);
  WRITE_REG(Alt[2], ARCHI_APB_SOC_CTRL_ADDR, PADFUN2_OFFSET);
}

/*
 * enable/disable a pad by setting its pull-up enable
 * see gap8GiveRegPadCfgIndex()
 */
int gap8EnableOnePad(unsigned int padN, unsigned char En)
{
  int cfgIn;
  cfgIn = gap8GiveRegPadCfgIndex(padN);
  unsigned int Reg = READ_REG(ARCHI_APB_SOC_CTRL_ADDR, PADCFG0_OFFSET + (cfgIn&0xFFFFFFFC));
  Reg = __builtin_pulp_binsert_r(Reg, En, (cfgIn&0x3));
  WRITE_REG(Reg, ARCHI_APB_SOC_CTRL_ADDR, PADCFG0_OFFSET + (cfgIn&0xFFFFFFFC));
  return 0;
}

/*
 * set the Pad Strength of a pad, see gap8GiveRegPadCfgIndex()
 */
int gap8SetOnePadStrength(unsigned int padN, unsigned char St)
{
  int cfgIn;
  cfgIn = gap8GiveRegPadCfgIndex(padN);
  unsigned int Reg = READ_REG(ARCHI_APB_SOC_CTRL_ADDR, PADCFG0_OFFSET + (cfgIn&0xFFFFFFFC));
  Reg = __builtin_pulp_binsert_r(Reg, St, (cfgIn&0x3)+1);
  WRITE_REG(Reg, ARCHI_APB_SOC_CTRL_ADDR, PADCFG0_OFFSET + (cfgIn&0xFFFFFFFC));
  return 0;
}

unsigned int gap8GetOnePadConfig(unsigned int padN)
{
  int cfgIn;
  cfgIn = gap8GiveRegPadCfgIndex(padN);
  unsigned int Reg = READ_REG(ARCHI_APB_SOC_CTRL_ADDR, PADCFG0_OFFSET + (cfgIn&0xFFFFFFFC));
  return __builtin_pulp_bextractu_r(Reg, SO(2, (cfgIn&0x3)));
}

void gap8SetAllPadConfig(unsigned int Config[])
{
  unsigned int i;
  for (i=0; i<PADCFG_GROUPS; i++) WRITE_REG(Config[i], ARCHI_APB_SOC_CTRL_ADDR, PADCFG0_OFFSET + (0x4*i));
}

//#define	APB_GPIO_BASE_ADDR	(SOC_PERIPHERALS_BASE_ADDR+0x1000)
#define	APB_GPIO_BASE_ADDR	(ARCHI_SOC_PERIPHERALS_ADDR+0x1000)


void gap8SetGPIODir(unsigned int Gpio, unsigned int Dir)
{
  unsigned int Reg = READ_REG(APB_GPIO_BASE_ADDR, GPIO_DIR_OFFSET);
  Reg = __builtin_pulp_binsert_r(Reg, Dir, Gpio);
  WRITE_REG(Reg, APB_GPIO_BASE_ADDR, GPIO_DIR_OFFSET);
}

void gap8WriteGPIO(unsigned int Gpio, unsigned int V)
{
  unsigned int Reg = READ_REG(APB_GPIO_BASE_ADDR, GPIO_OUT_OFFSET);
  Reg = __builtin_pulp_binsert_r(Reg, V, Gpio);
  WRITE_REG(Reg, APB_GPIO_BASE_ADDR, GPIO_OUT_OFFSET);
}

unsigned int gap8ReadGPIO(unsigned int Gpio)
{
  unsigned int Reg = READ_REG(APB_GPIO_BASE_ADDR, GPIO_IN_OFFSET);
  return __builtin_pulp_bextractu_r(Reg, Gpio);
}

void gap8EnableGPIOInt(unsigned int Gpio, unsigned int En)
{
  unsigned int Reg = READ_REG(APB_GPIO_BASE_ADDR, GPIO_INTEN_OFFSET);
  Reg = __builtin_pulp_binsert_r(Reg, En, Gpio);
  WRITE_REG(Reg, APB_GPIO_BASE_ADDR, GPIO_INTEN_OFFSET);
}

void gap8SetGPIOInterruptConfig(unsigned int Gpio, unsigned int Cfg)
{
  unsigned char bit=0;
  unsigned int Reg = READ_REG(APB_GPIO_BASE_ADDR, GPIO_INTCFG0_OFFSET + ((Gpio>>4)<<2));//Gpio>>4 give 1 for GPIO_INTCFG1_OFFSET and 0 for GPIO_INTCFG0_OFFSET, but register is on 4 bytes so need <<2
  Reg = __builtin_pulp_binsert_r(Reg, Cfg, SO(2, (Gpio&0x0F)<<1));
  WRITE_REG(Reg, APB_GPIO_BASE_ADDR, GPIO_INTCFG0_OFFSET + ((Gpio>>4)<<2));
}

unsigned int gap8GetGPIOInterruptConfig(unsigned int Gpio)
{
  unsigned int Reg = READ_REG(APB_GPIO_BASE_ADDR, GPIO_INTCFG0_OFFSET + ((Gpio>>4)<<2));
  return __builtin_pulp_bextractu_r(Reg, SO(2, (Gpio&0x0F)<<1));
}

unsigned int gap8ReadAndClearGPIOInterruptStatus(unsigned int Gpio)
{
  unsigned int Reg = READ_REG(APB_GPIO_BASE_ADDR, GPIO_INTSTATUS_OFFSET);
  return __builtin_pulp_bextractu_r(Reg, Gpio);
}

/*
 * return the GPIO number from 0 to 31 in gap8
 * @param padN is the number of the pad/pin of gap8 IC from 0 to 63
 */
int gap8GiveGpioNb(unsigned int padN)
{
  if ((padN >= 8) && (padN < 12))
    return padN - 8;
  if ((padN >= 12) && (padN < 18))
    return padN - 12;
  if ((padN >= 18) && (padN < 40))
    return 4 + padN - 18;
  if ((padN >= 42) && (padN < 44))
    return 26 + padN - 42;
  if (45 == padN)
    return 28;
  if ((padN >= 47) && (padN < 50))
    return 29 + padN - 47;
  else
    return PAD_NO_GPIO;
}

//Always-ON power section
/*
inline void gapEnablePadSleep(sleepPadEnT en)
{
  WRITE_REG(en, REG_PAD_SLEEP_OFFSET, AO_PAD_BASE_ADDR);
}*/

void gapSetOneSleepPadCfg(unsigned int PadN, sleepPadEnableT en, sleepPadValueT val)
{
  unsigned int Reg = READ_REG(AO_PAD_BASE_ADDR, REG_SLEEP_PADCFG0_OFFSET + ((PadN>>4)<<2));
  //printf("Set Reg=0x%x (int) ((val<<1)|en)=0x%x\n", Reg, (int) ((val<<1)|en));
  Reg = __builtin_pulp_binsert_r(Reg, (int) ((val<<1)|en), SO(SLEEP_ONE_PADCFG_SIZE, (PadN & 0x0F)*SLEEP_ONE_PADCFG_SIZE));
  //val = __builtin_pulp_binsert_r(val, (int) Alt, SO(ONE_PADFUN_SIZE, bit&0x1F));
  //printf("Set Reg=0x%x SO(SLEEP_ONE_PADCFG_SIZE, (PadN & 0x0F)*SLEEP_ONE_PADCFG_SIZE)=0x%x \n", Reg, SO(SLEEP_ONE_PADCFG_SIZE, (PadN & 0x0F)*SLEEP_ONE_PADCFG_SIZE));
  WRITE_REG(Reg, AO_PAD_BASE_ADDR, REG_SLEEP_PADCFG0_OFFSET + ((PadN>>4)<<2));
}

sleepPadEnableT gapGetOneSleepPadCfg(unsigned int PadN)
{
  unsigned int Reg;
  //unsigned int Reg = READ_REG(AO_PAD_BASE_ADDR, REG_SLEEP_PADCFG0_OFFSET + ((PadN>>4)<<2));
  Reg = READ_REG(AO_PAD_BASE_ADDR, REG_SLEEP_PADCFG0_OFFSET + ((PadN>>4)<<2));
  //printf("Get Reg=0x%x SO(SLEEP_ONE_PADCFG_SIZE, (PadN & 0x0F)*SLEEP_ONE_PADCFG_SIZE)=0x%x \n", Reg, SO(SLEEP_ONE_PADCFG_SIZE, (PadN & 0x0F)*SLEEP_ONE_PADCFG_SIZE));
  return __builtin_pulp_bextractu_r(Reg, SO(SLEEP_ONE_PADCFG_SIZE, (PadN&0x0F)*SLEEP_ONE_PADCFG_SIZE));
}

void gapSetAllSleepPadCfg(unsigned int cfg[])
//void gapSetAllSleepPadCfg()
{
 //printf("AO_PAD_BASE_ADDR=0x%x REG_SLEEP_PADCFG0_OFFSET=0x%x\n", AO_PAD_BASE_ADDR, REG_SLEEP_PADCFG0_OFFSET);
  //WRITE_REG(Alt[0], APB_SOC_CTRL_ADDR, PADFUN0_OFFSET);
  WRITE_REG(cfg[0], AO_PAD_BASE_ADDR, REG_SLEEP_PADCFG0_OFFSET);
  WRITE_REG(cfg[1], AO_PAD_BASE_ADDR, REG_SLEEP_PADCFG1_OFFSET);
  WRITE_REG(cfg[2], AO_PAD_BASE_ADDR, REG_SLEEP_PADCFG2_OFFSET);
  WRITE_REG(cfg[3], AO_PAD_BASE_ADDR, REG_SLEEP_PADCFG3_OFFSET);
/*
  unsigned int i;
  for (i=0; i<SLEEP_PAD_REG_NB; i++)
  {
    //WRITE_REG(cfg[i], AO_PAD_BASE_ADDR, REG_SLEEP_PADCFG0_OFFSET + (0x4*i));
    //printf("val=0x%x\n", cfg[i]);
    //printf("val=0x%x\n", *(volatile unsigned int *) (REG_SLEEP_PADCFG0_OFFSET + 0x4*i));
  }
*/
}

void gapGetAllSleepPadCfg(unsigned int cfg[])
{
  unsigned int i;
  for (i=0; i<SLEEP_PAD_REG_NB; i++)
  {
    cfg[i] = READ_REG(AO_PAD_BASE_ADDR, REG_SLEEP_PADCFG0_OFFSET + (0x4*i));
    //printf("sleep pad register %i=0x%x val=0x%x\n", i, *(volatile unsigned int *) (REG_SLEEP_PADCFG0_OFFSET + 0x4*i), cfg[i]);
  }
}


#undef SO
#undef READ_REG
#undef	WRITE_REG
