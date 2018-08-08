#include "rt/rt_data.h"

static unsigned int __rt_padframe_default[] = { 0x00055500, 0x00154000, 0x00054000, 0x00000000,};

static unsigned int __rt_padframe_hyper[] = { 0x00055500, 0x0f154000, 0x003fffff, 0x00000000,};

static unsigned int __rt_padframe_gpio[] = { 0x00055500, 0x0f154000, 0x003fffff, 0x00000000,};

rt_padframe_profile_t __rt_padframe_profiles[] = {
  { .name="default", .config=__rt_padframe_default },
  { .name="hyper", .config=__rt_padframe_hyper },
};

int __rt_nb_profile = 2;
