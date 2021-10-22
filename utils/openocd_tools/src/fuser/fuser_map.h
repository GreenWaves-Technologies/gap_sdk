
typedef struct
{
    unsigned char id;
    unsigned char val;
}pi_fuser_reg_t;

#ifdef REVC_HYPER
pi_fuser_reg_t fuser_map[] = {
    {.id=0  , .val= 0x90    },
    {.id=1  , .val= 0x40    },
    {.id=27 , .val= 0x40    },
    {.id=28 , .val= 0x10    },
    {.id=30 , .val= 0xff    },
    {.id=31 , .val= 0xff    },
    {.id=37 , .val= 0x1     },
};
#elif defined(REVC_QSPI)
pi_fuser_reg_t fuser_map[] = {
    {.id=0  , .val= 0x90    },
    {.id=1  , .val= 0x40    },
    {.id=27 , .val= 0x40    },
    {.id=28 , .val= 0x10    },
    {.id=30 , .val= 0xff    },
    {.id=31 , .val= 0xff    },
};
#elif defined(REVB_HYPER)
pi_fuser_reg_t fuser_map[] = {
    {.id=0  , .val= 0x10    },
    {.id=1  , .val= 0x2     },
    {.id=33 , .val= 0x1F    },
    {.id=37 , .val= 0x1     },
};
#elif defined(TEST_FUSER)
pi_fuser_reg_t fuser_map[] = {
    {.id=126  , .val= 0x11    },
    {.id=127  , .val= 0x11    },
};
#else
#ifndef DUMP_REG
#define DUMP_REG
#endif
pi_fuser_reg_t fuser_map[] = {};
#endif
