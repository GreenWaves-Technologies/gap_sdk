# Change Log

## [v4.7.4](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.7.4) 

- Remove dependency to yotta tool
- Do not remove stored (GET) blockwise message when EMPTY ACK received
    When non piggybacked response mode is used original GET request must not be removed from the stored message list.
    Message is needed for building the next (GET) blockwise message.
- Move definitions to sn_config.h

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.7.3...v4.7.4)

## [v4.7.3](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.7.3) 

- Do not store EMPTY response to blockwise list
    An Empty message only contains the 4-byte header so it does not require any blockwise operations.
    This will fix unneseccary message sending timeouts which leads mbed cloud client to do unnecessary
    reconnections which increases the network traffic.

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.7.2...v4.7.3)

## [v4.7.2](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.7.2) 

- Fix handling of duplicate blockwise ACK's
    CoAP data buffer was not added into duplication info store when creating response for blockwise request.
    This leads to case where whole bootstrap flow just timeouts if received any duplicate messages during blockwise operation.
    Fixes error: IOTCLT-3188 - UDP connection fails for lost ACK sending

- Remove error trace when building reset message without options
    This makes it possible to build the reset message without allocating option or getting error message.

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.7.1...v4.7.2)

## [v4.7.1](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.7.1) 

- Fix CoAP stored blockwise message release and list continue
	Add re-scan routine goto if message is caused user callback
	This will fix hard fault when blockwise message sending timeouts. This happens cause same list is manipulated through rx callback.

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.7.0...v4.7.1)

## [v4.7.0](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.7.0) 

- Add function that can be used to clear the received blockwise payloads for example in the case of a connection error.
- Silence compiler warning when CoAP duplicate detection is enabled.

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.6.3...v4.7.0)

## [v4.6.3](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.6.3) 

 - Bug fix: Remove timed out blockwise message from resend queue. If blockwise message was timed out message was still kept in the resend queue which causes unnecessary reconnections on client side.
 - Documentation: Document all the available macros.

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.6.2...v4.6.3)

## [v4.6.2](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.6.2) 

Do not clear block2 in subsequent block request.

When sending a request with block2 option, eg. indicating need
for response to be blockwised, copy the block2 option from the
sent_blockwise list item so that the block2 option will be added
to all requests. This fixes an issue where previously the block2
was only sent for the first blockwise request and not for the
subsequent ones, including the last request. This made the response
not follow the request block2 option.

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.6.1...v4.6.2)

## [v4.6.1](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.6.1) 
**Closed issues:**
-  IOTCLT-2900 - Blockwise handling leaking memory in some error cases

Fix unused parameter compiler warning when blockwise is not used.

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.6.0...v4.6.1)

## [v4.6.0](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.6.0) 
**New feature:**
-  Add new API which clears one item from the resend queue based on token

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.5.1...v4.6.0)

## [v4.5.1](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.5.1) 
**Closed issues:**
 - IOTCLT-2883 - Blockwise observations not completing

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.5.0...v4.5.1)

## [v4.5.0](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.5.0) 
**Closed issues:**
 - IIOTCLT-2769 - mbed-coap: extra response received after registration

Added own flag to enable blockwise support, without setting default blockwise
payload size. This allows to receive blockwise messages while still sending
without blockwise.

Fix CoAP request blockwise response handling
When request is sent, response can have blockwise option set. All requests must
be stored to the linked list.

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.4.4...v4.5.0)

## [v4.4.4](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.4.4) 
**Closed issues:**
 - IOTCLT-2638 [GitHub] hardfault during reconnection retry with Thread

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.4.3...v4.4.4)

## [v4.4.3](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.4.3) 
**Closed issues:**
 - IOTCLT-2506 [GitHub] Cannot set registration time if server does not use max age option

Extend blockwise message transfer status to have states for sending as well.

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.4.2...v4.4.3)

## [v4.4.2](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.4.2) 
**Closed issues:**
 - IOTCLT-2469 CoAP UDP retransmission does not work for blocks after first one for requests (Eg. registration POST)

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.4.1...v4.4.2)

## [v4.4.1](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.4.1) 
**Closed issues:**
 - IOTCLT-2539 Block wise messaging call-backs not working logically

 Improve TCP+TLS transport layer to allow send larger messages without blockwising.

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.4.0...v4.4.1)

## [v4.4.0](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.4.0) 
**New feature:**
- Make sn_coap_protocol_send_rst as public needed for CoAP ping sending
- Allow disabling resendings by defining SN_COAP_DISABLE_RESENDINGS

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.3.0...v4.4.0)

## [v4.3.0](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.3.0) 
**New feature:**
-  Add new API which clears the whole sent blockwise message list

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.2.0...v4.3.0)

## [v4.2.0](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.2.0) 
**New feature:**
-  Add new API to remove sent blockwise message from the linked list

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.1.1...v4.2.0)

## [v4.1.1](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.1.1) 
**Closed issues:**
-  IOTCLT-2203 mbed-coap does not handle PUT or POST if they indicate a smaller block size preference (fixed regression) 

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.1.0...v4.1.1)

## [v4.1.0](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.1.0) 

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.0.10...v4.1.0)

 **New feature:**
  - New API to disable automatic GET(BLOCK2) request sending.

 **Closed issues:**
  -  IOTCLT-2203 mbed-coap does not handle PUT or POST if they indicate a smaller block size preference 

## [v4.0.10](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.0.10) 

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.0.9...v4.0.10)

 **Closed issues:**
  -  IOTMAC-615 Node mDS registration failure during OTA transfer

## [v4.0.9](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.0.9) 

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.0.8...v4.0.9)

 **Closed issues:**
  -  IOTCLT-1899 Maximum COAP message resending buffer size limited to 255 bytes
  -  IOTCLT-1888 Problem with blockwise transfers that are even increments of block_size

## [v4.0.8](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.0.8) 

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.0.4...v4.0.8)

 **Closed issues:**
  -  IOTCLT-1880 Lwm2m blockwise data transfer (using Block1 option) with Leshan not working
  -  IOTCLT-1885 Return 4.08 Request Entity Incomplete on Block transfer errors
  -  IOTCLT-1883 Detected message duplications stop mbed-client

## [v4.0.4](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.0.4) 

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.0.3...v4.0.4)

 **Closed issues:**
	- https://github.com/ARMmbed/mbed-client/issues/481 - Obs Con blockwise fails to transmit 2nd block

## [v4.0.3](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.0.3)

-[Full Changelog](https://github.com/ARMmbed/mbed-coap/compare/v4.0.2...v4.0.3)

 **New feature**
 		
 	- CoAP Protocol Confirmable resend fix and minor memory optimization (IOTMAC-328)
 		
 **Closed issues:**
 		
 	- IOTCLT-1439 - stuck in while loop

## [v4.0.0](https://github.com/ARMmbed/mbed-coap/releases/tag/v4.0.2)

**New feature**

 - Initial release of mbed-coap separated from mbed-client-c
