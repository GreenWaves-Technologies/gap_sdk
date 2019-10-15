static const fixedpt coef_fixedpt_sqrt_x0[] = {
   1,
   1,
   1,
   2,
   3,
   5,
   7,
   10,
   14,
   19,
   27,
   37,
   52,
   73,
   101,
   141,
   196,
   273,
   380,
   529,
   736,
   1024,
   1424,
   1981,
   2756,
   3834,
   5333,
   7419,
   10321,
   14357,
   19972,
   27782,
   38647,
   53761,
   74785,
   104031,
   144715,
   201308,
   280033,
   389544,
   541881,
   753793,
   1048576,
   1458638,
   2029061,
   2822557,
   3926362,
   5461827,
   7597760,
   10568983,
   14702149,
   20451655,
   28449594,
   39575253,
   55051773,
   76580629,
   106528681,
   148188386,
   206139768,
   286753945,
   398893554,
   554887109,
   771884380,
   1073741824
};

static const fixedpt coef_fixedpt_sqrt_y0[] = {
   64,
   75,
   89,
   105,
   123,
   146,
   172,
   203,
   239,
   282,
   333,
   393,
   463,
   546,
   645,
   760,
   897,
   1058,
   1248,
   1472,
   1736,
   2048,
   2415,
   2848,
   3360,
   3963,
   4674,
   5512,
   6501,
   7668,
   9044,
   10667,
   12581,
   14839,
   17502,
   20642,
   24346,
   28715,
   33867,
   39944,
   47112,
   55565,
   65536,
   77295,
   91164,
   107522,
   126816,
   149571,
   176409,
   208063,
   245397,
   289430,
   341364,
   402616,
   474860,
   560066,
   660561,
   779088,
   918884,
   1083763,
   1278228,
   1507586,
   1778099,
   2097152
};

static const fixedpt coef_fixedpt_sqrt_m[] = {
   120280,
   101981,
   86466,
   73311,
   62158,
   52701,
   44684,
   37886,
   32122,
   27235,
   23091,
   19578,
   16600,
   14074,
   11933,
   10117,
   8578,
   7273,
   6166,
   5228,
   4433,
   3758,
   3186,
   2702,
   2290,
   1942,
   1646,
   1396,
   1183,
   1003,
   851,
   721,
   611,
   518,
   439,
   372,
   316,
   268,
   227,
   192,
   163,
   138,
   117,
   99,
   84,
   71,
   60,
   51,
   43,
   36,
   31,
   26,
   22,
   19,
   16,
   13,
   11,
   9,
   8,
   7,
   6,
   5,
   4
};

#if 0
static inline fixedpt fixedpt_sqrt(fixedpt x) {
   int i = -1;
   if(x < 1)
      return 64;
   if(x > 1073741824)
      return 2097152;
   while (x > coef_fixedpt_sqrt_x0[i]) i++;
   
   return coef_fixedpt_sqrt_y0[i-1] + fixedpt_mul(coef_fixedpt_sqrt_m[i-1], (x - coef_fixedpt_sqrt_x0[i-1]));
}
#else

static void fixedpt_sqrt_init(fixedpt **coef_x0, fixedpt **coef_y0, fixedpt **coef_m) {
   *coef_x0 = (fixedpt *) malloc(sizeof(fixedpt)*64);
   *coef_y0 = (fixedpt *) malloc(sizeof(fixedpt)*64);
   *coef_m  = (fixedpt *) malloc(sizeof(fixedpt)*64);
   mchan_memcpy_async(*coef_x0, coef_fixedpt_sqrt_x0, sizeof(fixedpt)*64);
   mchan_memcpy_async(*coef_y0, coef_fixedpt_sqrt_y0, sizeof(fixedpt)*64);
   mchan_memcpy_async(*coef_m,  coef_fixedpt_sqrt_m,  sizeof(fixedpt)*64);
   mchan_memcpy_wait();
}

static void fixedpt_sqrt_deinit(fixedpt *coef_x0, fixedpt *coef_y0, fixedpt *coef_m) {
   free(coef_x0);
   free(coef_y0);
   free(coef_m);
}

static inline fixedpt fixedpt_sqrt(fixedpt x, fixedpt *coef_x0, fixedpt *coef_y0, fixedpt *coef_m) {
   int i = 0;
   if(x < 1)
      return 64;
   if(x > 1073741824)
      return 2097152;
   while (x > coef_x0[i]) i++;
   i--;
   
   return coef_y0[i] + fixedpt_mul(coef_m[i], (x - coef_x0[i]));
}
#endif

