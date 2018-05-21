onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate -divider udma
add wave -noupdate /tb/tb_test_i/boot_end
add wave -noupdate /tb/tb_test_i/boot_end_spi
add wave -noupdate /tb/tb_test_i/boot_end_hyper
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/RDATA_IN_WIDTH}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/clk}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/rst_n}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/prefetch_i}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/prefetch_addr_i}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/branch_i}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/branch_addr_i}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/hwlp_i}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/hwlp_addr_i}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/fetch_gnt_o}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/fetch_valid_o}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/valid_o}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/rdata_o}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/addr_o}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/instr_req_o}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/instr_addr_o}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/instr_gnt_i}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/instr_rvalid_i}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/instr_rdata_i}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/busy_o}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/CS}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/NS}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/L0_buffer}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/addr_q}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/instr_addr_int}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/valid}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/fetch_gnt_int}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/send_rvalid_int}
add wave -noupdate -group prefetch_L0_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/fetch_valid_int}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/RDATA_IN_WIDTH}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/clk}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/rst_n}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/req_i}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/branch_i}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/addr_i}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/hwloop_i}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/hwloop_target_i}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/ready_i}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/valid_o}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/rdata_o}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/addr_o}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/is_hwlp_o}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/instr_req_o}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/instr_addr_o}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/instr_gnt_i}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/instr_rvalid_i}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/instr_rdata_i}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/busy_o}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/busy_L0}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/CS}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/NS}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/do_fetch}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/do_hwlp}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/do_hwlp_int}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/use_last}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/save_rdata_last}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/use_hwlp}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/save_rdata_hwlp}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/valid}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/hwlp_is_crossword}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/is_crossword}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/next_is_crossword}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/next_valid}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/next_upper_compressed}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/fetch_possible}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/upper_is_compressed}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/addr_q}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/addr_n}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/addr_int}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/addr_aligned_next}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/addr_real_next}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/is_hwlp_q}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/is_hwlp_n}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/rdata_last_q}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/valid_L0}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/rdata_L0}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/addr_L0}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/fetch_valid}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/fetch_gnt}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/rdata}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/rdata_unaligned}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/aligned_is_compressed}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/unaligned_is_compressed}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/hwlp_aligned_is_compressed}
add wave -noupdate -group prefetch_buffer {/tb/pulp_chip_iface_wrap_i/i_dut/cluster_domain_i/cluster_i/CORE[0]/core_region_i/RISCV_CORE/if_stage_i/prefetch_128/prefetch_buffer_i/hwlp_unaligned_is_compressed}
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/clk_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/rst_ni
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/clock_en_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/test_en_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/boot_addr_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/core_id_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cluster_id_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/instr_req_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/instr_gnt_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/instr_rvalid_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/instr_addr_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/instr_rdata_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/data_req_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/data_gnt_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/data_rvalid_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/data_we_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/data_be_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/data_addr_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/data_wdata_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/data_rdata_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/data_err_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_master_tag_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_master_req_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_master_ready_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_master_gnt_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_master_op_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_master_type_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_master_flags_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_master_valid_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_master_result_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_master_flags_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/irq_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/irq_id_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/irq_ack_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/irq_id_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/irq_sec_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/sec_lvl_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/debug_req_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/debug_gnt_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/debug_rvalid_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/debug_addr_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/debug_we_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/debug_wdata_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/debug_rdata_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/debug_halted_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/debug_halt_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/debug_resume_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/fetch_enable_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/core_busy_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ext_perf_counters_i
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/is_hwlp_id
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/hwlp_dec_cnt_id
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/instr_valid_id
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/instr_rdata_id
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/is_compressed_id
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/illegal_c_insn_id
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/pc_if
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/pc_id
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/clear_instr_valid
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/pc_set
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/pc_mux_id
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/exc_pc_mux_id
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/exc_cause
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/trap_addr_mux
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/lsu_load_err
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/lsu_store_err
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/is_decoding
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/useincr_addr_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/data_misaligned
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/mult_multicycle
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/jump_target_id
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/jump_target_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/branch_in_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/branch_decision
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ctrl_busy
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_busy
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/lsu_busy
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_busy
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/pc_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/alu_en_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/alu_operator_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/alu_operand_a_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/alu_operand_b_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/alu_operand_c_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/bmask_a_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/bmask_b_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/imm_vec_ext_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/alu_vec_mode_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/mult_operator_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/mult_operand_a_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/mult_operand_b_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/mult_operand_c_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/mult_en_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/mult_sel_subword_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/mult_signed_mode_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/mult_imm_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/mult_dot_op_a_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/mult_dot_op_b_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/mult_dot_op_c_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/mult_dot_signed_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/mult_cplx_shift_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_en_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_op_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_lat_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_waddr_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_read_regs
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_read_regs_valid
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_read_dep
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_write_regs
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_write_regs_valid
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_write_dep
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/perf_apu_type
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/perf_apu_cont
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/perf_apu_dep
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/perf_apu_wb
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/regfile_waddr_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/regfile_we_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/regfile_waddr_fw_wb_o
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/regfile_we_wb
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/regfile_wdata
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/regfile_alu_waddr_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/regfile_alu_we_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/regfile_alu_waddr_fw
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/regfile_alu_we_fw
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/regfile_alu_wdata_fw
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/csr_access_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/csr_op_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/mtvec
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/utvec
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/csr_access
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/csr_op
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/csr_addr
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/csr_addr_int
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/csr_rdata
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/csr_wdata
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/current_priv_lvl
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/data_we_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/data_type_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/data_sign_ext_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/data_reg_offset_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/data_req_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/data_load_event_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/data_misaligned_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/lsu_rdata
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/halt_if
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_ready
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_ready
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_valid
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_valid
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/wb_valid
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/lsu_ready_ex
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/lsu_ready_wb
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/apu_ready_wb
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/instr_req_int
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/m_irq_enable
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/u_irq_enable
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/csr_irq_sec
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/epc
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/csr_save_cause
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/csr_save_if
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/csr_save_id
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/csr_cause
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/csr_restore_mret_id
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/csr_restore_uret_id
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/hwlp_start
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/hwlp_end
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/hwlp_cnt
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/csr_hwlp_regid
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/csr_hwlp_we
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/csr_hwlp_data
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/dbg_settings
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/dbg_req
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/dbg_ack
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/dbg_stall
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/dbg_trap
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/dbg_reg_rreq
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/dbg_reg_raddr
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/dbg_reg_rdata
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/dbg_reg_wreq
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/dbg_reg_waddr
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/dbg_reg_wdata
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/dbg_csr_req
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/dbg_csr_addr
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/dbg_csr_we
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/dbg_csr_wdata
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/dbg_jump_addr
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/dbg_jump_req
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/perf_imiss
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/perf_jump
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/perf_jr_stall
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/perf_ld_stall
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/core_ctrl_firstfetch
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/core_busy_int
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/core_busy_q
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/clk
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/clock_en
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/dbg_busy
add wave -noupdate -group FC /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/sleeping
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/boot_addr_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/branch_req
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/clear_instr_valid_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/clk
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/dbg_jump_addr_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/dbg_jump_req_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/exc_pc
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/exc_pc_mux_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/exc_vec_pc_mux_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/exception_pc_reg_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/FPU
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/halt_if_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/hwlp_cnt_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/hwlp_dec_cnt
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/hwlp_dec_cnt_id_o
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/hwlp_dec_cnt_if
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/hwlp_end_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/hwlp_jump
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/hwlp_start_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/hwlp_target
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/id_ready_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/if_busy_o
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/if_ready
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/if_valid
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/illegal_c_insn
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/illegal_c_insn_id_o
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/instr_addr_o
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/instr_compressed_int
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/instr_decompressed
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/instr_gnt_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/instr_rdata_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/instr_rdata_id_o
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/instr_req_o
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/instr_rvalid_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/instr_valid_id_o
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/is_compressed_id_o
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/is_hwlp_id_o
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/is_hwlp_id_q
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/jump_target_ex_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/jump_target_id_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/m_trap_base_addr_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/N_HWLP
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/offset_fsm_cs
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/offset_fsm_ns
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/fetch_addr
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/fetch_addr_n
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/fetch_is_hwlp
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/fetch_rdata
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/fetch_ready
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/fetch_valid
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/pc_if_o
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/pc_id_o
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/valid
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/pc_mux_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/pc_set_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/perf_imiss_o
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/prefetch_busy
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/RDATA_WIDTH
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/req_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/rst_n
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/trap_addr_mux_i
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/trap_base_addr
add wave -noupdate -expand -group fc_if /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/if_stage_i/u_trap_base_addr_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/alu_bmask_a_mux_sel
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/alu_bmask_b_mux_sel
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/alu_en
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/alu_en_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/alu_op_a_mux_sel
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/alu_op_b_mux_sel
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/alu_op_c_mux_sel
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/alu_operand_a
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/alu_operand_a_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/alu_operand_b
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/alu_operand_b_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/alu_operand_c
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/alu_operand_c_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/alu_operator
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/alu_operator_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/alu_vec_mode
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/alu_vec_mode_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/APU
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_busy_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_en
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_en_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_flags
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_flags_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_flags_src
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_lat
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_lat_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_op
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_op_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_operands
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_operands_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_perf_dep_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_read_dep_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_read_regs
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_read_regs_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_read_regs_valid
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_read_regs_valid_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_stall
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_type
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_type_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_waddr
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_waddr_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_write_dep_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_write_regs
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_write_regs_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_write_regs_valid
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/apu_write_regs_valid_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/bmask_a_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/bmask_a_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/bmask_a_id_imm
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/bmask_a_mux
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/bmask_b_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/bmask_b_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/bmask_b_id_imm
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/bmask_b_mux
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/branch_decision_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/branch_in_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/branch_taken_ex
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/clear_instr_valid_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/clk
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/core_ctrl_firstfetch_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/csr_access
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/csr_access_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/csr_apu_stall
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/csr_cause_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/csr_hwlp_data_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/csr_hwlp_regid_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/csr_hwlp_we_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/csr_irq_sec_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/csr_op
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/csr_op_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/csr_restore_mret_id_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/csr_restore_uret_id_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/csr_save_cause_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/csr_save_id_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/csr_save_if_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/csr_status
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/ctrl_busy_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/current_priv_lvl_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/data_load_event_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/data_load_event_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/data_misaligned_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/data_misaligned_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/data_reg_offset_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/data_reg_offset_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/data_req_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/data_req_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/data_sign_ext_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/data_sign_ext_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/data_type_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/data_type_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/data_we_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/data_we_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/dbg_ack_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/dbg_jump_req_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/dbg_reg_raddr_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/dbg_reg_rdata_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/dbg_reg_rreq_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/dbg_reg_waddr_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/dbg_reg_wdata_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/dbg_reg_wreq_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/dbg_req_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/dbg_settings_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/dbg_stall_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/dbg_trap_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/deassert_we
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/ebrk_insn
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/ecall_insn_dec
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/ex_ready_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/ex_valid_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/exc_ack
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/exc_cause_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/exc_kill
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/exc_pc_mux_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/fetch_enable_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/fp_rnd_mode
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/FPU
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/fpu_op
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/fpu_op_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/frm_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/halt_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/halt_if_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/hwloop_cnt
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/hwloop_cnt_int
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/hwloop_cnt_mux_sel
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/hwloop_end
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/hwloop_regid
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/hwloop_regid_int
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/hwloop_start
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/hwloop_start_int
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/hwloop_start_mux_sel
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/hwloop_target
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/hwloop_target_mux_sel
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/hwloop_valid
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/hwloop_we
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/hwloop_we_int
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/hwlp_cnt_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/hwlp_dec_cnt_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/hwlp_end_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/hwlp_start_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/id_ready_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/id_valid_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/illegal_c_insn_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/illegal_insn_dec
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_a
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_a_mux_sel
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_b
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_b_mux_sel
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_bi_type
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_clip_type
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_i_type
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_iz_type
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_s2_type
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_s3_type
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_s_type
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_sb_type
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_shuffle_type
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_shuffleb_type
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_shuffleh_type
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_u_type
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_uj_type
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_vec_ext_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_vec_ext_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_vs_type
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_vu_type
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/imm_z_type
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/instr
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/instr_rdata_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/instr_req_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/instr_valid_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/irq_ack_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/irq_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/irq_id_ctrl
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/irq_id_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/irq_id_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/irq_req_ctrl
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/irq_sec_ctrl
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/irq_sec_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/is_compressed_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/is_decoding_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/is_hwlp_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/jr_stall
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/jump_in_dec
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/jump_in_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/jump_target
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/jump_target_mux_sel
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/jump_target_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/load_stall
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/lsu_load_err_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/lsu_store_err_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/m_irq_enable_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/misaligned_stall
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mret_insn_dec
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_cplx_shift
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_cplx_shift_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_dot_en
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_dot_op_a_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_dot_op_b_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_dot_op_c_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_dot_signed
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_dot_signed_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_en
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_en_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_imm_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_imm_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_imm_mux
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_int_en
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_multicycle_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_operand_a_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_operand_b_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_operand_c_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_operator
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_operator_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_sel_subword
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_sel_subword_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_signed_mode
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/mult_signed_mode_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/N_HWLP
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/N_HWLP_BITS
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/operand_a_fw_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/operand_a_fw_mux_sel
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/operand_b
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/operand_b_fw_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/operand_b_fw_mux_sel
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/operand_b_vec
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/operand_c_fw_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/operand_c_fw_mux_sel
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/pc_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/pc_id_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/pc_if_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/pc_mux_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/pc_set_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/perf_jr_stall_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/perf_jump_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/perf_ld_stall_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/pipe_flush_dec
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/prepost_useincr
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/prepost_useincr_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/PULP_SECURE
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/reg_d_alu_is_reg_a_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/reg_d_alu_is_reg_b_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/reg_d_alu_is_reg_c_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/reg_d_ex_is_reg_a_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/reg_d_ex_is_reg_b_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/reg_d_ex_is_reg_c_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/reg_d_wb_is_reg_a_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/reg_d_wb_is_reg_b_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/reg_d_wb_is_reg_c_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/rega_used_dec
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regb_used_dec
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regc_mux
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regc_used_dec
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_addr_ra_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_addr_rb_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_addr_rc_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_alu_waddr_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_alu_waddr_fw_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_alu_waddr_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_alu_waddr_mux_sel
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_alu_wdata_fw_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_alu_we_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_alu_we_fw_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_alu_we_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_data_ra_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_data_rb_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_data_rc_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_fp_a
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_fp_b
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_fp_c
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_fp_d
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_waddr_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_waddr_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_waddr_wb_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_wdata_wb_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_we_ex_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_we_id
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/regfile_we_wb_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/rst_n
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/scalar_replication
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/SHARED_DSP_MULT
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/SHARED_FP
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/SHARED_FP_DIVSQRT
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/SHARED_INT_DIV
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/test_en_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/trap_addr_mux_o
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/u_irq_enable_i
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/uret_insn_dec
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/WAPUTYPE
add wave -noupdate -group fc_id /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/wb_ready_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/alu_cmp_result
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/alu_en_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/alu_operand_a_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/alu_operand_b_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/alu_operand_c_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/alu_operator_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/alu_ready
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/alu_result
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/alu_vec_mode_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_active
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_busy_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_en_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_flags_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_gnt
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_lat_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_master_gnt_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_master_op_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_master_operands_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_master_ready_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_master_req_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_master_result_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_master_valid_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_multicycle
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_op_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_operands_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_perf_cont_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_perf_type_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_perf_wb_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_read_dep_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_read_regs_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_read_regs_valid_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_ready
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_ready_wb_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_req
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_result
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_singlecycle
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_stall
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_valid
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_waddr
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_waddr_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_write_dep_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_write_regs_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/apu_write_regs_valid_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/bmask_a_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/bmask_b_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/branch_decision_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/branch_in_ex_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/clk
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/csr_access_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/csr_rdata_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/ex_ready_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/ex_valid_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/FPU
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/fpu_busy
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/fpu_fflags_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/fpu_fflags_we_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/fpu_op_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/fpu_prec_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/imm_vec_ext_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/jump_target_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/lsu_en_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/lsu_rdata_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/lsu_ready_ex_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/mult_cplx_shift_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/mult_dot_op_a_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/mult_dot_op_b_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/mult_dot_op_c_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/mult_dot_signed_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/mult_en_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/mult_imm_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/mult_multicycle_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/mult_operand_a_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/mult_operand_b_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/mult_operand_c_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/mult_operator_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/mult_ready
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/mult_result
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/mult_sel_subword_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/mult_signed_mode_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/regfile_alu_waddr_fw_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/regfile_alu_waddr_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/regfile_alu_wdata_fw_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/regfile_alu_we_fw_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/regfile_alu_we_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/regfile_waddr_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/regfile_waddr_lsu
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/regfile_waddr_wb_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/regfile_wdata_wb_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/regfile_we_i
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/regfile_we_lsu
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/regfile_we_wb_o
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/rst_n
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/SHARED_DSP_MULT
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/SHARED_FP
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/SHARED_INT_DIV
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/wb_contention
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/wb_contention_lsu
add wave -noupdate -group fc_ex /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/ex_stage_i/wb_ready_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/APU
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/apu_contention_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/apu_dep_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/apu_typeconflict_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/apu_wb_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/boot_addr_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/branch_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/branch_taken_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/clk
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/cluster_id_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/core_id_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/csr_access_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/csr_addr_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/csr_cause_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/csr_irq_sec_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/csr_op_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/csr_rdata_int
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/csr_rdata_o
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/csr_restore_mret_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/csr_restore_uret_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/csr_save_cause_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/csr_save_id_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/csr_save_if_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/csr_wdata_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/csr_wdata_int
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/csr_we_int
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/epc_o
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/exception_pc
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/ext_counters_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/fflags_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/fflags_n
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/fflags_q
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/fflags_we_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/fprec_n
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/fprec_o
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/fprec_q
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/FPU
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/frm_n
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/frm_o
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/frm_q
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/hwlp_cnt_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/hwlp_data_o
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/hwlp_end_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/hwlp_regid_o
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/hwlp_start_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/hwlp_we_o
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/id_valid_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/id_valid_q
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/imiss_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/is_compressed_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/is_decoding_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/is_irq
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/is_pccr
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/is_pcer
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/is_pcmr
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/jr_stall_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/jump_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/ld_stall_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/m_irq_enable_o
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/mcause_n
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/mcause_q
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/mem_load_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/mem_store_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/mepc_n
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/mepc_q
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/mstatus_n
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/mstatus_q
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/mtvec_n
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/mtvec_o
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/mtvec_q
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/mtvec_reg_q
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/N_APU_CNT
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/N_EXT_CNT
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/N_HWLP
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/N_HWLP_BITS
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/N_PERF_COUNTERS
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/N_PERF_REGS
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/pc_id_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/pc_if_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/pc_set_i
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/pccr_all_sel
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/PCCR_in
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/PCCR_inc
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/PCCR_inc_q
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/pccr_index
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/PCCR_n
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/PCCR_q
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/PCER_n
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/PCER_q
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/PCMR_n
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/PCMR_q
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/PERF_APU_ID
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/PERF_EXT_ID
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/perf_rdata
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/priv_lvl_n
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/priv_lvl_o
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/priv_lvl_q
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/priv_lvl_reg_q
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/PULP_SECURE
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/rst_n
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/sec_lvl_o
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/u_irq_enable_o
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/ucause_n
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/ucause_q
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/uepc_n
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/uepc_q
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/utvec_n
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/utvec_o
add wave -noupdate -group cs_register /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/cs_registers_i/utvec_q
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/apu_en_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/apu_read_dep_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/apu_stall_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/apu_write_dep_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/boot_done
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/boot_done_q
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/branch_in_id
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/branch_taken_ex_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/clk
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/csr_cause_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/csr_irq_sec_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/csr_restore_mret_id_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/csr_restore_uret_id_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/csr_save_cause_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/csr_save_id_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/csr_save_if_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/csr_status_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/ctrl_busy_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/ctrl_fsm_cs
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/ctrl_fsm_ns
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/current_priv_lvl_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/data_load_event_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/data_misaligned_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/data_req_ex_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/dbg_ack_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/dbg_jump_req_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/dbg_req_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/dbg_settings_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/dbg_stall_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/dbg_trap_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/deassert_we_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/ebrk_insn_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/ecall_insn_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/ex_valid_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/exc_ack_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/exc_cause_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/exc_kill_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/exc_pc_mux_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/fetch_enable_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/first_fetch_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/FPU
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/halt_id_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/halt_if_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/id_ready_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/illegal_insn_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/instr_req_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/instr_valid_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/irq_ack_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/irq_enable_int
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/irq_id_ctrl_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/irq_id_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/irq_req_ctrl_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/irq_sec_ctrl_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/is_decoding_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/jr_stall_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/jump_done
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/jump_done_q
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/jump_in_dec
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/jump_in_dec_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/jump_in_id_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/load_stall_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/m_IE_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/misaligned_stall_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/mret_insn_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/mult_multicycle_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/operand_a_fw_mux_sel_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/operand_b_fw_mux_sel_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/operand_c_fw_mux_sel_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/pc_mux_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/pc_set_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/perf_jr_stall_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/perf_jump_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/perf_ld_stall_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/pipe_flush_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/reg_d_alu_is_reg_a_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/reg_d_alu_is_reg_b_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/reg_d_alu_is_reg_c_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/reg_d_ex_is_reg_a_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/reg_d_ex_is_reg_b_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/reg_d_ex_is_reg_c_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/reg_d_wb_is_reg_a_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/reg_d_wb_is_reg_b_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/reg_d_wb_is_reg_c_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/regfile_alu_we_fw_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/regfile_we_ex_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/regfile_we_wb_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/rst_n
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/trap_addr_mux_o
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/u_IE_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/uret_insn_i
add wave -noupdate -expand -group fc_id_control /tb/pulp_chip_iface_wrap_i/i_dut/soc_domain_i/ulpsoc_i/fc_subsystem_i/FC_CORE/id_stage_i/controller_i/wb_ready_i
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {27461251914 ps} 0}
quietly wave cursor active 1
configure wave -namecolwidth 217
configure wave -valuecolwidth 117
configure wave -justifyvalue left
configure wave -signalnamewidth 1
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ns
update
WaveRestoreZoom {27460578258 ps} {27461925570 ps}
