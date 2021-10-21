vp_model(NAME i2c_bus
    PREFIX "devices/i2c"
    SOURCES "i2c_bus.cpp"
    )

add_subdirectory(helper)
add_subdirectory(corruptor)
add_subdirectory(eeprom)

