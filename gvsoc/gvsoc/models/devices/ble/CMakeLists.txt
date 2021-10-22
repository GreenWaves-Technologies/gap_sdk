set(NINA_B112_SRCS "nina_b112.cpp"
    "nina_b112_cmd_parser.cpp")

vp_model(NAME nina_b112
    PREFIX "devices/ble"
    SOURCES ${NINA_B112_SRCS}
    )
