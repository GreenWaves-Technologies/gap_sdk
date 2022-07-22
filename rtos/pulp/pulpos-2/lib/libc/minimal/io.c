/*
 * Copyright (C) 2019 GreenWaves Technologies
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

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include "hal/pulp.h"
#include <stdint.h>
#include "io.h"
#include <pmsis.h>
#include "semihost.h"



#if defined(POS_CONFIG_IO_UART) && POS_CONFIG_IO_UART == 1
static int pos_io_uart_enabled;
static PI_FC_TINY struct pi_device pos_io_uart;

#define POS_PUTC_UART_BUFFER_SIZE 128

static PI_L2 char pos_libc_uart_buffer[POS_PUTC_UART_BUFFER_SIZE];
static int pos_libc_uart_buffer_index;

static PI_L2 char pos_libc_uart_buffer_cl[ARCHI_NB_CLUSTER][POS_PUTC_UART_BUFFER_SIZE];
static int pos_libc_uart_buffer_index_cl[ARCHI_NB_CLUSTER];
#endif


#if (defined(POS_CONFIG_IO_HOST) && POS_CONFIG_IO_HOST == 1) || (defined(POS_CONFIG_IO_UART) && POS_CONFIG_IO_UART == 1)
static PI_CL_L1_TINY pos_cl_mutex_t pos_io_lock = POS_CL_MUTEX_INIT;
#endif


#if defined(POS_CONFIG_IO_HOST) && POS_CONFIG_IO_HOST == 1

#define POS_PUTC_HOST_BUFFER_SIZE 128

static PI_L2 char pos_libc_host_buffer[POS_PUTC_HOST_BUFFER_SIZE];
static int pos_libc_host_buffer_index;

static PI_L2 char pos_libc_host_buffer_cl[ARCHI_NB_CLUSTER][POS_PUTC_HOST_BUFFER_SIZE];
static int pos_libc_host_buffer_index_cl[ARCHI_NB_CLUSTER];
#endif

static int errno;
int *__errno() { return &errno; }



int strcmp(const char *s1, const char *s2)
{
    while (*s1 != '\0' && *s1 == *s2)
    {
        s1++;
        s2++;
    }

    return (*(unsigned char *) s1) - (*(unsigned char *) s2);
}



int strncmp(const char *s1, const char *s2, size_t n)
{
    if (n == 0)
        return 0;

    while (n-- != 0 && *s1 == *s2)
    {
        if (n == 0 || *s1 == '\0')
            break;
        s1++;
        s2++;
    }

    return (*(unsigned char *) s1) - (*(unsigned char *) s2);
}



size_t strlen(const char *str)
{
    const char *start = str;

    while (*str)
        str++;
    return str - start;
}



int memcmp(const void *m1, const void *m2, size_t n)
{
    unsigned char *s1 = (unsigned char *) m1;
    unsigned char *s2 = (unsigned char *) m2;

    while (n--)
    {
        if (*s1 != *s2)
        {
            return *s1 - *s2;
        }
        s1++;
        s2++;
    }
    return 0;
}



void *memset(void *m, int c, size_t n)
{
    char *s = (char *)m;
    while (n--)
        *s++ = (char) c;

    return m;
}



void *memcpy(void *dst0, const void *src0, size_t len0)
{
    void *save = dst0;

    char src_aligned = (((size_t) src0) & 3) == 0;
    char dst_aligned = (((size_t) dst0) & 3) == 0;
    char copy_full_words = (len0 & 3) == 0;

    if (src_aligned && dst_aligned && copy_full_words)
    {
        // all accesses are aligned => can copy full words
        uint32_t *dst = (uint32_t *) dst0;
        uint32_t *src = (uint32_t *) src0;

        while (len0)
        {
            *dst++ = *src++;
            len0 -= 4;
        }
    }
    else
    {
        // copy bytewise
        char *dst = (char *) dst0;
        char *src = (char *) src0;

        while (len0)
        {
            *dst++ = *src++;
            len0--;
        }
    }

    return save;
}



void *memmove(void *d, const void *s, size_t n)
{
    char *dest = d;
    const char *src  = s;

    if ((size_t) (dest - src) < n)
    {
        /*
         * The <src> buffer overlaps with the start of the <dest> buffer.
         * Copy backwards to prevent the premature corruption of <src>.
         */

        while (n > 0)
        {
            n--;
            dest[n] = src[n];
        }
    }
    else
    {
        /* It is safe to perform a forward-copy */
        while (n > 0)
        {
            *dest = *src;
            dest++;
            src++;
            n--;
        }
    }

    return d;
}



char *strcpy(char *d, const char *s)
{
	char *dest = d;
	while (*s != '\0')
    {
		*d = *s;
		d++;
		s++;
	}
	*d = '\0';
	return dest;
}



char *strcat(char *dest, const char *src)
{
	strcpy(dest + strlen(dest), src);
	return dest;
}



char *strchr(const char *s, int c)
{
    char tmp = (char) c;

    while ((*s != tmp) && (*s != '\0'))
        s++;

    return (*s == tmp) ? (char *) s : NULL;
}



static void pos_libc_putc_stdout(char c)
{
    *(volatile uint32_t *)(long)(ARCHI_STDOUT_ADDR + STDOUT_PUTC_OFFSET + (hal_core_id()<<3) + (hal_cluster_id()<<7)) = c;
}




#if defined(POS_CONFIG_IO_HOST) && POS_CONFIG_IO_HOST == 1

typedef struct
{
    int cid;
    int done;
    pi_task_t task;
} pos_cl_libc_putc_host_flush_t;


static void pos_libc_putc_host_req(void *_req)
{
    pos_cl_libc_putc_host_flush_t *req = _req;
    pos_semihost_write(0, pos_libc_host_buffer_cl[req->cid], strlen(pos_libc_host_buffer_cl[req->cid]));
    int cid = req->cid;
  __asm__ __volatile__ ("" : : : "memory");
    req->done = 1;
  __asm__ __volatile__ ("" : : : "memory");
    pi_cluster_notif_req_done(cid);
}


int pos_libc_putc_host_cl_flush()
{
    pos_cl_libc_putc_host_flush_t req;
    req.cid = hal_cluster_id();
    req.done = 0;
    pos_task_init_from_cluster(&req.task);
    pi_task_callback(&req.task, pos_libc_putc_host_req, (void* )&req);
    pi_cl_send_task_to_fc(&req.task);

    while((*(volatile int *)&req.done) == 0)
    {
        eu_evt_maskWaitAndClr(1<<PI_CL_WAIT_TASK_EVT);
    }

    return 0;
}


static void pos_libc_putc_host(char c)
{
    char *buffer;
    int *index;

    if (hal_is_fc())
    {
        buffer = pos_libc_host_buffer;
        index = &pos_libc_host_buffer_index;
    }
    else
    {
        buffer = pos_libc_host_buffer_cl[hal_cluster_id()];
        index = &pos_libc_host_buffer_index_cl[hal_cluster_id()];
    }


    buffer[(*index)++] = c;

    if (*index == POS_PUTC_HOST_BUFFER_SIZE - 1 || c == '\n')
    {
        buffer[*index] = 0;
        *index = 0;
        if (hal_is_fc())
        {
            pos_semihost_write(0, buffer, strlen(buffer));
        }
        else
        {
            pos_libc_putc_host_cl_flush();
        }
    }
}

#endif


#if defined(POS_CONFIG_IO_UART) && POS_CONFIG_IO_UART == 1

typedef struct
{
    int cid;
    int done;
    pi_task_t task;
} pos_cl_libc_putc_uart_flush_t;


int pos_libc_putc_uart_flush(uint8_t *buffer)
{
    pi_uart_write(&pos_io_uart, buffer, strlen(buffer));
}


static void pos_libc_putc_uart_req(void *_req)
{
    pos_cl_libc_putc_uart_flush_t *req = _req;
    pos_libc_putc_uart_flush(pos_libc_uart_buffer_cl[req->cid]);
    req->done = 1;
    pi_cluster_notif_req_done(req->cid);
}


int pos_libc_putc_uart_cl_flush()
{
    pos_cl_libc_putc_uart_flush_t req;
    req.cid = hal_cluster_id();
    req.done = 0;
    pos_task_init_from_cluster(&req.task);
    pi_task_callback(&req.task, pos_libc_putc_uart_req, (void* )&req);
    pi_cl_send_task_to_fc(&req.task);

    while((*(volatile int *)&req.done) == 0)
    {
        eu_evt_maskWaitAndClr(1<<PI_CL_WAIT_TASK_EVT);
    }

    return 0;
}

static void pos_libc_putc_uart(char c)
{
    if (pos_io_uart_enabled)
    {
        char *buffer;
        int *index;

        if (hal_is_fc())
        {
            buffer = pos_libc_uart_buffer;
            index = &pos_libc_uart_buffer_index;
        }
        else
        {
            buffer = pos_libc_uart_buffer_cl[hal_cluster_id()];
            index = &pos_libc_uart_buffer_index_cl[hal_cluster_id()];
        }


        buffer[(*index)++] = c;

        if (*index == POS_PUTC_UART_BUFFER_SIZE - 1 || c == '\n')
        {
            buffer[*index] = 0;
            *index = 0;
            if (hal_is_fc())
            {
                pos_libc_putc_uart_flush(buffer);
            }
            else
            {
                pos_libc_putc_uart_cl_flush();
            }
        }
    }
}
#endif



static void pos_putc(char c)
{
#if defined(POS_CONFIG_IO_UART) && POS_CONFIG_IO_UART == 1
    pos_libc_putc_uart(c);
#elif defined(POS_CONFIG_IO_HOST) && POS_CONFIG_IO_HOST == 1
    pos_libc_putc_host(c);
#else
    pos_libc_putc_stdout(c);
#endif
}



int puts(const char *s)
{
#if (defined(POS_CONFIG_IO_HOST) && POS_CONFIG_IO_HOST == 1) || (defined(POS_CONFIG_IO_UART) && POS_CONFIG_IO_UART == 1)
    if (!hal_is_fc())
        pos_cl_mutex_lock(&pos_io_lock);
#endif

    char c;
    do
    {
        c = *s;
        if (c == 0)
        {
            pos_putc('\n');
            break;
        }
        pos_putc(c);
        s++;
    } while(1);

#if (defined(POS_CONFIG_IO_HOST) && POS_CONFIG_IO_HOST == 1) || (defined(POS_CONFIG_IO_UART) && POS_CONFIG_IO_UART == 1)
    if (!hal_is_fc())
        pos_cl_mutex_unlock(&pos_io_lock);
#endif

    return 0;
}



int fputc(int c, FILE *stream)
{
#if (defined(POS_CONFIG_IO_HOST) && POS_CONFIG_IO_HOST == 1) || (defined(POS_CONFIG_IO_UART) && POS_CONFIG_IO_UART == 1)
    if (!hal_is_fc())
        pos_cl_mutex_lock(&pos_io_lock);
#endif

    pos_putc(c);

#if (defined(POS_CONFIG_IO_HOST) && POS_CONFIG_IO_HOST == 1) || (defined(POS_CONFIG_IO_UART) && POS_CONFIG_IO_UART == 1)
    if (!hal_is_fc())
        pos_cl_mutex_unlock(&pos_io_lock);
#endif

    return 0;
}


int pos_libc_fputc_locked(int c, FILE *stream)
{
    pos_putc(c);

    return 0;
}



int putchar(int c)
{
    return fputc(c, stdout);
}



int pos_libc_prf_locked(int (*func)(), void *dest, char *format, va_list vargs)
{
    int err;

#if (defined(POS_CONFIG_IO_HOST) && POS_CONFIG_IO_HOST == 1) || (defined(POS_CONFIG_IO_UART) && POS_CONFIG_IO_UART == 1)
    if (!hal_is_fc())
        pos_cl_mutex_lock(&pos_io_lock);
#endif

    err =  pos_libc_prf(func, dest, format, vargs);

#if (defined(POS_CONFIG_IO_HOST) && POS_CONFIG_IO_HOST == 1) || (defined(POS_CONFIG_IO_UART) && POS_CONFIG_IO_UART == 1)
    if (!hal_is_fc())
        pos_cl_mutex_unlock(&pos_io_lock);
#endif

    return err;
}


static void __attribute__((noreturn)) pos_wait_forever()
{
#if defined(ITC_VERSION)
    hal_itc_enable_clr(0xffffffff);
    while(1) hal_itc_wait_for_interrupt();
#elif defined(EU_VERSION) && EU_VERSION >=3
    eu_evt_maskClr(0xffffffff);
    eu_evt_wait();
#endif
    while(1);
}


void exit(int status)
{
    pos_init_stop();

    apb_soc_ctrl_corestatus_set(ARCHI_APB_SOC_CTRL_ADDR,
        APB_SOC_CTRL_CORESTATUS_STATUS((1<<31) | status)
    );

#if defined(POS_CONFIG_IO_HOST) && POS_CONFIG_IO_HOST == 1
    pos_semihost_exit(status == 0 ? SEMIHOST_EXIT_SUCCESS : SEMIHOST_EXIT_ERROR);
#endif

    pos_wait_forever();
}



void abort()
{
    exit(-1);
}



int pos_io_start()
{
#if defined(POS_CONFIG_IO_UART) && POS_CONFIG_IO_UART == 1
    struct pi_uart_conf conf;

    pi_uart_conf_init(&conf);

    conf.enable_tx = 1;
    conf.uart_id = POS_CONFIG_IO_UART_ITF;
    conf.baudrate_bps = POS_CONFIG_IO_UART_BAUDRATE;

    pi_open_from_conf(&pos_io_uart, &conf);

    if (pi_uart_open(&pos_io_uart))
        return -1;

    pos_io_uart_enabled = 1;

#endif

    return 0;
}



int pos_io_stop()
{
#if defined(POS_CONFIG_IO_UART) && POS_CONFIG_IO_UART == 1

    pos_io_uart_enabled = 0;

    pi_uart_close(&pos_io_uart);

#endif

  return 0;
}


size_t strcspn( const char * s1, const char * s2 )
{
    size_t len = 0;
    const char * p;
    while ( s1[len] )
    {
        p = s2;
        while ( *p )
        {
            if ( s1[len] == *p++ )
            {
                return len;
            }
        }
        ++len;
    }
    return len;
}


size_t strspn( const char * s1, const char * s2 )
{
    size_t len = 0;
    const char * p;
    while ( s1[ len ] )
    {
        p = s2;
        while ( *p )
        {
            if ( s1[len] == *p )
            {
                break;
            }
            ++p;
        }
        if ( ! *p )
        {
            return len;
        }
        ++len;
    }
    return len;
}


void pos_io_init()
{
#if defined(POS_CONFIG_IO_UART) && POS_CONFIG_IO_UART == 1
    pos_io_uart_enabled = 0;
    pos_libc_uart_buffer_index = 0;
    for (int i=0; i<ARCHI_NB_CLUSTER; i++)
    {
        pos_libc_uart_buffer_index_cl[i] = 0;
    }
#endif

#if defined(POS_CONFIG_IO_HOST) && POS_CONFIG_IO_HOST == 1
    pos_libc_host_buffer_index = 0;
    for (int i=0; i<ARCHI_NB_CLUSTER; i++)
    {
        pos_libc_host_buffer_index_cl[i] = 0;
    }
#endif
}
