DOC_IP ?= rtc soc_eu efuse udma gpio adv_timer fc_mpu fc_icache_ctrl cluster_ctrl_unit apb_soc_ctrl cl_dma timer_unit ne16 i3c cluster_icache_ctrl fc_itc power_manager secured_riscv_debug non_secured_riscv_debug xip sfu fll riscv_dbg rom

#TODO this doc has been removed: quiddikey
#TODO add back udma AES when markdown is added

xip:
	./regmap/bin/regmap --name=xip \
		--input=fe/ips/XIP/README.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/XIP/README.md \
		--header=include/archi/chips/gap9_v2/xip/xip \
		--rst=doc/ips/xip.rst

rtc:
	./regmap/bin/regmap --name=rtc \
		--input=fe/ips/rtc_dolphin/docs/RTC_reference.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/rtc_dolphin/docs/RTC_reference.md \
		--header=include/archi/chips/gap9_v2/rtc/rtc \
		--rst=doc/ips/rtc_v1.rst

fc_icache_ctrl:
	./regmap/bin/regmap --name=fc_icache_ctrl \
		--input=docs/IP_REFERENCES/FC_ICACHE_CTRL_reference.md \
		--input-md=$(GAP9_V2_HOME)/docs/IP_REFERENCES/FC_ICACHE_CTRL_reference.md \
		--header=include/archi/chips/gap9_v2/fc_icache_ctrl/fc_icache_ctrl \
		--rst=doc/ips/fc_icache_ctrl.rst

non_secured_riscv_debug:
	./regmap/bin/regmap --name=non_secured_riscv_debug \
		--input=fe/ips/riscv_gwt/doc/NONSECURED_RI5CY_DEBUG_reference.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/riscv_gwt/doc/NONSECURED_RI5CY_DEBUG_reference.md \
		--header=include/archi/chips/gap9_v2/non_secured_riscv_debug/non_secured_riscv_debug \
		--rst=doc/ips/non_secured_riscv_debug.rst

secured_riscv_debug:
	./regmap/bin/regmap --name=secured_riscv_debug \
		--input=fe/ips/riscv_gwt/doc/SECURED_RI5CY_DEBUG_reference.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/riscv_gwt/doc/SECURED_RI5CY_DEBUG_reference.md \
		--header=include/archi/chips/gap9_v2/secured_riscv_debug/secured_riscv_debug \
		--rst=doc/ips/secured_riscv_debug.rst

fc_itc:
	./regmap/bin/regmap --name=fc_itc \
		--input=fe/ips/apb_interrupt_cntrl/docs/FC_ITC_reference.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/apb_interrupt_cntrl/docs/FC_ITC_reference.md \
		--header=include/archi/chips/gap9_v2/fc_itc/fc_itc \
		--rst=doc/ips/fc_itc.rst

power_manager:
	./regmap/bin/regmap --name=power_manager \
		--input=fe/ips/power_manager_vega/docs/DLC_reference.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/power_manager_vega/docs/DLC_reference.md \
		--header=include/archi/chips/gap9_v2/power_manager/power_manager \
		--rst=doc/ips/power_manager.rst

cluster_ctrl_unit:
	./regmap/bin/regmap --name=cluster_ctrl_unit \
		--input=docs/IP_REFERENCES/CLUSTER_CRTL_UNIT_reference.md \
		--input-md=$(GAP9_V2_HOME)/docs/IP_REFERENCES/CLUSTER_CRTL_UNIT_reference.md \
		--header=include/archi/chips/gap9_v2/cluster_ctrl_unit/cluster_ctrl_unit \
		--rst=doc/ips/cluster_ctrl_unit.rst

cluster_icache_ctrl:
	./regmap/bin/regmap --name=cluster_icache_ctrl \
		--input=fe/ips/hier-icache/DOC/CLUSTER_ICACHE_CTRL_reference.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/hier-icache/DOC/CLUSTER_ICACHE_CTRL_reference.md \
		--header=include/archi/chips/gap9_v2/cluster_icache_ctrl/cluster_icache_ctrl \
		--rst=doc/ips/cluster_icache_ctrl.rst

apb_soc_ctrl:
	./regmap/bin/regmap --name=apb_soc_ctrl \
		--input=docs/IP_REFERENCES/APB_SOC_CTRL_reference.md \
		--input-md=$(GAP9_V2_HOME)/docs/IP_REFERENCES/APB_SOC_CTRL_reference.md \
		--header=include/archi/chips/gap9_v2/apb_soc_ctrl/apb_soc_ctrl \
		--rst=doc/ips/apb_soc_ctrl.rst

rom:
	./regmap/bin/regmap --name=rom \
		--input=docs/IP_REFERENCES/GAP9_ROM.md \
		--input-md=$(GAP9_V2_HOME)/docs/IP_REFERENCES/GAP9_ROM.md \
		--header=include/archi/chips/gap9_v2/rom/rom \
		--rst=doc/ips/rom.rst

cl_dma:
	./regmap/bin/regmap --name=cl_dma \
		--input=fe/ips/mchan/doc/CL_DMA_reference.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/mchan/doc/CL_DMA_reference.md \
		--header=include/archi/chips/gap9_v2/cl_dma/cl_dma \
		--rst=doc/ips/cl_dma.rst

timer_unit:
	./regmap/bin/regmap --name=timer_unit \
		--input=fe/ips/timer_unit/doc/TIMER_UNIT_reference.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/timer_unit/doc/TIMER_UNIT_reference.md \
		--header=include/archi/chips/gap9_v2/timer_unit/timer_unit \
		--rst=doc/ips/timer_unit.rst

# Stopped here
quiddikey:
	./regmap/bin/regmap --name=quiddikey \
		--input=fe/ips/quiddikey/doc/quiddikey_reference.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/quiddikey/doc/quiddikey_reference.md\
		--header=include/archi/chips/gap9_v2/quiddikey/quiddikey \
		--rst=doc/ips/quiddikey.rst

ne16:
	./regmap/bin/regmap --name=ne16 \
		--input=fe/ips/ne16/docs/ne16.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/ne16/docs/ne16.md \
		--header=include/archi/chips/gap9_v2/ne16/ne16 \
		--rst=doc/ips/ne16.rst

i3c:
	./regmap/bin/regmap --name=i3c \
		--input=fe/ips/lnt_i3c_master/docs/I3C_reference.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/lnt_i3c_master/docs/I3C_reference.md \
		--header=include/archi/chips/gap9_v2/i3c/i3c \
		--rst=doc/ips/i3c.rst

soc_eu:
	./regmap/bin/regmap --name=soc_eu \
		--input=docs/IP_REFERENCES/SOC_EVENT_GENERATOR_reference.md \
		--input-md=$(GAP9_V2_HOME)/docs/IP_REFERENCES/SOC_EVENT_GENERATOR_reference.md \
		--header=include/archi/chips/gap9_v2/soc_eu/soc_eu \
		--rst=doc/ips/soc_eu.rst

efuse:
	./regmap/bin/regmap --name=efuse \
		--input=fe/ips/apb/apb_efuse_if/README.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/apb/apb_efuse_if/README.md \
		--header=include/archi/chips/gap9_v2/efuse/efuse \
		--rst=doc/ips/efuse.rst

udma_filter:
	./regmap/bin/regmap --name=udma_filter \
		--input=fe/ips/udma/udma_filter/docs/FILTER_reference.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/udma/udma_filter/docs/FILTER_reference.md \
		--header=include/archi/chips/gap9_v2/udma_filter/udma_filter \
		--rst=doc/ips/udma_filter.rst

udma_qspi:
	cd $(GAP9_V2_HOME)/fe/ips/udma/udma_qspi && make doc.ip HEADERS_PATH=$(CURDIR)/include/archi/chips/gap9_v2 RST_PATH=$(CURDIR)/doc/ips

udma_hyper:
	./regmap/bin/regmap \
		--name=udma_hyper \
		--input=fe/ips/udma/udma_hyperbus_octospi/README.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/udma/udma_hyperbus_octospi/README.md \
		--header=include/archi/chips/gap9_v2/udma_hyper/udma_hyper \
		--rst=doc/ips/udma_hyper.rst

sfu:
	./regmap/bin/regmap \
		--name=sfu \
		--input=fe/ips/udma/udma_anc/README.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/udma/udma_anc/README.md \
		--rst=doc/ips/sfu.rst

	./regmap/bin/regmap \
		--name=sfu \
		--input=fe/ips/udma/udma_anc/README.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/udma/udma_anc/README.md \
		--header=include/archi/chips/gap9_v2/sfu/sfu \
		--register=CLK_SETUP --register=CLK_EN --register=CLK_CFG_0 --register=CLK_CFG_1 --register=CLK_CFG_2 --register=GRAPH_MASK \
		--register=MEM_IN_CFG_0 --register=MEM_IN_CFG_1 --register=STREAM_IN_CFG_0 --register=STREAM_IN_CFG_1 \
		--register=PDM_IN_CFG0 --register=PDM_IN_CFG1 --register=MEM_OUT_CFG --register=STREAM_OUT_CFG --register=PDM_OUT_CFG \
		--register=LIMITER_CFG --register=GFU_CFG --register=MIXER_CFG --register=SPLITTER_CFG --register=PIPELINE_CFG \
		--register=RESAMPLER_CFG --register=POLYPHASE_CFG --register=ASRC_CFG --register=NORMSAT_CFG --register=GFU_GLOB \
		--register=GFU_CFG_A --register=GFU_CFG_B --register=GFU_CFG_C --register=GFU_CFG_D --register=GFU_CFG_E \
		--register=MIXER_GLOB --register=MIXER_CTX_OUT --register=MIXER_CTX_CFG --register=MIXER_CTX_INIT \
		--register=MIXER_CTX_BIAS --register=MIXER_PORT_CFG \
		--register=SPLITTER_GLOB --register=SPLITTER_CTX_CFG --register=SPLITTER_PORT_CFG \
		--register=PIPE_GLOB --register=PIPE_INT_CFG \
		--register=RESAMPLER_GLOB --register=RESAMPLER_CFG0 --register=RESAMPLER_CFG1 --register=RESAMPLER_CFG2 \
		--register=LIMITER_INT_CFG --register=LIMITER_COEFF \
		--register=POLYPHASE_GLOB --register=POLYPHASE_CFG_A --register=POLYPHASE_CFG_B --register=POLYPHASE_CFG_C \
		--register=POLYPHASE_CFG_D --register=POLYPHASE_CFG_E --register=POLYPHASE_CFG_OD \
		--register=FIFOMODE_GLOB --register=FIFOMODE_CFG_A --register=FIFOMODE_CFG_B --register=FIFOMODE_CFG_C --register=FIFOMODE_CFG_D \
		--register=NORMSAT_GLOB --register=NORMSAT_INT_CFG

udma_ffc:
	./regmap/bin/regmap \
		--name=udma_ffc\
		--input=fe/ips/udma/udma_ffc/README.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/udma/udma_ffc/README.md \
		--header=include/archi/chips/gap9_v2/udma_ffc/udma_ffc \
		--rst=doc/ips/udma_ffc.rst

udma_mram:
	-./regmap/bin/regmap \
		--name=udma_mram \
		--input=fe/ips/udma/udma_mram/README.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/udma/udma_mram/README.md \
		--header=include/archi/chips/gap9_v2/udma_mram/udma_mram \
		--rst=doc/ips/udma_mram.rst

udma_i2s:
	cd $(GAP9_V2_HOME)/fe/ips/udma/udma_i2s && make doc.ip HEADERS_PATH=$(CURDIR)/include/archi/chips/gap9_v2 RST_PATH=$(CURDIR)/doc/ips

udma_uart:
#	cd $(GAP9_V2_HOME)/fe/ips/udma/udma_uart && make doc.ip HEADERS_PATH=$(CURDIR)/include/archi/chips/gap9_v2 RST_PATH=$(CURDIR)/doc/ips
	./regmap/bin/regmap \
		--name=udma_uart \
		--input=fe/ips/udma/udma_uart/docs/doc.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/udma/udma_uart/docs/doc.md \
		--header=include/archi/chips/gap9_v2/udma_uart/udma_uart \
		--rst=doc/ips/udma_uart.rst

udma_cpi:
	./regmap/bin/regmap \
		--name=udma_cpi \
		--input=fe/ips/udma/udma_camera/doc/CAM_CPI_reference.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/udma/udma_camera/doc/CAM_CPI_reference.md \
		--header=include/archi/chips/gap9_v2/udma_cpi/udma_cpi \
		--rst=doc/ips/udma_cpi.rst

udma_timestamp:
	./regmap/bin/regmap \
		--name=udma_timestamp \
		--input=fe/ips/udma/fe/ips/udma/udma_timestamp/README.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/udma/udma_timestamp/README.md \
		--header=include/archi/chips/gap9_v2/udma_timestamp/udma_timestamp \
		--rst=doc/ips/udma_timestamp.rst


udma_i2c:
	./regmap/bin/regmap \
		--name=udma_i2c \
		--input=fe/ips/udma/udma_i2c/README.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/udma/udma_i2c/README.md \
		--header=include/archi/chips/gap9_v2/udma_i2c/udma_i2c \
		--rst=doc/ips/udma_i2c.rst

udma_ctrl:
	cd $(GAP9_V2_HOME)/fe/ips/udma/udma_core && make doc.ip HEADERS_PATH=$(CURDIR)/include/archi/chips/gap9_v2 RST_PATH=$(CURDIR)/doc/ips

udma_core_fifo:
	./regmap/bin/regmap \
		--name=udma_core_fifo \
		--input=fe/ips/udma/udma_core/doc/UDMA_CORE_FIFO_ADDRGEN_reference.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/udma/udma_core/doc/UDMA_CORE_FIFO_ADDRGEN_reference.md \
		--header=include/archi/chips/gap9_v2/udma_core_fifo/udma_core_fifo \
		--rst=doc/ips/udma_core_fifo.rst

udma_core_lin:
	./regmap/bin/regmap \
		--name=udma_core_lin_addrgen \
		--input=fe/ips/udma/udma_core/doc/UDMA_CORE_LIN_ADDRGEN_reference.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/udma/udma_core/doc/UDMA_CORE_LIN_ADDRGEN_reference.md \
		--header=include/archi/chips/gap9_v2/udma_core_lin/udma_core_lin \
		--rst=doc/ips/udma_core_lin.rst

udma_core_2d:
	./regmap/bin/regmap \
		--name=udma_core_2d \
		--input=fe/ips/udma/udma_core/doc/UDMA_CORE_2D_ADDRGEN_reference.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/udma/udma_core/doc/UDMA_CORE_2D_ADDRGEN_reference.md \
		--header=include/archi/chips/gap9_v2/udma_core_2d/udma_core_2d \
		--rst=doc/ips/udma_core_2d.rst

udma_aes:
	./regmap/bin/regmap \
		--name=udma_aes \
		--input=fe/ips/udma/udma_aes/README.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/udma/udma_aes/README.md \
		--header=include/archi/chips/gap9_v2/udma_aes/udma_aes \
		--rst=doc/ips/udma_aes.rst

udma_aes_dual_core:
	./regmap/bin/regmap \
		--name=udma_aes_dual_core \
		--input=fe/ips/udma/udma_aes/README_dual_core.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/udma/udma_aes/README_dual_core.md \
		--header=include/archi/chips/gap9_v2/udma_aes_dual_core/udma_aes_dual_core \
		--rst=doc/ips/udma_aes_dual_core.rst

udma:  udma_filter udma_qspi udma_hyper udma_ctrl udma_core_fifo udma_core_lin udma_core_2d udma_mram udma_i2s udma_uart udma_cpi udma_i2c udma_aes udma_aes_dual_core udma_timestamp

gpio:
	./regmap/bin/regmap \
		--name=gpio \
		--input=fe/ips/apb/apb_gpio/docs/APB_GPIO_reference.md \
		--input-md=$(GAP9_V2_HOME)/./fe/ips/apb/apb_gpio/docs/APB_GPIO_reference.md \
		--header=include/archi/chips/gap9_v2/gpio/gpio \
		--rst=doc/ips/gpio.rst

adv_timer:
	./regmap/bin/regmap --name=adv_timer \
		--input=fe/ips/apb/apb_adv_timer/doc/APB_ADV_TIMER_reference.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/apb/apb_adv_timer/doc/APB_ADV_TIMER_reference.md \
		--header=include/archi/chips/gap9_v2/pwm/pwm \
		--rst=doc/ips/adv_timer.rst

fc_mpu:
	./regmap/bin/regmap --name=fc_mpu \
		--input=docs/IP_REFERENCES/FC_MPU_reference.md \
		--input-md=$(GAP9_V2_HOME)/docs/IP_REFERENCES/FC_MPU_reference.md \
		--header=include/archi/chips/gap9_v2/fc_mpu/fc_mpu \
		--rst=doc/ips/fc_mpu.rst

fll:
	./regmap/bin/regmap \
		--name=fll \
		--input=fe/ips/fll_new/README.md \
		--input-md=$(GAP9_V2_HOME)/fe/ips/fll_new/README.md \
		--header=include/archi/chips/gap9_v2/fll/fll \
		--rst=doc/ips/fll.rst

riscv_dbg:
	./regmap/bin/regmap --name=riscv_dbg_dm \
		--input=docs/IP_REFERENCES/RISCV_DBG_DM_reference.md \
		--input-md=$(GAP9_V2_HOME)/docs/IP_REFERENCES/RISCV_DBG_DM_reference.md \
		--header=include/archi/riscv_dbg/riscv_dbg_dm \
		--rst=doc/ips/riscv_dbg_dm.rst


build:
	rsync -avR --copy-links include $(TARGET_INSTALL_DIR)
	rsync -avR --copy-links include $(INSTALL_DIR)

clean:
	rm -f .sconsign.dblite
	scons -c

gen: $(DOC_IP)

.PHONY: build clean gen
