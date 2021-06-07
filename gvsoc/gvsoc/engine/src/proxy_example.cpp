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

#include <gv/gvsoc_proxy.hpp>
#include <unistd.h>


int main()
{
    Gvsoc_proxy *proxy = new Gvsoc_proxy("/home/haugoug/src/pulp-sdk/tests/pmsis_tests/quick/hello/build/gapuino/plt_config.json");

    if (proxy->open())
        return -1;

    for (int i=0; i<5; i++)
    {
        proxy->run();

        sleep(2);

        printf("Deactivate\n");


        int64_t timestamp = proxy->pause();

        printf("Paused at %ld\n", timestamp);

        sleep(5);

        proxy->remove_event_regex(".*");

        proxy->run();

        sleep(5);

        printf("Activate\n");
        proxy->pause();

        proxy->add_event_regex(".*");
    }

    proxy->pause();
    proxy->close();




    return 0;
}