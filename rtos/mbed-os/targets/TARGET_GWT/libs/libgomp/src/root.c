/* Copyright 2014 DEI - Universita' di Bologna
   author       DEI - Universita' di Bologna
                Alessandro Capotondi - alessandro.capotondi@unibo.it
   info         Library main file for single compilation flow */

#include "libgomp.h"

#include "config/pulp/appsupport.c"
#include "config/pulp/lock.c"
#include "config/pulp/memutils.c"

#include "barrier.c"
#include "iter.c"
#include "work.c"
#include "team.c"
#include "target.c"
#include "loop.c"
#include "sections.c"
#include "single.c"
#include "parallel.c"
#include "critical.c"
#include "env.c"

#include "libgomp.c"
