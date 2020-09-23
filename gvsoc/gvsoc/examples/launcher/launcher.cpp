/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include <gv/gvsoc.h>
#include <stdio.h>


int main(int argc, char **argv)
{
    struct gv_conf gv_conf;

    gv_init(&gv_conf);

    void *gvsoc = gv_create(argv[1], &gv_conf);

    gv_start(gvsoc);

    int64_t timestamp = gv_time(gvsoc);

    while(1)
    {
        gv_step(gvsoc, timestamp);
        timestamp = gv_time(gvsoc);
    }

    gv_stop(gvsoc);
    gv_destroy(gvsoc);

    return 0;
}