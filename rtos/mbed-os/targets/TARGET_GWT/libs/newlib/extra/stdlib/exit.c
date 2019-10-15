#include <stdlib.h>
#include <unistd.h>/* for _exit() declaration */
#include <reent.h>

/*
 * Exit, flushing stdio buffers if necessary.
 */
extern void    Boot_Deinit();
extern void    Platform_Exit(int code);

void
_DEFUN (exit, (code),
        int code)
{
    Boot_Deinit();
    Platform_Exit(code);
}
