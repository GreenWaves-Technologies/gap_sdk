build:
	scons

clean:
	rm -f .sconsign.dblite
	scons -c

hwce_v4:
	regmap --name=hwce --input-xls=doc/hwce/HWCE_v4_reference.xlsx --header=include/archi/hwce/hwce_v4.h

hwce_v5:
	regmap --name=hwce --input-xls=doc/hwce/HWCE_v5_reference.xlsx --header=include/archi/hwce/hwce_v5.h

mchan_v7:
	regmap --name=mchan --input-xls=doc/mchan/CL_DMA_v7_reference.xlsx --header=include/archi/dma/mchan_v7.h

udma_mram_v1:
	regmap --name=udma_mram --input-xls=doc/udma/MRAM_reference.xlsx --header=include/archi/udma/mram/udma_mram_v1.h
	
udma_memcpy_v1:
	regmap --name=udma_memcpy --input-xls=doc/udma/MEMCPY_v1_reference.xlsx --header=include/archi/udma/memcpy/v1/udma_memcpy_v1
	
udma_i2s_v2:
	regmap --name=udma_i2s --input-xls=doc/udma/i2s/I2S_V2_reference.xlsx --header=include/archi/udma/i2s/udma_i2s_v2.h
	
udma_i2s_v3:
	regmap --name=udma_i2s --input-xls=doc/udma/i2s/I2S_V3_reference.xlsx --header=include/archi/udma/i2s/v3/udma_i2s_v3
	
udma_cpi_v1:
	regmap --name=udma_cpi --input-xls=doc/udma/CAM_CPI_V1_reference.xlsx --header=include/archi/udma/cpi/udma_cpi_v1.h
	
udma_hyper_v2:
	regmap --name=hyper --input-xls=doc/HYPER_V2_reference.xlsx --header=include/archi/udma/hyper/udma_hyper_v2.h
	
pmu_v3:
	regmap --name=maestro  --input-json=ips/pmu/pmu_v3.json           --header=include/archi/maestro/maestro_v3.h
	
gpio_v3:
	regmap --name=gpio  --input-xls=doc/gpio/APB_GPIO_V3_reference.xlsx           --header=include/archi/gpio/gpio_v3.h
	
pwm_v1:
	regmap --name=pwm  --input-xls=doc/pwm/pwm_v1.xlsx           --header=include/archi/pwm/v1/pwm_v1

vega.apb_soc_ctrl:
	regmap --name=apb_soc  --input-xls=doc/vega/APB_SOC_CTRL_reference.xlsx  --header=include/archi/chips/vega/apb_soc_ctrl.h

gap9: udma_i2s_v3

vega: udma_i2s_v2 pmu_v3 udma_hyper_v2 mchan_v7 udma_mram_v1 hwce_v5 gpio_v3 vega.apb_soc_ctrl udma_cpi_v1
	regmap --name=pmu      --input-json=chips/vega/pmu.json           --header=include/archi/chips/vega/pmu.h

gap: hwce_v4 udma_memcpy_v1 pwm_v1
	regmap --name=apb_soc  --input-xls doc/gap/APB_SOC_CTRL_reference.xlsx    --header=include/archi/chips/gap/apb_soc.h

pulpissimo: udma_i2s_v2
	regmap --name=apb_soc  --input-xls=doc/PULPISSIMO_APB_SOC_CTRL_reference.xlsx  --header=include/archi/chips/pulpissimo/apb_soc_ctrl.h

gen:
	regmap --name=itc      --input-json=ips/itc/itc_v1.json           --header=include/archi/itc/itc_v1.h
	regmap --name=udma_i2s --input-xls=doc/I2S_V1_reference.xlsx      --header=include/archi/udma/i2s/udma_i2s_v1_new.h
	regmap --name=maestro  --input-json=ips/pmu/pmu_v2.json           --header=include/archi/maestro/maestro_v2_new.h
	regmap --name=pmu      --input-json=chips/wolfe/pmu.json           --header=include/archi/chips/wolfe/pmu.h
	regmap --name=maestro  --input-json=ips/pmu/pmu_v1.json           --header=include/archi/maestro/maestro_v1_new.h
	regmap --name=timer    --input-xls=doc/TIMER_UNIT_reference.xlsx  --header=include/archi/timer/timer_v2.h
	regmap --name=rtc      --input-xls=doc/RTC_UNIT_reference.xlsx    --header=include/archi/rtc/rtc_v2.h
	regmap --name=apb_soc  --input-xls=doc/WOLFE_APB_SOC_CTRL_reference.xlsx  --header=include/archi/chips/wolfe/apb_soc_ctrl_new.h
	regmap --name=gpio     --input-xls=doc/APB_GPIO_reference.xlsx    --header=include/archi/gpio/gpio_v2_new.h

.PHONY: build clean gen


REGMAP = $(GAP_SDK_HOME)/rtos/pulpos/gap_archi/regmap/bin/regmap

pulp.gen:
	$(REGMAP) --name=apb_soc_ctrl \
		--input=arch/pulp/APB_SOC_CTRL_reference.md \
		--input-md=arch/pulp/APB_SOC_CTRL_reference.md \
		--header=include/archi/chips/pulp/apb_soc_ctrl/apb_soc_ctrl

	$(REGMAP) --name=cluster_ctrl_unit \
			--input=arch/pulp/CLUSTER_CRTL_UNIT_reference.md \
			--input-md=arch/pulp/CLUSTER_CRTL_UNIT_reference.md \
			--header=include/archi/chips/pulp/cluster_ctrl_unit/cluster_ctrl_unit

	$(REGMAP) --name=cluster_icache_ctrl \
			--input=arch/pulp/CLUSTER_ICACHE_CTRL_reference.md \
			--input-md=arch/pulp/CLUSTER_ICACHE_CTRL_reference.md \
			--header=include/archi/chips/pulp/cluster_icache_ctrl/cluster_icache_ctrl
