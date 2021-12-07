ifeq '$(CONFIG_OMP_ENABLED)' '1'
ifneq '$(event_unit/version)' '1'
PULP_LIB_CL_ASM_SRCS_omp   += libs/omp/omp_asm.S
PULP_LIB_CL_SRCS_omp   += libs/omp/omp_wrapper_gcc.c
PULP_LIBS += omp
endif
endif

ifeq '$(CONFIG_LIB_IO_ENABLED)' '1'
PULP_LIB_FC_SRCS_rtio   += libs/io/io.c libs/io/semihost.c libs/io/fprintf.c libs/io/prf.c libs/io/sprintf.c
PULP_LIBS += rtio
endif

ifeq '$(CONFIG_LIB_BENCH_ENABLED)' '1'
PULP_LIB_FC_SRCS_bench   += libs/bench/bench.c
PULP_LIBS += bench
endif

