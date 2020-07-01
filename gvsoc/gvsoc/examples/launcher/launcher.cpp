/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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