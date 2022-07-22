/* PMSIS includes */
#include "pmsis.h"
#include "bsp/ram.h"
#include "bsp/ram/hyperram.h"

/* Variables used. */
#define XIP_PAGE_SIZE    (3) //(XIP_PAGE_SIZE_4KB)
#define BUFFER_ALIGN_SIZE      ( 512 << XIP_PAGE_SIZE )
#define BUFFER_SIZE ( BUFFER_ALIGN_SIZE * 3)

#ifdef XIP_RECONF
#define XIP_NB_DCACHE_PAGE (16)
#else
#define XIP_NB_DCACHE_PAGE (1)
#endif

static uint8_t *buff, *rcv_buff;
static uint32_t hyper_buff = 0;
static struct pi_device ram = {0};
static struct pi_hyperram_conf conf = {0};

static uint8_t *xip_buff = (uint8_t*)0x20000000;

#ifdef ASYNC
static volatile uint8_t done = 0;

/* Callback for end of RX transfer. */
void __end_of_rx(void *arg)
{
    printf("Read async done.\n");
    done = 1;
}

/* Callback for end of TX transfer. */
void __end_of_tx(void *arg)
{
    pi_task_t *cb = (pi_task_t *) arg;
    printf("Write async done.\n");
    pi_ram_read_async(&ram, hyper_buff, rcv_buff, (uint32_t) BUFFER_SIZE, cb);
}
#endif  /* ASYNC */

void data_exception_handler(void *arg)
{
    pi_xip_data_err_t *data_err = arg;
    uint32_t error_addr = data_err->error_addr;
    printf("data_exception_handler\n");
    if(error_addr != &xip_buff[BUFFER_SIZE*2])
    {
        printf("test failed\n");
        pmsis_exit(-1);
    }
    printf("test success\n");
    pmsis_exit(0);
}

void instr_exception_handler(void *arg)
{
    uint32_t error_addr = (uint32_t)arg;
    printf("instr_exception_handler\n");
    // fetch addr gives back a page (due to icache effect etc)
    if(error_addr != 0x2FFFFE00)
    {
        printf("test failed, error_addr=0x%x\n",error_addr);
        pmsis_exit(-1);
    }
    printf("test success\n");
    pmsis_exit(0);
}




void test_hyper_ram(void)
{
    printf("Entering main controller\n");


    uint32_t errors = 0;

    buff = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE*2);
    if (buff == NULL)
    {
        printf("buff alloc failed !\n");
        pmsis_exit(-1);
    }

    rcv_buff = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE*2);
    if (rcv_buff == NULL)
    {
        printf("rcv_buff alloc failed !\n");
        pmsis_exit(-2);
    }

    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        buff[i] = i & 0xFF;
        rcv_buff[i] = 0;
    }

#if defined (XIP_RECONF)
    for (uint32_t i=BUFFER_SIZE; i<(uint32_t) BUFFER_SIZE*2; i++)
    {
        buff[i] = (i+8) & 0xFF;
        rcv_buff[i] = 0;
    }
#endif

    /* Init & open ram. */
    pi_hyperram_conf_init(&conf);
    (&conf)->xip_en = 1; // we want to use xip on this device
    (&conf)->baudrate = 50*1000*1000;
    printf("baudrate at hyper init %x\n",(&conf)->baudrate);
    pi_open_from_conf(&ram, &conf);
    if (pi_ram_open(&ram))
    {
        printf("Error ram open !\n");
        pmsis_exit(-3);
    }

    uint32_t hyper_buff_raw = 0;
    if (pi_ram_alloc(&ram, &hyper_buff_raw, (uint32_t) BUFFER_SIZE*2+BUFFER_ALIGN_SIZE))
    {
        printf("Ram malloc failed !\n");
        pmsis_exit(-4);
    }
    else
    {
        printf("Ram allocated : %lx %ld.\n", hyper_buff, (uint32_t) BUFFER_SIZE*2+BUFFER_ALIGN_SIZE);
    }
    hyper_buff = (hyper_buff_raw + BUFFER_ALIGN_SIZE) & (~(BUFFER_ALIGN_SIZE-1));
    printf("hyper_buff aligned = %x\n",hyper_buff);
#if defined(XIP_ACCESS_TEST) || defined (XIP_RECONF)
    /* Write a buffer in ram, then read back from ram. */
    #ifdef ASYNC
    pi_task_t cb_tx, cb_rx;
    pi_task_callback(&cb_rx, __end_of_rx, NULL);
    pi_task_callback(&cb_tx, __end_of_tx, &cb_rx);
    pi_ram_write_async(&ram, hyper_buff, buff, (uint32_t) BUFFER_SIZE, &cb_tx);
    while (!done)
    {
        pi_yield();
    }
    #else
#if defined (XIP_RECONF)
    pi_ram_write(&ram, hyper_buff, buff, (uint32_t) BUFFER_SIZE*2);
    printf("Write sync done.\n");
    pi_ram_read(&ram, hyper_buff, rcv_buff, (uint32_t) BUFFER_SIZE*2);
    printf("Read sync done.\n");
#else
    pi_ram_write(&ram, hyper_buff, buff, (uint32_t) BUFFER_SIZE);
    printf("Write sync done.\n");
    pi_ram_read(&ram, hyper_buff, rcv_buff, (uint32_t) BUFFER_SIZE);
    printf("Read sync done.\n");
#endif
    #endif  /* ASYNC */

    /* Verification for normal read / write */
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        if (buff[i] != rcv_buff[i])
        {
            errors++;
            printf("%2x-%2x ", buff[i], rcv_buff[i]);
        }
    }

#if defined (XIP_RECONF)
    /* Verification for xip read, second buf, reconfigured */
    for (uint32_t i=BUFFER_SIZE; i<(uint32_t) BUFFER_SIZE*2; i++)
    {
        if (rcv_buff[i] != ((i+8) & 0xFF) )
        {
            errors++;
            printf("%i: %x - %x \n", i, xip_buff[i], ((i+8) & 0xFF));
        }
    }
#endif

#endif
    uint32_t page_mask = 0;
    pi_device_t *xip_device = pmsis_l2_malloc(sizeof(pi_device_t));
    for( int i = 0; i<XIP_NB_DCACHE_PAGE; i++)
    {
        pi_xip_dcache_page_alloc(i, XIP_PAGE_SIZE);
        printf("dcache page addr= 0x%x\n",hal_xip_dcache_page_l2_addr_get(i));
    }
    pi_xip_free_page_mask_get(&page_mask,XIP_NB_DCACHE_PAGE);
    if(page_mask != (1<<XIP_NB_DCACHE_PAGE)-1)
    {
        printf("page mask = 0x%x / vs 0x%x\n",page_mask,(1<<XIP_NB_DCACHE_PAGE)-1);
    }
    pi_xip_conf_t xip_conf = {0};
    xip_conf.ro = 0;
    xip_conf.per_id = XIP_DEVICE_HYPER0;
    xip_conf.page_size = XIP_PAGE_SIZE;
    xip_conf.page_mask = page_mask;
    printf("xip page alloc done\n");

    uint32_t mount_size = BUFFER_SIZE / (512 << XIP_PAGE_SIZE);

#if defined(XIP_ACCESS_TEST) || defined(XIP_RECONF)
    xip_device->config = &xip_conf;
    int ret = 0;
    if((ret =pi_xip_mount(xip_device,xip_buff,hyper_buff,mount_size,0)))
    {
        printf("xip open failed: %x\n",ret);
        pmsis_exit(-1);
    }
    printf("xip open success\n");

    /* Verification for xip read */
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        if (xip_buff[i] != rcv_buff[i])
        {
            errors++;
            printf("%i: %x - %x ", i, xip_buff[i], rcv_buff[i]);
            pmsis_exit(-1);
        }
    }
#ifdef XIP_RECONF
   
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE/16; i++)
    {
        xip_buff[i] = (uint8_t)(i+3);
    }

    for (uint32_t i=BUFFER_SIZE/2; i< ((BUFFER_SIZE/2) + (uint32_t) BUFFER_SIZE/16); i++)
    {
        xip_buff[i] = (uint8_t)(i+7);
    }
    // flush the XIP to check if WB has been done
    pi_xip_unmount(xip_device);

    printf("RECONF 1\n");
    if((ret =pi_xip_mount(xip_device,xip_buff,hyper_buff,mount_size,0)))
    {
        printf("xip open failed: %x\n",ret);
        pmsis_exit(-1);
    }
    /* Verification for xip WB */
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE/16; i++)
    {
        if (xip_buff[i] != (uint8_t)(i+3))
        {
            errors++;
            printf("%i: %x - %x ", i, xip_buff[i], rcv_buff[i]);
        }
    }
    for (uint32_t i=BUFFER_SIZE/2; i< ((BUFFER_SIZE/2) + (uint32_t) BUFFER_SIZE/16); i++)
    {
        if (xip_buff[i] != (uint8_t)(i+7))
        {
            errors++;
            printf("%i: %x - %x ", i, xip_buff[i], rcv_buff[i]);
        }
    }

    pi_xip_unmount(xip_device);

    printf("RECONF 2\n");
    if((ret =pi_xip_mount(xip_device,xip_buff,hyper_buff+BUFFER_SIZE,mount_size,0)))
    {
        printf("xip open failed: %x\n",ret);
        pmsis_exit(-1);
    }
    /* Verification for xip read, second buf, reconfigured */
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        if (xip_buff[i] != ((i+8) & 0xFF) )
        {
            errors++;
            printf("%i: %x - %x \n", i, xip_buff[i], ((i+8) & 0xFF));
        }
    }

#endif
    pi_xip_unmount(xip_device);
#endif
    
#ifdef ILLEGAL_DATA_ACCESS
    {
    xip_conf.data_exception_handler = data_exception_handler;
    xip_device->config = &xip_conf;
    int ret = 0;
    if((ret =pi_xip_mount(xip_device,xip_buff,hyper_buff,mount_size,0)))
    {
        printf("xip open failed: %x\n",ret);
        pmsis_exit(-1);
    }
    printf("xip open success\n");
    /* Verification for xip read */
    
    if(*(volatile uint32_t*)(&xip_buff[BUFFER_SIZE*2]) != 0xbadacce5)
    {
        printf("xip_buf error = 0x%x\n",*(volatile uint32_t*)(&xip_buff[BUFFER_SIZE*2]));
        errors++;
    }
    // we are not supposed to arrive here due to exception
    errors++;
    
    pi_xip_unmount(xip_device);
    }
#endif

#ifdef ILLEGAL_INSN_ACCESS
    {
    xip_conf.instr_exception_handler = instr_exception_handler;
    xip_device->config = &xip_conf;
    int ret = 0;
    if((ret =pi_xip_mount(xip_device,xip_buff,hyper_buff,mount_size,0)))
    {
        printf("xip open failed: %x\n",ret);
        pmsis_exit(-1);
    }
    printf("xip open success\n");
    printf("going to try illegal code exec\n");
    
    asm volatile (
            "jalr %[ill_insn_addr]\t\n"
            : : [ill_insn_addr] "r" (0x2FFFFF00));
    // we are not supposed to arrive here due to exception
    errors++;
    
    pi_xip_unmount(xip_device);
    }
#endif
    pmsis_l2_malloc_free(buff, (uint32_t) BUFFER_SIZE);
    pmsis_l2_malloc_free(rcv_buff, (uint32_t) BUFFER_SIZE);
    pi_ram_free(&ram, hyper_buff_raw, (uint32_t) BUFFER_SIZE+BUFFER_ALIGN_SIZE);
    
    pi_ram_close(&ram);

    printf("\nHyperram transfer done with %ld error(s) !\n", errors);
    printf("\nTest %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Hyperram Test ***\n\n");
    return pmsis_kickoff((void *) test_hyper_ram);
}
