vp_model(NAME i2c_corruptor
    PREFIX "devices/i2c/corruptor"
    SOURCES "i2c_corruptor.cpp"
    )

vp_model_link_libraries(NAME i2c_corruptor LIBRARY i2c_helper)
