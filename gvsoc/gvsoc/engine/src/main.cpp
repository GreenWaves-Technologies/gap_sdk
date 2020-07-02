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

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include <gv/gvsoc.h>
#include <algorithm>
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>




int main(int argc, char *argv[])
{
    char *config_path = NULL;
    bool open_proxy = false;

    for (int i=1; i<argc; i++)
    {
        if (strncmp(argv[i], "--config=", 9) == 0)
        {
            config_path = &argv[i][9];
        }
        else if (strcmp(argv[i], "--proxy") == 0)
        {
            open_proxy = true;
        }
    }

    if (config_path == NULL)
    {
        fprintf(stderr, "No configuration specified, please specify through option --config=<config path>.\n");
        return -1;
    }

    int proxy_socket;
    void *instance = gv_open(config_path, open_proxy, &proxy_socket, -1, -1);

    if (open_proxy)
    {
        printf("Opened proxy on socket %d\n", proxy_socket);
    }

    int retval = gv_run(instance);

    gv_stop(instance);

    return retval;
}