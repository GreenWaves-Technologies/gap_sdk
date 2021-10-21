vp_model(NAME i2c_eeprom
    PREFIX "devices/i2c/eeprom"
    SOURCES "i2c_eeprom.cpp"
    )

vp_model_link_libraries(NAME i2c_eeprom LIBRARY i2c_helper)
