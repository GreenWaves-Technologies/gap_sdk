
####################
# UNIT TESTS
####################

# Add test specific include paths
set(unittest-includes ${unittest-includes}
    /features/cellular/framework/device/cellularstatemachine
  ../features/cellular/framework/device
  ../features/cellular/framework/common
  ../features/netsocket/cellular
)

# Source files
set(unittest-sources
  ../features/cellular/framework/device/CellularStateMachine.cpp
  ../features/cellular/framework/AT/ATHandler_factory.cpp
)

# Test files
set(unittest-test-sources
  features/cellular/framework/device/cellularstatemachine/cellularstatemachinetest.cpp
  stubs/FileHandle_stub.cpp
  stubs/CellularDevice_stub.cpp
  stubs/EventQueue_stub.cpp
  stubs/mbed_assert_stub.c
  stubs/UARTSerial_stub.cpp
  stubs/SerialBase_stub.cpp
  stubs/ATHandler_stub.cpp
  stubs/AT_CellularNetwork_stub.cpp
  stubs/AT_CellularBase_stub.cpp
  stubs/AT_CellularContext_stub.cpp
  stubs/AT_CellularDevice_stub.cpp
  stubs/Semaphore_stub.cpp
  stubs/NetworkInterface_stub.cpp
  stubs/NetworkInterfaceDefaults_stub.cpp
  stubs/Thread_stub.cpp
  stubs/Mutex_stub.cpp
  stubs/EventQueue_stub.cpp
  stubs/equeue_stub.c
)

# defines
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMDMRTS=PTC0 -DMDMCTS=PTC1 -DMDMTXD=NC -DMDMRXD=NC -DMBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE=115200 -DCELLULAR_DEVICE=myCellularDevice -DDEVICE_SERIAL_FC=1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMDMRTS=PTC0 -DMDMCTS=PTC1 -DMDMTXD=NC -DMDMRXD=NC -DMBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE=115200 -DCELLULAR_DEVICE=myCellularDevice -DDEVICE_SERIAL_FC=1")
