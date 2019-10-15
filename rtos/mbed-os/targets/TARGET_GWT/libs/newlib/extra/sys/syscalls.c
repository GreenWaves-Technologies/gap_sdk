/* Support files for GNU libc.  Files in the system namespace go here.
   Files in the C namespace (ie those that do not start with an
   underscore) go in .c.  */

#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <reent.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

/* Forward prototypes.  */
int     _system         _PARAMS ((const char *));
int     _rename         _PARAMS ((const char *, const char *));
int     _isatty         _PARAMS ((int));
clock_t _times          _PARAMS ((struct tms *));
int     _gettimeofday   _PARAMS ((struct timeval *, void *));
void    _raise          _PARAMS ((void));
int     _unlink         _PARAMS ((const char *));
int     _link           _PARAMS ((void));
int     _stat           _PARAMS ((const char *, struct stat *));
int     _fstat          _PARAMS ((int, struct stat *));
caddr_t _sbrk           _PARAMS ((int));
int     _getpid         _PARAMS ((int));
int     _kill           _PARAMS ((int, int));
void    _exit           _PARAMS ((int));
int     _close          _PARAMS ((int));
int     _open           _PARAMS ((const char *, int, ...));
int     _write          _PARAMS ((int, char *, int));
int     _lseek          _PARAMS ((int, int, int));
int     _read           _PARAMS ((int, char *, int));

static int wrap          _PARAMS ((int));
static int error         _PARAMS ((int));
static int findslot     _PARAMS ((int));

/* following is copied from libc/stdio/local.h to check std streams */
extern void   _EXFUN(__sinit,(struct _reent *));
#define CHECK_INIT(ptr)                         \
    do                                          \
    {                                           \
        if ((ptr) && !(ptr)->__sdidinit)        \
            __sinit (ptr);                      \
    }                                           \
    while (0)

/* Adjust our internal handles to stay away from std* handles.  */
#define FILE_HANDLE_OFFSET (0x20)

/* Struct used to keep track of the file position, just so we
   can implement fseek(fh,x,SEEK_CUR).  */
typedef struct
{
    int handle;
    int pos;
}
    poslog;

#define MAX_OPEN_FILES 20
static poslog openfiles [MAX_OPEN_FILES];

static int
findslot (int fh)
{
    int i;
    for (i = 0; i < MAX_OPEN_FILES; i ++)
        if (openfiles[i].handle == fh)
            break;
    return i;
}

static int
error (int result)
{
    return result;
}

static int
wrap (int result)
{
    if (result == -1)
        return error (-1);
    return result;
}

int __attribute__((weak))
_read (int file,
       char * ptr,
       int len)
{
    int slot=0;
    int x=0;

    if (x < 0)
        return error (-1);

    if (slot != MAX_OPEN_FILES)
        openfiles [slot].pos += len - x;

    /* x == len is not an error, at least if we want feof() to work.  */
    return len - x;
}

int __attribute__((weak))
_lseek (int file,
        int ptr,
        int dir)
{
    return 0;
}

int __attribute__((weak))
_write (int    file,
        char * ptr,
        int    len)
{
    int slot=0;
    int x=0;

    if (x == -1 || x == len)
        return error (-1);

    if (slot != MAX_OPEN_FILES)
        openfiles[slot].pos += len - x;

    return len - x;
}

extern int strlen (const char *);

int __attribute__((weak))
_open (const char * path,
       int          flags,
       ...)
{
    return -1;
}

int __attribute__((weak))
_close (int file)
{
    return -1;
}

int __attribute__((weak))
_kill (int pid, int sig)
{
    (void)pid; (void)sig;
    return 0;
}

int
_getpid (int n)
{
    return 1;
    n = n;
}

caddr_t __attribute__((weak))
_sbrk (int incr)
{
    extern char   end asm ("__heapl2ram_start");/* Defined by the linker.  */
    extern char   limit asm ("__heapl2ram_limit");/* Defined by the linker.  */

    static char * heap_end = (char *)0x1C000000;
    char *        prev_heap_end;

    if (heap_end == (char *)0x1C000000)
        heap_end = & end;

    prev_heap_end = heap_end;

    if (heap_end + incr > (& limit))
    {
        errno = ENOMEM;
        return (caddr_t) -1;
    }

    heap_end += incr;

    return (caddr_t) prev_heap_end;
}

int __attribute__((weak))
_fstat (int file, struct stat * st)
{
    return 0;
}

int __attribute__((weak))
_stat (const char *fname, struct stat *st)
{

    return 0;
}

int __attribute__((weak))
_link (void)
{
    return -1;
}

int __attribute__((weak))
_unlink (const char *path)
{
    return -1;
}

void
_raise (void)
{
    return;
}

int
_gettimeofday (struct timeval * tp, void * tzvp)
{
    struct timezone *tzp = tzvp;

    return 0;
}


int __attribute__((weak))
_isatty (int fd)
{
    return (fd <= 2) ? 1 : 0;  /* one of stdin, stdout, stderr */
}

int
_system (const char *s)
{
    return 0;
}

int __attribute__((weak))
_rename (const char * oldpath, const char * newpath)
{
    errno = ENOSYS;
    return -1;
}

extern void exit(int return_code);

void _exit(int code)
{
    exit(code);
}
