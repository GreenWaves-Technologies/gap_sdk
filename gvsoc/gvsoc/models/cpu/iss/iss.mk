COMMON_SRCS = $(GVSOC_ISS_PATH)/vp/src/iss_wrapper.cpp $(GVSOC_ISS_PATH)/src/iss.cpp \
	$(GVSOC_ISS_PATH)/src/insn_cache.cpp $(GVSOC_ISS_PATH)/src/csr.cpp \
	$(GVSOC_ISS_PATH)/src/decoder.cpp $(GVSOC_ISS_PATH)/src/trace.cpp \
	$(GVSOC_ISS_PATH)/src/resource.c \
	$(GVSOC_ISS_PATH)/flexfloat/flexfloat.c

COMMON_CFLAGS = -DRISCV=1 -DRISCY -I$(GVSOC_ISS_PATH)/include -I$(GVSOC_ISS_PATH)/vp/include -I$(GVSOC_ISS_PATH)/flexfloat -mtune=generic -fno-strict-aliasing

ifdef USE_TRDB
COMMON_CFLAGS += -DUSE_TRDB=1
COMMON_LDFLAGS = -ltrdb -lbfd -lopcodes -liberty -lz
endif


define declare_iss_isa_build

$(VP_BUILD_DIR)/$(1)_decoder_gen.hpp: $(GVSOC_ISS_PATH)/isa_gen/isa_generator.py $(GVSOC_ISS_PATH)/isa_gen/isa_riscv_gen.py $(GVSOC_ISS_PATH)/isa_gen/isa_gen.py $(3)
	$(GVSOC_ISS_PATH)/isa_gen/isa_generator.py --source-file=$(VP_BUILD_DIR)/$(1)_decoder_gen.cpp --header-file=$(VP_BUILD_DIR)/$(1)_decoder_gen.hpp $(2)

$(VP_BUILD_DIR)/$(1)_decoder_gen.cpp: $(GVSOC_ISS_PATH)/isa_gen/isa_generator.py $(GVSOC_ISS_PATH)/isa_gen/isa_riscv_gen.py $(GVSOC_ISS_PATH)/isa_gen/isa_gen.py $(3)
	$(GVSOC_ISS_PATH)/isa_gen/isa_generator.py --source-file=$(VP_BUILD_DIR)/$(1)_decoder_gen.cpp --header-file=$(VP_BUILD_DIR)/$(1)_decoder_gen.hpp $(2)

endef
