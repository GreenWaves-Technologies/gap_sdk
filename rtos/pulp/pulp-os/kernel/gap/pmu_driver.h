/*
        Power and clock management unit driver: Regulator control, Clocks control, Retention control and system power states management
        January 2016.
        Author: Eric Flamand, GreenWaves Technologies. eric.flamand@greenwaves-technologies.com
*/


#ifndef __GAP8_PMU_DRIVER_H__
#define __GAP8_PMU_DRIVER_H__

#include "rt/rt_api.h"



/* PMU State:
        State:                  SOC_LP/HP, SOC_CLUSTER_LP/HP, RETENTIVE, DEEP_SLEEP.
				Bits[2:1]: Regulator State, Bit[0], Cluster On/Off
	FllClusterDown		Cluster FLL stop mode: FLL_FAST_WAKEUP, FLL_RETENTIVE or FLL_SHUTDOWN
	DCDC_Settings[]
        	REGU_NV         Nominal,   BaseValue + Incr*ValueNv             BaseValue and Incr are constants (550, 50)
        	REGU_LV         Low,       BaseValue + Incr*ValueLv             BaseValue and Incr are constants (550, 50)
        	REGU_RET        Retentive, BaseValue + Incr*ValueRet            BaseValue and Incr are constants (550, 50)
        	REGU_OFF        0
        Fll SoC: 		Frequency:      Value in Hz
        Fll Cluster: 		Frequency:      Value in Hz
*/
typedef enum {
	FLL_FAST_WAKEUP=0,	/* Fll is kept active and ready to be used */
	FLL_RETENTIVE=1,	/* Fll is stopped and it's state is kept in retention */
	FLL_SHUTDOWN=2		/* Fll is stopped and has to be re initialized when restarting */
} FllClusterDownT;

typedef struct {
	unsigned char State;
	unsigned char FllClusterDown;
	unsigned char DCDC_Settings[4];
        unsigned int  Frequency[N_FLL];
} PMU_StateT;

extern PMU_StateT 		PMUState;
extern PMU_RetentionStateT 	PMURetentionState;

#define PMU_ERROR_NO_ERROR                      (0)
#define PMU_ERROR_FREQ_TOO_HIGH                 (0x1<<0)
#define PMU_ERROR_NEED_STATE_CHANGE             (0x1<<1)
#define PMU_ERROR_VDD_CHANGE_TOO_HIGH           (0x1<<2)
#define PMU_ERROR_SOC_STATE_CHANGE_FAILED       (0x1<<3)
#define PMU_ERROR_CLUSTER_STATE_CHANGE_FAILED   (0x1<<4)

extern void 		   InitPMUDriver();

static inline int 	   PMU_ClusterIsDown()    { return (CLUSTER_STATE(PMUState.State)==CLUSTER_OFF);}
static inline int 	   PMU_ClusterIsRunning() { return (CLUSTER_STATE(PMUState.State)==CLUSTER_ON);}

static inline void	   SetFllClusterDownMode(unsigned int Mode) { PMUState.FllClusterDown = Mode; }

extern int 		   ChangePowerSystemState(PMU_SystemStateT State, unsigned int NewVoltage);
extern void 		   PMU_ShutDown(int Retentive, PMU_SystemStateT WakeUpState);

extern void 		   ReportPMUError(unsigned Error);

extern unsigned int 	   SetFllSoCFrequency(unsigned int Frequency);
extern unsigned int 	   SetFllClusterFrequency(unsigned int Frequency);
static inline unsigned int GetFllSoCFrequency()     { return PMUState.Frequency[FLL_SOC]; }
static inline unsigned int GetFllClusterFrequency() { return PMUState.Frequency[FLL_CLUSTER]; }

extern unsigned int RT_L2_DATA 	DCDC_MaxFreq[DCDC_OPER_POINTS];
extern RT_L2_DATA unsigned int 	FllsFrequency[N_FLL];
extern PMU_RetentionStateT RT_L2_DATA 	PMURetentionState;

#endif
