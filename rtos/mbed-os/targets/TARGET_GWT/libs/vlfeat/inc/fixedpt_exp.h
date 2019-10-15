#include "mchan_hal.h"

static const fixedpt coef_fixedpt_exp_HP_x0[] = {
   -512,
   -458,
   -404,
   -350,
   -296,
   -242,
   -188,
   -134,
   -80,
   -26,
   26,
   80,
   134,
   188,
   242,
   296,
   350,
   404,
   458,
   512
};

static const fixedpt coef_fixedpt_exp_HP_y0[] = {
   34,
   42,
   52,
   65,
   80,
   99,
   122,
   151,
   186,
   230,
   284,
   351,
   433,
   534,
   660,
   814,
   1005,
   1241,
   1532,
   1891
};

static const fixedpt coef_fixedpt_exp_HP_m[] = {
   38,
   47,
   58,
   72,
   89,
   110,
   136,
   168,
   207,
   256,
   316,
   390,
   482,
   595,
   734,
   907,
   1119,
   1381,
   1705
};


static inline fixedpt fixedpt_exp_HP(fixedpt x) {
   int i = -1;
   if(x < -512)
      return 34;
   if(x > 512)
      return 1891;
   while (x > coef_fixedpt_exp_HP_x0[i]) i++;
   
   return coef_fixedpt_exp_HP_y0[i-1] + fixedpt_mul(coef_fixedpt_exp_HP_m[i-1], (x - coef_fixedpt_exp_HP_x0[i-1]));
}

static const fixedpt coef_fixedpt_exp_x0[] = {
   -2048,
   -1832,
   -1616,
   -1401,
   -1185,
   -970,
   -754,
   -538,
   -323,
   -107,
   107,
   323,
   538,
   754,
   970,
   1185,
   1401,
   1616,
   1832,
   2048
};

static const fixedpt coef_fixedpt_exp_y0[] = {
   138,
   171,
   211,
   260,
   321,
   397,
   490,
   604,
   746,
   921,
   1137,
   1404,
   1733,
   2139,
   2640,
   3259,
   4023,
   4966,
   6129,
   7566
};

static const fixedpt coef_fixedpt_exp_m[] = {
   154,
   190,
   235,
   290,
   358,
   441,
   545,
   673,
   831,
   1025,
   1266,
   1563,
   1929,
   2381,
   2939,
   3628,
   4478,
   5527,
   6823
};

#if 0
static inline fixedpt fixedpt_exp(fixedpt x) {
   int i = -1;
   if(x < -2048)
      return 138;
   if(x > 2048)
      return 7566;
   while (x > coef_fixedpt_exp_x0[i]) i++;
   
   return coef_fixedpt_exp_y0[i-1] + fixedpt_mul(coef_fixedpt_exp_m[i-1], (x - coef_fixedpt_exp_x0[i-1]));
}
#else

static void fixedpt_exp_init(fixedpt **coef_x0, fixedpt **coef_y0, fixedpt **coef_m) {
   *coef_x0 = (fixedpt *) malloc(sizeof(fixedpt)*64);
   *coef_y0 = (fixedpt *) malloc(sizeof(fixedpt)*64);
   *coef_m  = (fixedpt *) malloc(sizeof(fixedpt)*64);
   mchan_memcpy_async(*coef_x0, coef_fixedpt_exp_x0, sizeof(fixedpt)*64);
   mchan_memcpy_async(*coef_y0, coef_fixedpt_exp_y0, sizeof(fixedpt)*64);
   mchan_memcpy_async(*coef_m,  coef_fixedpt_exp_m,  sizeof(fixedpt)*64);
   mchan_memcpy_wait();
}

static void fixedpt_exp_deinit(fixedpt *coef_x0, fixedpt *coef_y0, fixedpt *coef_m) {
   free(coef_x0);
   free(coef_y0);
   free(coef_m);
}

static inline fixedpt fixedpt_exp(fixedpt x, fixedpt *coef_x0, fixedpt *coef_y0, fixedpt *coef_m) {
   int i = -1;
   if(x < -2048)
      return 138;
   if(x > 2048)
      return 7566;
   while (x > coef_x0[i]) i++;
   
   return coef_y0[i-1] + fixedpt_mul(coef_m[i-1], (x - coef_x0[i-1]));
}

#endif

