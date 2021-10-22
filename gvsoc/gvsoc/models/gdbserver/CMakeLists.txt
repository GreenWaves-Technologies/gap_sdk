set(GDB_SERVER_SRCS "gdbserver.cpp"
    "rsp.cpp"
    "rsp-packet-codec.cpp"
    "circular-buffer.cpp")

vp_model(NAME gdbserver
    PREFIX "gdbserver"
    SOURCES ${GDB_SERVER_SRCS}
    )
