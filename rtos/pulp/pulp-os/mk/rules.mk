
#
# HEADER RULES
#

define declareInstallFile

$(TARGET_INSTALL_DIR)/$(1): $(1)
	install -D $(1) $$@

INSTALL_HEADERS += $(TARGET_INSTALL_DIR)/$(1)

endef


define declareWsInstallFile

$(INSTALL_DIR)/$(1): $(1)
	install -D $(1) $$@

WS_INSTALL_HEADERS += $(INSTALL_DIR)/$(1)

endef


$(foreach file, $(INSTALL_FILES), $(eval $(call declareInstallFile,$(file))))

$(foreach file, $(WS_INSTALL_FILES), $(eval $(call declareWsInstallFile,$(file))))


#
# CC RULES for domain: cluster
#

PULP_LIB_NAME_rt ?= rt

PULP_CL_EXTRA_SRCS_rt = 
PULP_CL_EXTRA_ASM_SRCS_rt = 
PULP_CL_EXTRA_OMP_SRCS_rt = 

rt_CL_OBJS =     $(patsubst %.cpp,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rt)/cl/%.o, $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rt)/cl/%.o, $(PULP_LIB_CL_SRCS_rt) $(PULP_CL_SRCS_rt) $(PULP_LIB_CL_SRCS) $(PULP_APP_SRCS) $(PULP_CL_EXTRA_SRCS_rt)))
rt_CL_ASM_OBJS = $(patsubst %.S,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rt)/cl/%.o, $(PULP_LIB_CL_ASM_SRCS_rt) $(PULP_CL_ASM_SRCS_rt) $(PULP_LIB_CL_ASM_SRCS) $(PULP_APP_ASM_SRCS) $(PULP_CL_EXTRA_ASM_SRCS_rt))
rt_CL_OMP_OBJS = $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rt)/cl/omp/%.o, $(PULP_LIB_CL_OMP_SRCS_rt) $(PULP_CL_OMP_SRCS_rt) $(PULP_LIB_CL_OMP_SRCS) $(PULP_APP_OMP_SRCS) $(PULP_CL_EXTRA_OMP_SRCS_rt))

ifneq '$(rt_CL_OMP_OBJS)' ''
PULP_LDFLAGS_rt += $(PULP_OMP_LDFLAGS_rt)
endif

-include $(rt_CL_OBJS:.o=.d)
-include $(rt_CL_ASM_OBJS:.o=.d)
-include $(rt_CL_OMP_OBJS:.o=.d)

rt_cl_cflags = $(PULP_CL_ARCH_CFLAGS) $(PULP_CFLAGS) $(PULP_CL_CFLAGS) $(PULP_CFLAGS_rt) $(PULP_CL_CFLAGS_rt) $(PULP_LIB_CFLAGS_rt)
rt_cl_omp_cflags = $(rt_cl_cflags) $(PULP_OMP_CFLAGS) $(PULP_CL_OMP_CFLAGS) $(PULP_OMP_CFLAGS_rt) $(PULP_CL_OMP_CFLAGS_rt)

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rt)/cl/%.o: %.c
	@mkdir -p `dirname $@`
	$(PULP_CL_CC) $(rt_cl_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rt)/cl/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(PULP_CL_CC) $(rt_cl_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rt)/cl/%.o: %.S
	@mkdir -p `dirname $@`
	$(PULP_CL_CC) $(rt_cl_cflags) -DLANGUAGE_ASSEMBLY -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rt)/cl/omp/%.o: %.c
	@mkdir -p `dirname $@`
	touch libgomp.spec
	$(PULP_CL_CC) $(rt_cl_omp_cflags) -MMD -MP -c $< -o $@

rt_OBJS += $(rt_CL_OBJS) $(rt_CL_ASM_OBJS) $(rt_CL_OMP_OBJS)



#
# CC RULES for domain: fabric_controller
#

PULP_LIB_NAME_rt ?= rt

PULP_FC_EXTRA_SRCS_rt = 
PULP_FC_EXTRA_ASM_SRCS_rt = 
PULP_FC_EXTRA_OMP_SRCS_rt = 

rt_FC_OBJS =     $(patsubst %.cpp,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rt)/fc/%.o, $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rt)/fc/%.o, $(PULP_LIB_FC_SRCS_rt) $(PULP_FC_SRCS_rt) $(PULP_LIB_FC_SRCS)  $(PULP_FC_EXTRA_SRCS_rt)))
rt_FC_ASM_OBJS = $(patsubst %.S,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rt)/fc/%.o, $(PULP_LIB_FC_ASM_SRCS_rt) $(PULP_FC_ASM_SRCS_rt) $(PULP_LIB_FC_ASM_SRCS)  $(PULP_FC_EXTRA_ASM_SRCS_rt))
rt_FC_OMP_OBJS = $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rt)/fc/omp/%.o, $(PULP_LIB_FC_OMP_SRCS_rt) $(PULP_FC_OMP_SRCS_rt) $(PULP_LIB_FC_OMP_SRCS)  $(PULP_FC_EXTRA_OMP_SRCS_rt))

ifneq '$(rt_FC_OMP_OBJS)' ''
PULP_LDFLAGS_rt += $(PULP_OMP_LDFLAGS_rt)
endif

-include $(rt_FC_OBJS:.o=.d)
-include $(rt_FC_ASM_OBJS:.o=.d)
-include $(rt_FC_OMP_OBJS:.o=.d)

rt_fc_cflags = $(PULP_FC_ARCH_CFLAGS) $(PULP_CFLAGS) $(PULP_FC_CFLAGS) $(PULP_CFLAGS_rt) $(PULP_FC_CFLAGS_rt) $(PULP_LIB_CFLAGS_rt)
rt_fc_omp_cflags = $(rt_fc_cflags) $(PULP_OMP_CFLAGS) $(PULP_FC_OMP_CFLAGS) $(PULP_OMP_CFLAGS_rt) $(PULP_FC_OMP_CFLAGS_rt)

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rt)/fc/%.o: %.c
	@mkdir -p `dirname $@`
	$(PULP_FC_CC) $(rt_fc_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rt)/fc/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(PULP_FC_CC) $(rt_fc_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rt)/fc/%.o: %.S
	@mkdir -p `dirname $@`
	$(PULP_FC_CC) $(rt_fc_cflags) -DLANGUAGE_ASSEMBLY -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rt)/fc/omp/%.o: %.c
	@mkdir -p `dirname $@`
	touch libgomp.spec
	$(PULP_FC_CC) $(rt_fc_omp_cflags) -MMD -MP -c $< -o $@

rt_OBJS += $(rt_FC_OBJS) $(rt_FC_ASM_OBJS) $(rt_FC_OMP_OBJS)



#
# AR RULES for library: rt
#

PULP_LIB_NAME_rt ?= rt

$(CONFIG_BUILD_DIR)/lib$(PULP_LIB_NAME_rt).a: $(rt_OBJS)
	@mkdir -p `dirname $@`
	@rm -f $@
	$(PULP_AR) -r $@ $^

$(TARGET_INSTALL_DIR)/lib/${pulp_chip}/lib$(PULP_LIB_NAME_rt).a: $(CONFIG_BUILD_DIR)/lib$(PULP_LIB_NAME_rt).a
	@mkdir -p `dirname $@`
	cp $^ $@ 


TARGETS += $(CONFIG_BUILD_DIR)/lib$(PULP_LIB_NAME_rt).a
CLEAN_TARGETS += $(CONFIG_BUILD_DIR)/lib$(PULP_LIB_NAME_rt).a $($(PULP_LIB_NAME_rt)_OBJS)
INSTALL_TARGETS += $(TARGET_INSTALL_DIR)/lib/${pulp_chip}/lib$(PULP_LIB_NAME_rt).a



#
# CC RULES for domain: cluster
#

PULP_LIB_NAME_omp ?= gomp

PULP_CL_EXTRA_SRCS_omp = 
PULP_CL_EXTRA_ASM_SRCS_omp = 
PULP_CL_EXTRA_OMP_SRCS_omp = 

omp_CL_OBJS = $(patsubst %.cpp,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_omp)/cl/%.o, $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_omp)/cl/%.o, $(PULP_LIB_CL_SRCS_omp) $(PULP_CL_SRCS_omp) $(PULP_LIB_CL_SRCS) $(PULP_APP_SRCS) $(PULP_CL_EXTRA_SRCS_omp)))
omp_CL_ASM_OBJS = $(patsubst %.S,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_omp)/cl/%.o, $(PULP_LIB_CL_ASM_SRCS_omp) $(PULP_CL_ASM_SRCS_omp) $(PULP_LIB_CL_ASM_SRCS) $(PULP_APP_ASM_SRCS) $(PULP_CL_EXTRA_ASM_SRCS_omp))
omp_CL_OMP_OBJS = $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_omp)/cl/omp/%.o, $(PULP_LIB_CL_OMP_SRCS_omp) $(PULP_CL_OMP_SRCS_omp) $(PULP_LIB_CL_OMP_SRCS) $(PULP_APP_OMP_SRCS) $(PULP_CL_EXTRA_OMP_SRCS_omp))

ifneq '$(omp_CL_OMP_OBJS)' ''
PULP_LDFLAGS_omp += $(PULP_OMP_LDFLAGS_omp)
endif

-include $(omp_CL_OBJS:.o=.d)
-include $(omp_CL_ASM_OBJS:.o=.d)
-include $(omp_CL_OMP_OBJS:.o=.d)

omp_cl_cflags = $(PULP_CL_ARCH_CFLAGS) $(PULP_CFLAGS) $(PULP_CL_CFLAGS) $(PULP_CFLAGS_omp) $(PULP_CL_CFLAGS_omp) $(PULP_LIB_CFLAGS_omp)
omp_cl_omp_cflags = $(omp_cl_cflags) $(PULP_OMP_CFLAGS) $(PULP_CL_OMP_CFLAGS) $(PULP_OMP_CFLAGS_omp) $(PULP_CL_OMP_CFLAGS_omp)

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_omp)/cl/%.o: %.c
	@mkdir -p `dirname $@`
	$(PULP_CL_CC) $(omp_cl_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_omp)/cl/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(PULP_CL_CC) $(omp_cl_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_omp)/cl/%.o: %.S
	@mkdir -p `dirname $@`
	$(PULP_CL_CC) $(omp_cl_cflags) -DLANGUAGE_ASSEMBLY -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_omp)/cl/omp/%.o: %.c
	@mkdir -p `dirname $@`
	touch libgomp.spec
	$(PULP_CL_CC) $(omp_cl_omp_cflags) -MMD -MP -c $< -o $@

omp_OBJS += $(omp_CL_OBJS) $(omp_CL_ASM_OBJS) $(omp_CL_OMP_OBJS)



#
# CC RULES for domain: fabric_controller
#

PULP_LIB_NAME_omp ?= omp

PULP_FC_EXTRA_SRCS_omp = 
PULP_FC_EXTRA_ASM_SRCS_omp = 
PULP_FC_EXTRA_OMP_SRCS_omp = 

omp_FC_OBJS =     $(patsubst %.cpp,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_omp)/fc/%.o, $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_omp)/fc/%.o, $(PULP_LIB_FC_SRCS_omp) $(PULP_FC_SRCS_omp) $(PULP_LIB_FC_SRCS)  $(PULP_FC_EXTRA_SRCS_omp)))
omp_FC_ASM_OBJS = $(patsubst %.S,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_omp)/fc/%.o, $(PULP_LIB_FC_ASM_SRCS_omp) $(PULP_FC_ASM_SRCS_omp) $(PULP_LIB_FC_ASM_SRCS)  $(PULP_FC_EXTRA_ASM_SRCS_omp))
omp_FC_OMP_OBJS = $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_omp)/fc/omp/%.o, $(PULP_LIB_FC_OMP_SRCS_omp) $(PULP_FC_OMP_SRCS_omp) $(PULP_LIB_FC_OMP_SRCS)  $(PULP_FC_EXTRA_OMP_SRCS_omp))

ifneq '$(omp_FC_OMP_OBJS)' ''
PULP_LDFLAGS_omp += $(PULP_OMP_LDFLAGS_omp)
endif

-include $(omp_FC_OBJS:.o=.d)
-include $(omp_FC_ASM_OBJS:.o=.d)
-include $(omp_FC_OMP_OBJS:.o=.d)

omp_fc_cflags = $(PULP_FC_ARCH_CFLAGS) $(PULP_CFLAGS) $(PULP_FC_CFLAGS) $(PULP_CFLAGS_omp) $(PULP_FC_CFLAGS_omp) $(PULP_LIB_CFLAGS_omp)
omp_fc_omp_cflags = $(omp_fc_cflags) $(PULP_OMP_CFLAGS) $(PULP_FC_OMP_CFLAGS) $(PULP_OMP_CFLAGS_omp) $(PULP_FC_OMP_CFLAGS_omp)

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_omp)/fc/%.o: %.c
	@mkdir -p `dirname $@`
	$(PULP_FC_CC) $(omp_fc_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_omp)/fc/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(PULP_FC_CC) $(omp_fc_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_omp)/fc/%.o: %.S
	@mkdir -p `dirname $@`
	$(PULP_FC_CC) $(omp_fc_cflags) -DLANGUAGE_ASSEMBLY -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_omp)/fc/omp/%.o: %.c
	@mkdir -p `dirname $@`
	touch libgomp.spec
	$(PULP_FC_CC) $(omp_fc_omp_cflags) -MMD -MP -c $< -o $@

omp_OBJS += $(omp_FC_OBJS) $(omp_FC_ASM_OBJS) $(omp_FC_OMP_OBJS)



#
# AR RULES for library: omp
#

PULP_LIB_NAME_omp ?= omp

$(CONFIG_BUILD_DIR)/lib$(PULP_LIB_NAME_omp).a: $(omp_OBJS)
	@mkdir -p `dirname $@`
	@rm -f $@
	$(PULP_AR) -r $@ $^

$(TARGET_INSTALL_DIR)/lib/${pulp_chip}/lib$(PULP_LIB_NAME_omp).a: $(CONFIG_BUILD_DIR)/lib$(PULP_LIB_NAME_omp).a
	@mkdir -p `dirname $@`
	cp $^ $@ 


TARGETS += $(CONFIG_BUILD_DIR)/lib$(PULP_LIB_NAME_omp).a
CLEAN_TARGETS += $(CONFIG_BUILD_DIR)/lib$(PULP_LIB_NAME_omp).a $($(PULP_LIB_NAME_omp)_OBJS)
INSTALL_TARGETS += $(TARGET_INSTALL_DIR)/lib/${pulp_chip}/lib$(PULP_LIB_NAME_omp).a



#
# CC RULES for domain: cluster
#

PULP_LIB_NAME_rtio ?= rtio

PULP_CL_EXTRA_SRCS_rtio = 
PULP_CL_EXTRA_ASM_SRCS_rtio = 
PULP_CL_EXTRA_OMP_SRCS_rtio = 

rtio_CL_OBJS =     $(patsubst %.cpp,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rtio)/cl/%.o, $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rtio)/cl/%.o, $(PULP_LIB_CL_SRCS_rtio) $(PULP_CL_SRCS_rtio) $(PULP_LIB_CL_SRCS) $(PULP_APP_SRCS) $(PULP_CL_EXTRA_SRCS_rtio)))
rtio_CL_ASM_OBJS = $(patsubst %.S,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rtio)/cl/%.o, $(PULP_LIB_CL_ASM_SRCS_rtio) $(PULP_CL_ASM_SRCS_rtio) $(PULP_LIB_CL_ASM_SRCS) $(PULP_APP_ASM_SRCS) $(PULP_CL_EXTRA_ASM_SRCS_rtio))
rtio_CL_OMP_OBJS = $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rtio)/cl/omp/%.o, $(PULP_LIB_CL_OMP_SRCS_rtio) $(PULP_CL_OMP_SRCS_rtio) $(PULP_LIB_CL_OMP_SRCS) $(PULP_APP_OMP_SRCS) $(PULP_CL_EXTRA_OMP_SRCS_rtio))

ifneq '$(rtio_CL_OMP_OBJS)' ''
PULP_LDFLAGS_rtio += $(PULP_OMP_LDFLAGS_rtio)
endif

-include $(rtio_CL_OBJS:.o=.d)
-include $(rtio_CL_ASM_OBJS:.o=.d)
-include $(rtio_CL_OMP_OBJS:.o=.d)

rtio_cl_cflags = $(PULP_CL_ARCH_CFLAGS) $(PULP_CFLAGS) $(PULP_CL_CFLAGS) $(PULP_CFLAGS_rtio) $(PULP_CL_CFLAGS_rtio) $(PULP_LIB_CFLAGS_rtio)
rtio_cl_omp_cflags = $(rtio_cl_cflags) $(PULP_OMP_CFLAGS) $(PULP_CL_OMP_CFLAGS) $(PULP_OMP_CFLAGS_rtio) $(PULP_CL_OMP_CFLAGS_rtio)

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rtio)/cl/%.o: %.c
	@mkdir -p `dirname $@`
	$(PULP_CL_CC) $(rtio_cl_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rtio)/cl/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(PULP_CL_CC) $(rtio_cl_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rtio)/cl/%.o: %.S
	@mkdir -p `dirname $@`
	$(PULP_CL_CC) $(rtio_cl_cflags) -DLANGUAGE_ASSEMBLY -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rtio)/cl/omp/%.o: %.c
	@mkdir -p `dirname $@`
	touch libgomp.spec
	$(PULP_CL_CC) $(rtio_cl_omp_cflags) -MMD -MP -c $< -o $@

rtio_OBJS += $(rtio_CL_OBJS) $(rtio_CL_ASM_OBJS) $(rtio_CL_OMP_OBJS)



#
# CC RULES for domain: fabric_controller
#

PULP_LIB_NAME_rtio ?= rtio

PULP_FC_EXTRA_SRCS_rtio = 
PULP_FC_EXTRA_ASM_SRCS_rtio = 
PULP_FC_EXTRA_OMP_SRCS_rtio = 

rtio_FC_OBJS =     $(patsubst %.cpp,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rtio)/fc/%.o, $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rtio)/fc/%.o, $(PULP_LIB_FC_SRCS_rtio) $(PULP_FC_SRCS_rtio) $(PULP_LIB_FC_SRCS)  $(PULP_FC_EXTRA_SRCS_rtio)))
rtio_FC_ASM_OBJS = $(patsubst %.S,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rtio)/fc/%.o, $(PULP_LIB_FC_ASM_SRCS_rtio) $(PULP_FC_ASM_SRCS_rtio) $(PULP_LIB_FC_ASM_SRCS)  $(PULP_FC_EXTRA_ASM_SRCS_rtio))
rtio_FC_OMP_OBJS = $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rtio)/fc/omp/%.o, $(PULP_LIB_FC_OMP_SRCS_rtio) $(PULP_FC_OMP_SRCS_rtio) $(PULP_LIB_FC_OMP_SRCS)  $(PULP_FC_EXTRA_OMP_SRCS_rtio))

ifneq '$(rtio_FC_OMP_OBJS)' ''
PULP_LDFLAGS_rtio += $(PULP_OMP_LDFLAGS_rtio)
endif

-include $(rtio_FC_OBJS:.o=.d)
-include $(rtio_FC_ASM_OBJS:.o=.d)
-include $(rtio_FC_OMP_OBJS:.o=.d)

rtio_fc_cflags = $(PULP_FC_ARCH_CFLAGS) $(PULP_CFLAGS) $(PULP_FC_CFLAGS) $(PULP_CFLAGS_rtio) $(PULP_FC_CFLAGS_rtio) $(PULP_LIB_CFLAGS_rtio)
rtio_fc_omp_cflags = $(rtio_fc_cflags) $(PULP_OMP_CFLAGS) $(PULP_FC_OMP_CFLAGS) $(PULP_OMP_CFLAGS_rtio) $(PULP_FC_OMP_CFLAGS_rtio)

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rtio)/fc/%.o: %.c
	@mkdir -p `dirname $@`
	$(PULP_FC_CC) $(rtio_fc_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rtio)/fc/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(PULP_FC_CC) $(rtio_fc_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rtio)/fc/%.o: %.S
	@mkdir -p `dirname $@`
	$(PULP_FC_CC) $(rtio_fc_cflags) -DLANGUAGE_ASSEMBLY -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rtio)/fc/omp/%.o: %.c
	@mkdir -p `dirname $@`
	touch libgomp.spec
	$(PULP_FC_CC) $(rtio_fc_omp_cflags) -MMD -MP -c $< -o $@

rtio_OBJS += $(rtio_FC_OBJS) $(rtio_FC_ASM_OBJS) $(rtio_FC_OMP_OBJS)



#
# AR RULES for library: rtio
#

PULP_LIB_NAME_rtio ?= rtio

$(CONFIG_BUILD_DIR)/lib$(PULP_LIB_NAME_rtio).a: $(rtio_OBJS)
	@mkdir -p `dirname $@`
	@rm -f $@
	$(PULP_AR) -r $@ $^

$(TARGET_INSTALL_DIR)/lib/${pulp_chip}/lib$(PULP_LIB_NAME_rtio).a: $(CONFIG_BUILD_DIR)/lib$(PULP_LIB_NAME_rtio).a
	@mkdir -p `dirname $@`
	cp $^ $@ 


TARGETS += $(CONFIG_BUILD_DIR)/lib$(PULP_LIB_NAME_rtio).a
CLEAN_TARGETS += $(CONFIG_BUILD_DIR)/lib$(PULP_LIB_NAME_rtio).a $($(PULP_LIB_NAME_rtio)_OBJS)
INSTALL_TARGETS += $(TARGET_INSTALL_DIR)/lib/${pulp_chip}/lib$(PULP_LIB_NAME_rtio).a



#
# CC RULES for domain: cluster
#

PULP_LIB_NAME_bench ?= bench

PULP_CL_EXTRA_SRCS_bench = 
PULP_CL_EXTRA_ASM_SRCS_bench = 
PULP_CL_EXTRA_OMP_SRCS_bench = 

bench_CL_OBJS =     $(patsubst %.cpp,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_bench)/cl/%.o, $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_bench)/cl/%.o, $(PULP_LIB_CL_SRCS_bench) $(PULP_CL_SRCS_bench) $(PULP_LIB_CL_SRCS) $(PULP_APP_SRCS) $(PULP_CL_EXTRA_SRCS_bench)))
bench_CL_ASM_OBJS = $(patsubst %.S,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_bench)/cl/%.o, $(PULP_LIB_CL_ASM_SRCS_bench) $(PULP_CL_ASM_SRCS_bench) $(PULP_LIB_CL_ASM_SRCS) $(PULP_APP_ASM_SRCS) $(PULP_CL_EXTRA_ASM_SRCS_bench))
bench_CL_OMP_OBJS = $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_bench)/cl/omp/%.o, $(PULP_LIB_CL_OMP_SRCS_bench) $(PULP_CL_OMP_SRCS_bench) $(PULP_LIB_CL_OMP_SRCS) $(PULP_APP_OMP_SRCS) $(PULP_CL_EXTRA_OMP_SRCS_bench))

ifneq '$(bench_CL_OMP_OBJS)' ''
PULP_LDFLAGS_bench += $(PULP_OMP_LDFLAGS_bench)
endif

-include $(bench_CL_OBJS:.o=.d)
-include $(bench_CL_ASM_OBJS:.o=.d)
-include $(bench_CL_OMP_OBJS:.o=.d)

bench_cl_cflags = $(PULP_CL_ARCH_CFLAGS) $(PULP_CFLAGS) $(PULP_CL_CFLAGS) $(PULP_CFLAGS_bench) $(PULP_CL_CFLAGS_bench) $(PULP_LIB_CFLAGS_bench)
bench_cl_omp_cflags = $(bench_cl_cflags) $(PULP_OMP_CFLAGS) $(PULP_CL_OMP_CFLAGS) $(PULP_OMP_CFLAGS_bench) $(PULP_CL_OMP_CFLAGS_bench)

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_bench)/cl/%.o: %.c
	@mkdir -p `dirname $@`
	$(PULP_CL_CC) $(bench_cl_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_bench)/cl/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(PULP_CL_CC) $(bench_cl_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_bench)/cl/%.o: %.S
	@mkdir -p `dirname $@`
	$(PULP_CL_CC) $(bench_cl_cflags) -DLANGUAGE_ASSEMBLY -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_bench)/cl/omp/%.o: %.c
	@mkdir -p `dirname $@`
	touch libgomp.spec
	$(PULP_CL_CC) $(bench_cl_omp_cflags) -MMD -MP -c $< -o $@

bench_OBJS += $(bench_CL_OBJS) $(bench_CL_ASM_OBJS) $(bench_CL_OMP_OBJS)



#
# CC RULES for domain: fabric_controller
#

PULP_LIB_NAME_bench ?= bench

PULP_FC_EXTRA_SRCS_bench = 
PULP_FC_EXTRA_ASM_SRCS_bench = 
PULP_FC_EXTRA_OMP_SRCS_bench = 

bench_FC_OBJS =     $(patsubst %.cpp,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_bench)/fc/%.o, $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_bench)/fc/%.o, $(PULP_LIB_FC_SRCS_bench) $(PULP_FC_SRCS_bench) $(PULP_LIB_FC_SRCS)  $(PULP_FC_EXTRA_SRCS_bench)))
bench_FC_ASM_OBJS = $(patsubst %.S,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_bench)/fc/%.o, $(PULP_LIB_FC_ASM_SRCS_bench) $(PULP_FC_ASM_SRCS_bench) $(PULP_LIB_FC_ASM_SRCS)  $(PULP_FC_EXTRA_ASM_SRCS_bench))
bench_FC_OMP_OBJS = $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_bench)/fc/omp/%.o, $(PULP_LIB_FC_OMP_SRCS_bench) $(PULP_FC_OMP_SRCS_bench) $(PULP_LIB_FC_OMP_SRCS)  $(PULP_FC_EXTRA_OMP_SRCS_bench))

ifneq '$(bench_FC_OMP_OBJS)' ''
PULP_LDFLAGS_bench += $(PULP_OMP_LDFLAGS_bench)
endif

-include $(bench_FC_OBJS:.o=.d)
-include $(bench_FC_ASM_OBJS:.o=.d)
-include $(bench_FC_OMP_OBJS:.o=.d)

bench_fc_cflags = $(PULP_FC_ARCH_CFLAGS) $(PULP_CFLAGS) $(PULP_FC_CFLAGS) $(PULP_CFLAGS_bench) $(PULP_FC_CFLAGS_bench) $(PULP_LIB_CFLAGS_bench)
bench_fc_omp_cflags = $(bench_fc_cflags) $(PULP_OMP_CFLAGS) $(PULP_FC_OMP_CFLAGS) $(PULP_OMP_CFLAGS_bench) $(PULP_FC_OMP_CFLAGS_bench)

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_bench)/fc/%.o: %.c
	@mkdir -p `dirname $@`
	$(PULP_FC_CC) $(bench_fc_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_bench)/fc/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(PULP_FC_CC) $(bench_fc_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_bench)/fc/%.o: %.S
	@mkdir -p `dirname $@`
	$(PULP_FC_CC) $(bench_fc_cflags) -DLANGUAGE_ASSEMBLY -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_bench)/fc/omp/%.o: %.c
	@mkdir -p `dirname $@`
	touch libgomp.spec
	$(PULP_FC_CC) $(bench_fc_omp_cflags) -MMD -MP -c $< -o $@

bench_OBJS += $(bench_FC_OBJS) $(bench_FC_ASM_OBJS) $(bench_FC_OMP_OBJS)



#
# AR RULES for library: bench
#

PULP_LIB_NAME_bench ?= bench

$(CONFIG_BUILD_DIR)/lib$(PULP_LIB_NAME_bench).a: $(bench_OBJS)
	@mkdir -p `dirname $@`
	@rm -f $@
	$(PULP_AR) -r $@ $^

$(TARGET_INSTALL_DIR)/lib/${pulp_chip}/lib$(PULP_LIB_NAME_bench).a: $(CONFIG_BUILD_DIR)/lib$(PULP_LIB_NAME_bench).a
	@mkdir -p `dirname $@`
	cp $^ $@ 


TARGETS += $(CONFIG_BUILD_DIR)/lib$(PULP_LIB_NAME_bench).a
CLEAN_TARGETS += $(CONFIG_BUILD_DIR)/lib$(PULP_LIB_NAME_bench).a $($(PULP_LIB_NAME_bench)_OBJS)
INSTALL_TARGETS += $(TARGET_INSTALL_DIR)/lib/${pulp_chip}/lib$(PULP_LIB_NAME_bench).a


header:: $(INSTALL_HEADERS) $(WS_INSTALL_HEADERS)

fullclean::
	rm -rf $(CONFIG_BUILD_DIR)

clean:: $(GEN_TARGETS)
	rm -rf $(CLEAN_TARGETS)

prepare:: $(GEN_TARGETS) $(CONFIG_BUILD_DIR)/config.mk
	pulp-run $(pulpRunOpt) prepare

runner:: $(GEN_TARGETS) $(CONFIG_BUILD_DIR)/config.mk
	pulp-run $(pulpRunOpt) $(RUNNER_CMD)

power:: $(GEN_TARGETS) $(CONFIG_BUILD_DIR)/config.mk
	pulp-run $(pulpRunOpt) power

gen: $(GEN_TARGETS_FORCE)

build:: $(GEN_TARGETS) $(TARGETS)

all:: build prepare

install:: $(INSTALL_TARGETS)

run::
	pulp-run $(pulpRunOpt)

.PHONY: clean header prepare all install run build
