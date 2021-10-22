set(GPIO_PREFIX "pulp/gpio")

vp_model(NAME gpio_v2_impl
    PREFIX ${GPIO_PREFIX}
    SOURCES "gpio_v2_impl.cpp"
    )

vp_model(NAME gpio_v3_impl
    PREFIX ${GPIO_PREFIX}
    SOURCES "gpio_v3_impl.cpp"
    )
