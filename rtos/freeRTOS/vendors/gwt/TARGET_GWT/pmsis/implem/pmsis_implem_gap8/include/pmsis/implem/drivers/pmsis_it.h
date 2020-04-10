#ifndef __PMSIS_IT__
#define __PMSIS_IT__

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Machine mode IRQ handler wrapper */
#define HANDLER_WRAPPER(f)                                                     \
__attribute__((section(".text")))                                              \
static inline void __handler_wrapper_##f (void)                                \
{                                                                              \
    asm volatile( "addi sp, sp, -18*4\n\t"                                     \
                  "sw   ra, 15*4(sp)\n\t"                                      \
                  "jal  Exception_handler\n\t");                               \
    asm volatile("jalr  %0" :: "r" (f));                                       \
    asm volatile("j    SVC_Context");                                          \
}

/* Machine mode IRQ handler wrapper light version */
#define HANDLER_WRAPPER_LIGHT(f)                                    \
__attribute__((section(".text")))                                   \
static inline void __handler_wrapper_light_##f (void)               \
{                                                                   \
    asm volatile("addi sp, sp, -0x58\t\n"                           \
            "sw  x1, 0x00(sp)\t\n"                                  \
            "sw  x3, 0x04(sp)\t\n"                                  \
            "sw  x4, 0x08(sp)\t\n"                                  \
            "sw  x5, 0x0c(sp)\t\n"                                  \
            "sw  x6, 0x10(sp)\t\n"                                  \
            "sw  x7, 0x14(sp)\t\n"                                  \
            "sw x10, 0x18(sp)\t\n"                                  \
            "sw x11, 0x1c(sp)\t\n"                                  \
            "sw x12, 0x20(sp)\t\n"                                  \
            "sw x13, 0x24(sp)\t\n"                                  \
            "sw x14, 0x28(sp)\t\n"                                  \
            "sw x15, 0x2c(sp)\t\n"                                  \
            "sw x16, 0x30(sp)\t\n"                                  \
            "sw x17, 0x34(sp)\t\n"                                  \
            "csrr a0, 0x7B0\t\n"                                    \
            "csrr a1, 0x7B1\t\n"                                    \
            "csrr a2, 0x7B2\t\n"                                    \
            "sw a0, 0x38(sp)\t\n"                                   \
            "sw a1, 0x3c(sp)\t\n"                                   \
            "sw a2, 0x40(sp)\t\n"                                   \
            "csrr a0, 0x7B4\t\n"                                    \
            "csrr a1, 0x7B5\t\n"                                    \
            "csrr a2, 0x7B6\t\n"                                    \
            "sw a0, 0x44(sp)\t\n"                                   \
            "sw a1, 0x48(sp)\t\n"                                   \
            "sw a2, 0x4c(sp)\t\n"                                   \
            "csrr a0, mepc\t\n"                                     \
            "sw a0, 0x50(sp)\t\n"                                   \
            "csrr a0, mstatus\t\n"                                  \
            "sw a0, 0x54(sp)\t\n");                                 \
    asm volatile("jalr  %0" :: "r" (f));                            \
    asm volatile("lw a0, 0x54(sp)\t\n"                              \
            "csrrw x0, mstatus, a0\t\n"                             \
            "lw a0, 0x50(sp)\t\n"                                   \
            "csrrw x0, mepc, a0\t\n"                                \
            "lw a0, 0x44(sp)\t\n"                                   \
            "lw a1, 0x48(sp)\t\n"                                   \
            "lw a2, 0x4c(sp)\t\n"                                   \
            "csrrw x0, 0x7B4, a0\t\n"                               \
            "csrrw x0, 0x7B5, a1\t\n"                               \
            "csrrw x0, 0x7B6, a2\t\n"                               \
            "lw a0, 0x38(sp)\t\n"                                   \
            "lw a1, 0x3c(sp)\t\n"                                   \
            "lw a2, 0x40(sp)\t\n"                                   \
            "csrrw x0, 0x7B0, a0\t\n"                               \
            "csrrw x0, 0x7B1, a1\t\n"                               \
            "csrrw x0, 0x7B2, a2\t\n"                               \
            "lw  x1, 0x00(sp)\t\n"                                  \
            "lw  x3, 0x04(sp)\t\n"                                  \
            "lw  x4, 0x08(sp)\t\n"                                  \
            "lw  x5, 0x0c(sp)\t\n"                                  \
            "lw  x6, 0x10(sp)\t\n"                                  \
            "lw  x7, 0x14(sp)\t\n"                                  \
            "lw x10, 0x18(sp)\t\n"                                  \
            "lw x11, 0x1c(sp)\t\n"                                  \
            "lw x12, 0x20(sp)\t\n"                                  \
            "lw x13, 0x24(sp)\t\n"                                  \
            "lw x14, 0x28(sp)\t\n"                                  \
            "lw x15, 0x2c(sp)\t\n"                                  \
            "lw x16, 0x30(sp)\t\n"                                  \
            "lw x17, 0x34(sp)\t\n"                                  \
            "addi sp, sp, 0x58\t\n"                                 \
            "mret\n\t");                                            \
}
#endif
