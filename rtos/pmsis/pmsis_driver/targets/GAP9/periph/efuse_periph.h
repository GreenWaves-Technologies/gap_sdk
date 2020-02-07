/* ----------------------------------------------------------------------------
   -- EFUSE CTRL Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup EFUSE_CTRL_Peripheral_Access_Layer EFUSE_CTRL Peripheral Access Layer
 * @{
 */

/** EFUSE_CTRL - Register Layout Typedef */
typedef struct {
    volatile uint32_t CMD;                       /**< EFUSE_Control register, offset: 0x00 */
    volatile uint32_t CFG;                       /**< EFUSE_Control register, offset: 0x04 */
} EFUSE_CTRL_Type;

/* ----------------------------------------------------------------------------
   -- EFUSE_CTRL Register Masks
   ---------------------------------------------------------------------------- */
/*! @name CFG - EFUSE control configure register */
#define EFUSE_CTRL_SHORT_MASK                           (0x3FU)
#define EFUSE_CTRL_SHORT_SHIFT                          (0U)
#define EFUSE_CTRL_SHORT(x)                             (((uint32_t)(((uint32_t)(x)) /* << EFUSE_CTRL_SHORT_SHIFT */)) & EFUSE_CTRL_SHORT_MASK)

#define EFUSE_CTRL_MEDIUM_MASK                          (0xFFC0U)
#define EFUSE_CTRL_MEDIUM_SHIFT                         (6U)
#define EFUSE_CTRL_MEDIUM(x)                            (((uint32_t)(((uint32_t)(x)) << EFUSE_CTRL_MEDIUM_SHIFT)) & EFUSE_CTRL_MEDIUM_MASK)

#define EFUSE_CTRL_LONG_MASK                            (0x3FFF0000U)
#define EFUSE_CTRL_LONG_SHIFT                           (16U)
#define EFUSE_CTRL_LONG(x)                              (((uint32_t)(((uint32_t)(x)) << EFUSE_CTRL_LONG_SHIFT)) & EFUSE_CTRL_LONG_MASK)

#define EFUSE_CTRL_MARGIN_MASK                          (0xC0000000)
#define EFUSE_CTRL_MARGIN_SHIFT                         (30U)
#define EFUSE_CTRL_MARGIN(x)                            (((uint32_t)(((uint32_t)(x)) << EFUSE_CTRL_MARGIN_SHIFT)) & EFUSE_CTRL_MARGIN_MASK)

/*!
 * @addtogroup EFUSE_CTRL_Register_Masks EFUSE_CTRL Register Masks
 * @{
 */
#define    EFUSE_CTRL_CMD_READ       0x1
#define    EFUSE_CTRL_CMD_WRITE      0x2
#define    EFUSE_CTRL_CMD_SLEEP      0x4
/*!
 * @}
 */ /* end of group EFUSE_CTRL_Register_Masks */


/* EFUSE_CTRL - Peripheral instance base addresses */
/** Peripheral EFUSE_CTRL base address */
#define EFUSE_CTRL_BASE                               (SOC_PERI_BASE + 0x0F000u)
/** Peripheral EFUSE_CTRL base pointer */
#define EFUSE_CTRL                                    ((EFUSE_CTRL_Type *)EFUSE_CTRL_BASE)
  /** Array initializer of EFUSE_CTRL base addresses */
#define EFUSE_CTRL_BASE_ADDRS                         { EFUSE_CTRL_BASE }
/** Array initializer of EFUSE_CTRL base pointers */
#define EFUSE_CTRL_BASE_PTRS                          { EFUSE_CTRL }

/*!
 * @}
 */ /* end of group EFUSE_CTRL_Peripheral_Access_Layer */

/* ----------------------------------------------------------------------------
   -- EFUSE REG Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup EFUSE_REGS_Peripheral_Access_Layer EFUSE_REGS Peripheral Access Layer
 * @{
 */

/** EFUSE_REGS - Registers Layout Typedef */
typedef struct {
  volatile uint32_t REGS[128];            /**< EFUSE_Registers, offset: 0x000 */
} EFUSE_REGS_Type;

/* ----------------------------------------------------------------------------
   -- EFUSE_REGS Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup EFUSE_REGS_Register_Masks EFUSE_REGS Register Masks
 * @{
 */


/*!
 * @}
 */ /* end of group EFUSE_REGS_Register_Masks */


/* EFUSE_REGS - Peripheral instance base addresses */
/** Peripheral EFUSE_REGS base address */
#define EFUSE_REGS_BASE                                (SOC_PERI_BASE + 0x0F200u)
/** Peripheral EFUSE_REGS base pointer */
#define EFUSE_REGS                                     ((EFUSE_REGS_Type *)EFUSE_REGS_BASE)
/** Array initializer of EFUSE_REGS base addresses */
#define EFUSE_REGS_BASE_ADDRS                          { EFUSE_REGS_BASE }
/** Array initializer of EFUSE_REGS base pointers */
#define EFUSE_REGS_BASE_PTRS                           { EFUSE_REGS }

/*!
 * @}
 */ /* end of group EFUSE_REGS_Peripheral_Access_Layer */
