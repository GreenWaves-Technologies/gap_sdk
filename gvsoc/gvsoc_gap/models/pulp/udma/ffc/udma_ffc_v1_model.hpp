#pragma once

/**
 * Convert In, a float [S, E, M] in this order, in a fixed point In  in Q(FixedSize-Precision).Precision scaled by 2^Scale.
 *
 * with S on SignSize bits
 * with E on ExpSize bits
 * with M on MantSize bits
 *
 * In practice we need only to deal with S,E,M for F32, F16 and B16
 *
 * FixedSize is 8, 16, 24 or 32 bits
 *
 * If lower part of Mantissa of In has to be eliminated (>>) then rounding to nearest integer is performed.
 */
unsigned int FloatToFixedFormat(unsigned int In,
        int FixedSize,
        int Precision,
        int Scale,
        unsigned int SignSize,
        unsigned int ExpSize,
        unsigned int MantSize);
/**
 * Convert fixed point In  in Q(FixedSize-Precision).Precision scaled by 2^Scale into a float [S, E, M] in this order
 *
 * with S on SignSize bits
 * with E on ExpSize bits
 * with M on MantSize bits
 *
 * In practice we need only to deal with S,E,M for F32, F16 and B16
 *
 * FixedSize is 8, 16, 24 or 32 bits
 *
 * If lower part of In has to be eliminated (>>) then rounding to nearest integer is performed.
 */
unsigned int FixedToFloatFormat(int In,
        int FixedSize,
        int Precision,
        int Scale,
        unsigned int
        SignSize,
        unsigned int ExpSize,
        unsigned int MantSize);
