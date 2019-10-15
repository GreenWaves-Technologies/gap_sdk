
####################
# UNIT TESTS
####################

set(unittest-sources
  ../features/netsocket/SocketAddress.cpp
  ../features/netsocket/NetworkStack.cpp
  ../features/netsocket/InternetSocket.cpp
  ../features/netsocket/TCPSocket.cpp
  ../features/netsocket/TLSSocket.cpp
  ../features/netsocket/TLSSocketWrapper.cpp
  ../features/frameworks/nanostack-libservice/source/libip4string/ip4tos.c
  ../features/frameworks/nanostack-libservice/source/libip6string/ip6tos.c
  ../features/frameworks/nanostack-libservice/source/libip4string/stoip4.c
  ../features/frameworks/nanostack-libservice/source/libip6string/stoip6.c
  ../features/frameworks/nanostack-libservice/source/libBits/common_functions.c  
)

set(unittest-test-sources
  features/netsocket/TLSSocket/test_TLSSocket.cpp
  stubs/Mutex_stub.cpp
  stubs/mbed_assert_stub.c
  stubs/mbed_critical_stub.c
  stubs/equeue_stub.c
  ../features/nanostack/coap-service/test/coap-service/unittest/stub/mbedtls_stub.c
  stubs/EventQueue_stub.cpp
  stubs/mbed_shared_queues_stub.cpp
  stubs/nsapi_dns_stub.cpp
  stubs/EventFlags_stub.cpp
  stubs/stoip4_stub.c
  stubs/ip4tos_stub.c
  stubs/SocketStats_Stub.cpp
)

set(MBEDTLS_USER_CONFIG_FILE_PATH "\"../UNITTESTS/features/netsocket/TLSSocket/tls_test_config.h\"")
set_source_files_properties(features/netsocket/TLSSocket/test_TLSSocket.cpp PROPERTIES COMPILE_DEFINITIONS MBEDTLS_USER_CONFIG_FILE=${MBEDTLS_USER_CONFIG_FILE_PATH})
set_source_files_properties(../features/netsocket/TLSSocket.cpp PROPERTIES COMPILE_DEFINITIONS MBEDTLS_USER_CONFIG_FILE=${MBEDTLS_USER_CONFIG_FILE_PATH})
set_source_files_properties(../features/netsocket/TLSSocketWrapper.cpp PROPERTIES COMPILE_DEFINITIONS MBEDTLS_USER_CONFIG_FILE=${MBEDTLS_USER_CONFIG_FILE_PATH})

