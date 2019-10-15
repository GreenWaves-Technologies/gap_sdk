/** @internal @brief IEEE double precision infinity constant */
static union { vl_uint64 raw ; double value ; }
  const vl_infinity_d =
#ifdef VL_COMPILER_MSC
    { 0x7FF0000000000000ui64 } ;
#else
    { 0x7FF0000000000000ULL } ;
#endif

/** @brief IEEE double precision positive infinity (not signaling) */
#define VL_INFINITY_D (vl_infinity_d.value)

/** @brief IEEE double precision epsilon (math constant)
 **
 ** <code>1.0 + VL_EPSILON_D</code> is the smallest representable
 ** double precision number greater than @c 1.0. Numerically,
 ** ::VL_EPSILON_D is equal to @f$ 2^{-52} @f$.
 **/
#define VL_EPSILON_D 2.220446049250313e-16
