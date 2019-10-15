/* connector for sbrk */

#include <reent.h>
#include <unistd.h>

extern void * _sbrk (ptrdiff_t incr);

void *
_sbrk_r(struct _reent *r, ptrdiff_t incr)
{
  return _sbrk (incr);
}
