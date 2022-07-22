/* PMSIS includes */
#include "pmsis.h"
#include "bsp/ram.h"
#include "bsp/ram/hyperram.h"

/** Internal includes - we don't want to expose xip in user api **/
//#include "pmsis/implem/drivers/xip/xip_internal.h"

/* Variables used. */
#define XIP_PAGE_SIZE    (0) //(XIP_PAGE_SIZE_4KB)
#define BUFFER_ALIGN_SIZE      ( 512 << XIP_PAGE_SIZE )
#define BUFFER_SIZE ( BUFFER_ALIGN_SIZE * 32)
#define XIP_BUFFER_SIZE ( BUFFER_ALIGN_SIZE * 1)
#define DEV_BUFFER_SIZE ( BUFFER_SIZE/2)
#define XIP_NB_DCACHE_PAGE 16

#define NB_DEVICES 2

pi_xip_conf_t xip_conf = {0};
extern func_t g_xip_tlb_data_handler;

static volatile uint8_t g_tlb_exception_called = 0;



static uint8_t *buff, *rcv_buff;
static uint32_t hyper_buff[2] = {0};
static struct pi_device ram[2];
static struct pi_hyperram_conf conf[2];

static uint8_t *xip_buff[2] = {(uint8_t*)0x20000000,(uint8_t*)0x20002000};

void tlb_exception_test(void *arg)
{
    pi_xip_data_err_t *data_err = arg;
    //printf("LRU: 0x%x\n",hal_xip_tlb_lru_get());
    printf("there was an XIP data exception\n page_addr = %x\n",data_err->error_addr);
    if(data_err->error_addr < 0x20002000)
    {
        pi_xip_tlb_lru_update(data_err->error_addr, hyper_buff[0] + g_tlb_exception_called*XIP_BUFFER_SIZE,0,0,0);
    }
    else
    {
        printf("fill for device 1\n");
        pi_xip_tlb_lru_update(data_err->error_addr, hyper_buff[1] + g_tlb_exception_called*XIP_BUFFER_SIZE,1,0,0);
    }
    //printf("LRU: 0x%x\n",hal_xip_tlb_lru_get());
    g_tlb_exception_called++;
}

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

void test_hyper_ram(void)
{
    printf("Entering main controller\n");

    //printf("g_xip_tlb_data_handler=%x\n",g_xip_tlb_data_handler);

    uint32_t errors = 0;

    buff = (uint8_t *) pi_l2_malloc((uint32_t) BUFFER_SIZE);
    if (buff == NULL)
    {
        printf("buff alloc failed !\n");
        pmsis_exit(-1);
    }

    rcv_buff = (uint8_t *) pi_l2_malloc((uint32_t) BUFFER_SIZE);
    if (rcv_buff == NULL)
    {
        printf("rcv_buff alloc failed !\n");
        pmsis_exit(-2);
    }

    int acc = 0;
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        if(i%XIP_BUFFER_SIZE == 0)
        {
            acc++;
        }
        buff[i] = (i+acc) & 0xFF;
        rcv_buff[i] = 0;
    }

    /* Init & open ram. */
    pi_hyperram_conf_init(&conf[0]);
    (&conf[0])->xip_en = 1; // we want to use xip on this device
    (&conf[0])->baudrate = 50*1000*1000;
    (&conf[0])->hyper_itf = 0;
    printf("baudrate at hyper init %x\n",(&conf[0])->baudrate);
    pi_open_from_conf(&ram[0], &conf[0]);
    if (pi_ram_open(&ram[0]))
    {
        printf("Error ram open !\n");
        pmsis_exit(-3);
    }
    pi_hyperram_conf_init(&conf[1]);
    (&conf[1])->xip_en = 1; // we want to use xip on this device
    (&conf[1])->baudrate = 50*1000*1000;
    (&conf[1])->hyper_itf = 1;
    printf("baudrate at hyper init %x\n",(&conf[1])->baudrate);
    pi_open_from_conf(&ram[1], &conf[1]);
    if (pi_ram_open(&ram[1]))
    {
        printf("Error ram open !\n");
        pmsis_exit(-3);
    }
    uint32_t hyper_buff_raw = 0;
    for(int i=0;i < NB_DEVICES; i++)
    {
        if (pi_ram_alloc(&ram[i], &hyper_buff_raw, (uint32_t) DEV_BUFFER_SIZE+BUFFER_ALIGN_SIZE))
        {
            printf("Ram malloc failed !\n");
            pmsis_exit(-4);
        }
        else
        {
            printf("Ram allocated : %lx %ld.\n", hyper_buff_raw, (uint32_t) DEV_BUFFER_SIZE+BUFFER_ALIGN_SIZE);
        }
        hyper_buff[i] = (hyper_buff_raw + BUFFER_ALIGN_SIZE) & (~(BUFFER_ALIGN_SIZE-1));
        printf("hyper_buff aligned = %x\n",hyper_buff[i]);
    }
#if 1
    /* Write a buffer in ram, then read back from ram. */
    pi_ram_write(&ram[0], hyper_buff[0], buff, (uint32_t) DEV_BUFFER_SIZE);
    printf("Write sync 1 done.\n");
    pi_ram_write(&ram[1], hyper_buff[1], buff+DEV_BUFFER_SIZE, (uint32_t) DEV_BUFFER_SIZE);
    printf("Write sync done.\n");
    pi_ram_read(&ram[0], hyper_buff[0], rcv_buff, (uint32_t) DEV_BUFFER_SIZE);
    pi_ram_read(&ram[1], hyper_buff[1], rcv_buff+DEV_BUFFER_SIZE, (uint32_t) DEV_BUFFER_SIZE);
    printf("Read sync done.\n");

    /* Verification for normal read / write */
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        if (buff[i] != rcv_buff[i])
        {
            errors++;
            printf("%2x-%2x \n", buff[i], rcv_buff[i]);
        }
    }
#endif
#if 1
    uint32_t page_mask = 0;
    pi_device_t *xip_device = pi_l2_malloc(sizeof(pi_device_t));
    for( int i = 0; i<XIP_NB_DCACHE_PAGE; i++)
    {
        hal_compiler_barrier();
        pi_xip_dcache_page_alloc(i, XIP_PAGE_SIZE);
        //printf("dcache page addr= 0x%x\n",hal_xip_dcache_page_l2_addr_get(i));
        hal_compiler_barrier();
    }
    pi_xip_free_page_mask_get(&page_mask,XIP_NB_DCACHE_PAGE);
    printf("page mask = 0x%x\n",page_mask);
    pi_xip_conf_t xip_conf = {0};
    xip_conf.ro = 0x0;
    xip_conf.page_size = XIP_PAGE_SIZE;
    xip_conf.page_mask = page_mask;
    xip_conf.data_exception_handler = tlb_exception_test;

    printf("xip page alloc done\n");
    xip_device->config = &xip_conf;

    pi_xip_tlb_init(xip_device);
    printf("xip open success\n");

    /* Verification for xip read -- should trigg exception first*/

    printf("value of xip[0]=0x%x\n",xip_buff[0][0]);
    if(!g_tlb_exception_called)
    {
        printf("no exception was called\n");
        pmsis_exit(-1);
    }
    for(int k = 0; k<NB_DEVICES; k++)
    {
        for (int j=0; j < 16; j++)
        {
            for (uint32_t i=0; i<(uint32_t) XIP_BUFFER_SIZE; i++)
            {
                if (xip_buff[k][(j*XIP_BUFFER_SIZE)+i] != rcv_buff[k*DEV_BUFFER_SIZE + (j*XIP_BUFFER_SIZE)+i])
                {
                    errors++;
                    printf("%i: %x - %x\n", i, xip_buff[k][(j*XIP_BUFFER_SIZE)+i], rcv_buff[k*DEV_BUFFER_SIZE + (j*XIP_BUFFER_SIZE)+i]);
                }
            }
            printf("j: 0x%x\n",j);
        }
        g_tlb_exception_called = 0;
    }
    printf("LRU: 0x%x\n",hal_xip_tlb_lru_get());
#endif
    pi_l2_free(buff, (uint32_t) BUFFER_SIZE);
    pi_l2_free(rcv_buff, (uint32_t) BUFFER_SIZE);
    //pi_ram_free(&ram, hyper_buff_raw, (uint32_t) BUFFER_SIZE+BUFFER_ALIGN_SIZE);
    
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
