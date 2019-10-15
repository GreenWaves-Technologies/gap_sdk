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
