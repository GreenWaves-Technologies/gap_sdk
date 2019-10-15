
####################
# UNIT TESTS
####################

# Add test specific include paths
set(unittest-includes ${unittest-includes}
  ../features/cellular/framework/common/util
  ../features/cellular/framework/common
  ../features/cellular/framework/AT
  ../features/cellular/framework/device
  ../features/frameworks/mbed-client-randlib/mbed-client-randlib
  ../drivers
  ../hal
  ../features/netsocket/cellular
)

# Source files
set(unittest-sources
  stubs/randLIB_stub.c
  ../features/cellular/framework/AT/AT_CellularDevice.cpp
  ../features/cellular/framework/AT/ATHandler_factory.cpp
)

# Test files
set(unittest-test-sources
  features/cellular/framework/AT/at_cellulardevice/at_cellulardevicetest.cpp
  stubs/AT_CellularNetwork_stub.cpp
  stubs/ATHandler_stub.cpp
  stubs/AT_CellularSMS_stub.cpp
  stubs/AT_CellularInformation_stub.cpp
  stubs/CellularUtil_stub.cpp
  stubs/AT_CellularBase_stub.cpp
  stubs/NetworkInterface_stub.cpp
  stubs/NetworkInterfaceDefaults_stub.cpp
  stubs/EventQueue_stub.cpp
  stubs/FileHandle_stub.cpp
  stubs/mbed_assert_stub.c
  stubs/CellularDevice_stub.cpp
  stubs/NetworkStack_stub.cpp
  stubs/AT_CellularContext_stub.cpp
  stubs/Semaphore_stub.cpp
  stubs/UARTSerial_stub.cpp
  stubs/SerialBase_stub.cpp
  stubs/CellularStateMachine_stub.cpp
)

# defines
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEVICE_SERIAL=1 -DDEVICE_INTERRUPTIN=1 -DMBED_CONF_NSAPI_DEFAULT_CELLULAR_APN=NULL -DMBED_CONF_NSAPI_DEFAULT_CELLULAR_USERNAME=NULL -DMBED_CONF_NSAPI_DEFAULT_CELLULAR_PASSWORD=NULL -DMBED_CONF_NSAPI_DEFAULT_CELLULAR_PLMN=NULL -DMBED_CONF_NSAPI_DEFAULT_CELLULAR_SIM_PIN=NULL")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMDMTXD=NC -DMDMRXD=NC -DMBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE=115200")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEVICE_SERIAL=1 -DDEVICE_INTERRUPTIN=1 -DMBED_CONF_NSAPI_DEFAULT_CELLULAR_APN=NULL -DMBED_CONF_NSAPI_DEFAULT_CELLULAR_USERNAME=NULL -DMBED_CONF_NSAPI_DEFAULT_CELLULAR_PASSWORD=NULL -DMBED_CONF_NSAPI_DEFAULT_CELLULAR_PLMN=NULL -DMBED_CONF_NSAPI_DEFAULT_CELLULAR_SIM_PIN=NULL")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMDMTXD=NC -DMDMRXD=NC -DMBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE=115200")

