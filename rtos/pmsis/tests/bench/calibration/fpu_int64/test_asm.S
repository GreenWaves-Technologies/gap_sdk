#define INSN_0_3ARGS(name, insn)        \
.global name##_0;                       \
name##_0:                               \
    lp.setup x1, a0, name##_0_hwloop_end; \
    insn x11, x12, x13                ; \
    insn x11, x12, x13                ; \
    insn x11, x12, x13                ; \
    insn x11, x12, x13                ; \
    insn x11, x12, x13                ; \
    insn x11, x12, x13                ; \
    insn x11, x12, x13                ; \
    insn x11, x12, x13                ; \
    insn x11, x12, x13                ; \
name##_0_hwloop_end:                    \
    insn x11, x12, x13                ; \
    jr    ra;

#define INSN_1_3ARGS(name, insn)        \
.global name##_1;                       \
name##_1:                               \
    lp.setup x1, a0, name##_1_hwloop_end; \
    insn x11, x12, x11                ; \
    insn x11, x12, x11                ; \
    insn x11, x12, x11                ; \
    insn x11, x12, x11                ; \
    insn x11, x12, x11                ; \
    insn x11, x12, x11                ; \
    insn x11, x12, x11                ; \
    insn x11, x12, x11                ; \
    insn x11, x12, x11                ; \
name##_1_hwloop_end:                    \
    insn x11, x12, x11                ; \
    jr    ra;

#define INSN_2_3ARGS(name, insn)        \
.global name##_2;                       \
name##_2:                               \
    lp.setup x1, a0, name##_2_hwloop_end; \
    insn x11, x12, x11                ; \
    nop                               ; \
    insn x11, x12, x11                ; \
    nop                               ; \
    insn x11, x12, x11                ; \
    nop                               ; \
    insn x11, x12, x11                ; \
    nop                               ; \
    insn x11, x12, x11                ; \
name##_2_hwloop_end:                    \
    nop                               ; \
    jr    ra;


#define INSN_0_4ARGS(name, insn)        \
.global name##_0;                       \
name##_0:                               \
    lp.setup x1, a0, name##_0_hwloop_end; \
    insn x11, x12, x13, x14           ; \
    insn x11, x12, x13, x14           ; \
    insn x11, x12, x13, x14           ; \
    insn x11, x12, x13, x14           ; \
    insn x11, x12, x13, x14           ; \
    insn x11, x12, x13, x14           ; \
    insn x11, x12, x13, x14           ; \
    insn x11, x12, x13, x14           ; \
    insn x11, x12, x13, x14           ; \
name##_0_hwloop_end:                    \
    insn x11, x12, x13, x14           ; \
    jr    ra;

#define INSN_1_4ARGS(name, insn)        \
.global name##_1;                       \
name##_1:                               \
    lp.setup x1, a0, name##_1_hwloop_end; \
    insn x11, x12, x11, x14           ; \
    insn x11, x12, x11, x14           ; \
    insn x11, x12, x11, x14           ; \
    insn x11, x12, x11, x14           ; \
    insn x11, x12, x11, x14           ; \
    insn x11, x12, x11, x14           ; \
    insn x11, x12, x11, x14           ; \
    insn x11, x12, x11, x14           ; \
    insn x11, x12, x11, x14           ; \
name##_1_hwloop_end:                    \
    insn x11, x12, x11, x14           ; \
    jr    ra;

#define INSN_2_4ARGS(name, insn)        \
.global name##_2;                       \
name##_2:                               \
    lp.setup x1, a0, name##_2_hwloop_end; \
    insn x11, x12, x11, x14           ; \
    nop                               ; \
    insn x11, x12, x11, x14           ; \
    nop                               ; \
    insn x11, x12, x11, x14           ; \
    nop                               ; \
    insn x11, x12, x11, x14           ; \
    nop                               ; \
    insn x11, x12, x11, x14           ; \
name##_2_hwloop_end:                    \
    nop                               ; \
    jr    ra;



#define INSN_0_2ARGS(name, insn)        \
.global name##_0;                       \
name##_0:                               \
    lp.setup x1, a0, name##_0_hwloop_end; \
    insn x11, x12                     ; \
    insn x11, x12                     ; \
    insn x11, x12                     ; \
    insn x11, x12                     ; \
    insn x11, x12                     ; \
    insn x11, x12                     ; \
    insn x11, x12                     ; \
    insn x11, x12                     ; \
    insn x11, x12                     ; \
name##_0_hwloop_end:                    \
    insn x11, x12                     ; \
    jr    ra;

#define INSN_1_2ARGS(name, insn)        \
.global name##_1;                       \
name##_1:                               \
    lp.setup x1, a0, name##_1_hwloop_end; \
    insn x11, x12                     ; \
    insn x11, x12                     ; \
    insn x11, x12                     ; \
    insn x11, x12                     ; \
    insn x11, x12                     ; \
    insn x11, x12                     ; \
    insn x11, x12                     ; \
    insn x11, x12                     ; \
    insn x11, x12                     ; \
name##_1_hwloop_end:                    \
    insn x11, x12                     ; \
    jr    ra;

#define INSN_2_2ARGS(name, insn)        \
.global name##_2;                       \
name##_2:                               \
    lp.setup x1, a0, name##_2_hwloop_end; \
    insn x11, x12                     ; \
    nop                               ; \
    insn x11, x12                     ; \
    nop                               ; \
    insn x11, x12                     ; \
    nop                               ; \
    insn x11, x12                     ; \
    nop                               ; \
    insn x11, x12                     ; \
name##_2_hwloop_end:                    \
    nop                               ; \
    jr    ra;


#define INT64_INSN_0_3ARGS(name, insn)        \
.global name##_0;                       \
name##_0:                               \
    lp.setup x1, a0, name##_0_hwloop_end; \
    insn x12, x14, x16                ; \
    insn x12, x14, x16                ; \
    insn x12, x14, x16                ; \
    insn x12, x14, x16                ; \
    insn x12, x14, x16                ; \
    insn x12, x14, x16                ; \
    insn x12, x14, x16                ; \
    insn x12, x14, x16                ; \
    insn x12, x14, x16                ; \
name##_0_hwloop_end:                    \
    insn x12, x14, x16                ; \
    jr    ra;

#define INT64_INSN_1_3ARGS(name, insn)        \
.global name##_1;                       \
name##_1:                               \
    lp.setup x1, a0, name##_1_hwloop_end; \
    insn x12,  x14, x12                ; \
    insn x12,  x14, x12                ; \
    insn x12,  x14, x12                ; \
    insn x12,  x14, x12                ; \
    insn x12,  x14, x12                ; \
    insn x12,  x14, x12                ; \
    insn x12,  x14, x12                ; \
    insn x12,  x14, x12                ; \
    insn x12,  x14, x12                ; \
name##_1_hwloop_end:                    \
    insn x12,  x14, x12                ; \
    jr    ra;

#define INT64_INSN_2_3ARGS(name, insn)        \
.global name##_2;                       \
name##_2:                               \
    lp.setup x1, a0, name##_2_hwloop_end; \
    insn x12,  x14, x12                ; \
    nop                               ; \
    insn x12,  x14, x12                ; \
    nop                               ; \
    insn x12,  x14, x12                ; \
    nop                               ; \
    insn x12,  x14, x12                ; \
    nop                               ; \
    insn x12,  x14, x12                ; \
name##_2_hwloop_end:                    \
    nop                               ; \
    jr    ra;

#define INT64_INSN_0_2ARGS(name, insn)        \
.global name##_0;                       \
name##_0:                               \
    lp.setup x1, a0, name##_0_hwloop_end; \
    insn x12, x14                ; \
    insn x12, x14                ; \
    insn x12, x14                ; \
    insn x12, x14                ; \
    insn x12, x14                ; \
    insn x12, x14                ; \
    insn x12, x14                ; \
    insn x12, x14                ; \
    insn x12, x14                ; \
name##_0_hwloop_end:                    \
    insn x12, x14                ; \
    jr    ra;

#define INT64_INSN_1_2ARGS(name, insn)        \
.global name##_1;                       \
name##_1:                               \
    lp.setup x1, a0, name##_1_hwloop_end; \
    insn x12,  x14                ; \
    insn x12,  x14                ; \
    insn x12,  x14                ; \
    insn x12,  x14                ; \
    insn x12,  x14                ; \
    insn x12,  x14                ; \
    insn x12,  x14                ; \
    insn x12,  x14                ; \
    insn x12,  x14                ; \
name##_1_hwloop_end:                    \
    insn x12,  x14                ; \
    jr    ra;

#define INT64_INSN_2_2ARGS(name, insn)        \
.global name##_2;                       \
name##_2:                               \
    lp.setup x1, a0, name##_2_hwloop_end; \
    insn x12,  x14                ; \
    nop                               ; \
    insn x12,  x14                ; \
    nop                               ; \
    insn x12,  x14                ; \
    nop                             ; \
    insn x12,  x14                ; \
    nop                               ; \
    insn x12,  x14                ; \
name##_2_hwloop_end:                    \
    nop                               ; \
    jr    ra;



INSN_0_3ARGS(fadd, fadd.s)
INSN_1_3ARGS(fadd, fadd.s)
INSN_2_3ARGS(fadd, fadd.s)

INSN_0_3ARGS(fmul, fmul.s)
INSN_1_3ARGS(fmul, fmul.s)
INSN_2_3ARGS(fmul, fmul.s)

INSN_0_3ARGS(fsgnj, fsgnj.s)
INSN_1_3ARGS(fsgnj, fsgnj.s)
INSN_2_3ARGS(fsgnj, fsgnj.s)

INSN_0_3ARGS(fmin, fmin.s)
INSN_1_3ARGS(fmin, fmin.s)
INSN_2_3ARGS(fmin, fmin.s)

INSN_0_3ARGS(fdiv, fdiv.s)
INSN_1_3ARGS(fdiv, fdiv.s)
INSN_2_3ARGS(fdiv, fdiv.s)

INSN_0_2ARGS(fsqrt, fsqrt.s)
INSN_1_2ARGS(fsqrt, fsqrt.s)
INSN_2_2ARGS(fsqrt, fsqrt.s)


INSN_0_3ARGS(fdiv_h, fdiv.h)
INSN_1_3ARGS(fdiv_h, fdiv.h)
INSN_2_3ARGS(fdiv_h, fdiv.h)

INSN_0_3ARGS(vfadd_h, vfadd.h)
INSN_1_3ARGS(vfadd_h, vfadd.h)
INSN_2_3ARGS(vfadd_h, vfadd.h)

INSN_0_3ARGS(vfmul_h, vfmul.h)
INSN_1_3ARGS(vfmul_h, vfmul.h)
INSN_2_3ARGS(vfmul_h, vfmul.h)

INSN_0_3ARGS(vfmac_h, vfmac.h)
INSN_1_3ARGS(vfmac_h, vfmac.h)
INSN_2_3ARGS(vfmac_h, vfmac.h)

INSN_0_2ARGS(fcvt_h, fcvt.w.h)
INSN_1_2ARGS(fcvt_h, fcvt.w.h)
INSN_2_2ARGS(fcvt_h, fcvt.w.h)

INSN_0_4ARGS(fmadd, fmadd.s)
INSN_1_4ARGS(fmadd, fmadd.s)
INSN_2_4ARGS(fmadd, fmadd.s)

INSN_0_4ARGS(fnmadd, fnmadd.s)
INSN_1_4ARGS(fnmadd, fnmadd.s)
INSN_2_4ARGS(fnmadd, fnmadd.s)


INT64_INSN_0_3ARGS(add_d, add.d)
INT64_INSN_1_3ARGS(add_d, add.d)
INT64_INSN_2_3ARGS(add_d, add.d)

INT64_INSN_0_3ARGS(p_muls_d, p.muls.d)
INT64_INSN_1_3ARGS(p_muls_d, p.muls.d)
INT64_INSN_2_3ARGS(p_muls_d, p.muls.d)

INT64_INSN_0_3ARGS(p_mulhu_d, p.mulhu.d)
INT64_INSN_1_3ARGS(p_mulhu_d, p.mulhu.d)
INT64_INSN_2_3ARGS(p_mulhu_d, p.mulhu.d)

INT64_INSN_0_3ARGS(p_mac_d, p.mac.d)
INT64_INSN_1_3ARGS(p_mac_d, p.mac.d)
INT64_INSN_2_3ARGS(p_mac_d, p.mac.d)

INT64_INSN_0_2ARGS(p_cnt_d, p.cnt.d)
INT64_INSN_1_2ARGS(p_cnt_d, p.cnt.d)
INT64_INSN_2_2ARGS(p_cnt_d, p.cnt.d)

INT64_INSN_0_2ARGS(p_exths_d, p.exths.d)
INT64_INSN_1_2ARGS(p_exths_d, p.exths.d)
INT64_INSN_2_2ARGS(p_exths_d, p.exths.d)
