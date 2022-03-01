gap9_evk:
	$(MAKE) PMSIS_OS=freertos clean platform=fpga
	$(MAKE) PMSIS_OS=freertos platform=fpga io=host all
	cp BUILD/GAP9_V2/GCC_RISCV_FREERTOS/gap_flasher ../../gap_bins/gap_flasher-gap9_evk-fpga.elf
	$(MAKE) PMSIS_OS=freertos clean platform=board
	$(MAKE) PMSIS_OS=freertos platform=board io=host all
	cp BUILD/GAP9_V2/GCC_RISCV_FREERTOS/gap_flasher ../../gap_bins/gap_flasher-gap9_evk.elf
	$(MAKE) PMSIS_OS=freertos clean platform=board
	$(MAKE) MRAM=1 PMSIS_OS=freertos platform=board io=host all
	cp BUILD/GAP9_V2/GCC_RISCV_FREERTOS/gap_flasher ../../gap_bins/gap_flasher-gap9_evk-mram.elf

gap9_v2:
	$(MAKE) PMSIS_OS=freertos clean platform=board
	$(MAKE) PMSIS_OS=freertos platform=board io=host all
	cp BUILD/GAP9_V2/GCC_RISCV_FREERTOS/gap_flasher ../../gap_bins/gap_flasher-gap9_v2.elf
