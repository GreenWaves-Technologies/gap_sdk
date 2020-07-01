#ifndef __BRIDGE_STUBS_H__
#define __BRIDGE_STUBS_H__

#if defined(__PULP_OS__)

#define BRIDGE_Init() {}
#define BRIDGE_Connect(__wait_bridge, __event)          rt_bridge_connect(__wait_bridge, __event)
#define BRIDGE_Open(__name, __flags, __mode, __event)   rt_bridge_open(__name, __flags, __mode, __event)
#define BRIDGE_Read(__file, __buffer, __size, __event)  rt_bridge_read(__file, __buffer, __size, __event)
#define BRIDGE_Write(__file, __buffer, __size, __event) rt_bridge_write(__file, __buffer, __size, __event)
#define BRIDGE_Close(__file, __event)                   rt_bridge_close(__file, __event)
#define BRIDGE_Disconnect(__event)                      rt_bridge_disconnect(__event)

#endif  /* __PULP_OS__ */

#endif  /* __BRIDGE_STUBS_H__ */
