
#pragma once

unsigned int FloatToFixedFormat(unsigned int In,
                                int FixedSize,
                                int Precision,
                                int Scale,
                                unsigned int SignSize,
                                unsigned int ExpSize,
                                unsigned int MantSize);

unsigned int FixedToFloatFormat(int In,
                                int FixedSize,
                                int Precision,
                                int Scale,
                                unsigned int SignSize,
                                unsigned int ExpSize,
                                unsigned int MantSize);
