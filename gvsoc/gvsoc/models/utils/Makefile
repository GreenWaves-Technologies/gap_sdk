IMPLEMENTATIONS += utils/loader_impl
utils/loader_impl_SRCS = utils/loader_impl.cpp

ifeq '$(GVSOC_SKIP_DPI)' ''
IMPLEMENTATIONS += utils/dpi_wrapper_impl
utils/dpi_wrapper_impl_SRCS = utils/dpi_wrapper_impl.cpp
utils/dpi_wrapper_impl_LDFLAGS += -lpulpdpi
endif

IMPLEMENTATIONS += utils/injector_impl
utils/injector_impl_SRCS = utils/injector_impl.cpp

IMPLEMENTATIONS += utils/composite_impl
utils/composite_impl_SRCS = utils/composite_impl.cpp

IMPLEMENTATIONS += utils/clock_impl
utils/clock_impl_SRCS = utils/clock_impl.cpp

IMPLEMENTATIONS += utils/dpi_chip_wrapper
utils/dpi_chip_wrapper_SRCS = utils/dpi_chip_wrapper.cpp
