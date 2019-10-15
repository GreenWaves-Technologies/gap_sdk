ifeq '$(gpio/version)' '2'
IMPLEMENTATIONS += pulp/gpio/gpio_v2_impl
COMPONENTS += pulp/gpio/gpio_v2
pulp/gpio/gpio_v2_impl_SRCS = pulp/gpio/gpio_v2_impl.cpp
endif

ifeq '$(gpio/version)' '3'
IMPLEMENTATIONS += pulp/gpio/gpio_v3_impl
COMPONENTS += pulp/gpio/gpio_v3
pulp/gpio/gpio_v3_impl_SRCS = pulp/gpio/gpio_v3_impl.cpp
endif
