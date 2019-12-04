/*
 * Copyright 2019 GreenWaves Technologies, SAS
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

#include <stdint.h>
#include <stdio.h>

/* AutoTiler includes. */
#include "AutoTilerLib.h"
/* Resize generator. */
#include "ResizeGenerator.h"

#include "setup.h"

int main(int argc, char **argv)
{
    // This will parse AutoTiler options and perform various initializations
    if (TilerParseOptions(argc, argv))
    {
        printf("Failed to initialize or incorrect output arguments directory.\n");
        return 1;
    }

    ResizeConfiguration(36000);

    LoadResizeLibrary();

    uint32_t W_in = (uint32_t) CAMERA_WIDTH, H_in = (uint32_t) CAMERA_HEIGHT;
    uint32_t W_out = (uint32_t) RESIZE_WIDTH, H_out = (uint32_t) RESIZE_HEIGHT;
    GenerateResize("ResizeImage", W_in, H_in, W_out, H_out);

    GenerateTilingCode();
    return 0;
}
