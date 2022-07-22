#define CODE_REPEAT_4(insn) insn; insn; insn; insn;
#define CODE_REPEAT_3(insn) insn; insn; insn;
#define CODE_REPEAT_16(insn) CODE_REPEAT_4(CODE_REPEAT_4(insn))
#define CODE_REPEAT_64(insn) CODE_REPEAT_4(CODE_REPEAT_16(insn))
#define CODE_REPEAT_256(insn) CODE_REPEAT_4(CODE_REPEAT_64(insn))
#define CODE_REPEAT_1024(insn) CODE_REPEAT_4(CODE_REPEAT_256(insn))


#define JMP_BLOCK_256(id)         \
.option norvc;                    \
.global jmp_block_256_##id;       \
.align 16;                        \
jmp_block_256_##id:               \
    CODE_REPEAT_16(nop)           \
    CODE_REPEAT_16(nop)           \
    CODE_REPEAT_16(nop)           \
                                  \
    CODE_REPEAT_4(nop)            \
    CODE_REPEAT_4(nop)            \
    CODE_REPEAT_4(nop)            \
    addi  a0, a0, -1;             \
    bnez  a0, jmp_block_256_##id; \
    nop;                          \
    jr ra;

JMP_BLOCK_256(0)

.global jmp_block_512_0
.align 17
jmp_block_512_0:
    CODE_REPEAT_64(nop)

    CODE_REPEAT_16(nop)
    CODE_REPEAT_16(nop)
    CODE_REPEAT_16(nop)

    CODE_REPEAT_4(nop)
    CODE_REPEAT_4(nop)
    CODE_REPEAT_4(nop)
    addi  a0, a0, -1
    bnez  a0, jmp_block_512_0
    nop
    jr ra


#define JMP_BLOCK_1024(id)         \
.global jmp_block_1024_##id;       \
.align 17;                         \
jmp_block_1024_##id:               \
    CODE_REPEAT_64(nop)            \
    CODE_REPEAT_64(nop)            \
                                   \
    CODE_REPEAT_64(nop)            \
                                   \
    CODE_REPEAT_16(nop)            \
    CODE_REPEAT_16(nop)            \
    CODE_REPEAT_16(nop)            \
                                   \
    CODE_REPEAT_4(nop)             \
    CODE_REPEAT_4(nop)             \
    CODE_REPEAT_4(nop)             \
    addi  a0, a0, -1;              \
    bnez  a0, jmp_block_1024_##id; \
    nop;                           \
    jr ra

JMP_BLOCK_1024(0)


#define JMP_BLOCK_2048(id)         \
.option norvc;                     \
.global jmp_block_2048_##id;       \
.align 16;                         \
jmp_block_2048_##id:               \
    CODE_REPEAT_256(nop)           \
                                   \
    CODE_REPEAT_64(nop)            \
    CODE_REPEAT_64(nop)            \
                                   \
    CODE_REPEAT_64(nop)            \
                                   \
    CODE_REPEAT_16(nop)            \
    CODE_REPEAT_16(nop)            \
    CODE_REPEAT_16(nop)            \
                                   \
    CODE_REPEAT_4(nop)             \
    CODE_REPEAT_4(nop)             \
    CODE_REPEAT_4(nop)             \
    addi  a0, a0, -1;              \
    bnez  a0, jmp_block_2048_##id; \
    nop;                           \
    jr ra;

JMP_BLOCK_2048(0)
JMP_BLOCK_2048(1)
JMP_BLOCK_2048(2)
JMP_BLOCK_2048(3)


.global jmp_block_4096_0
.align 17
jmp_block_4096_0:
    CODE_REPEAT_256(nop)
    CODE_REPEAT_256(nop)

    CODE_REPEAT_256(nop)

    CODE_REPEAT_64(nop)
    CODE_REPEAT_64(nop)

    CODE_REPEAT_64(nop)

    CODE_REPEAT_16(nop)
    CODE_REPEAT_16(nop)
    CODE_REPEAT_16(nop)

    CODE_REPEAT_4(nop)
    CODE_REPEAT_4(nop)
    CODE_REPEAT_4(nop)
    addi  a0, a0, -1
    bnez  a0, jmp_block_4096_0
    nop
    jr ra


.global jmp_block_8192_0
.align 17
jmp_block_8192_0:
    CODE_REPEAT_1024(nop)

    CODE_REPEAT_256(nop)
    CODE_REPEAT_256(nop)
    CODE_REPEAT_256(nop)

    CODE_REPEAT_64(nop)
    CODE_REPEAT_64(nop)

    CODE_REPEAT_64(nop)

    CODE_REPEAT_16(nop)
    CODE_REPEAT_16(nop)
    CODE_REPEAT_16(nop)

    CODE_REPEAT_4(nop)
    CODE_REPEAT_4(nop)
    CODE_REPEAT_4(nop)
    addi  a0, a0, -1
    bnez  a0, jmp_block_8192_0
    nop
    jr ra



