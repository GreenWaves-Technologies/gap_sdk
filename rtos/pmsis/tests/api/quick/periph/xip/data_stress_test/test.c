/* PMSIS includes */
#include "pmsis.h"
#include "bsp/ram.h"
#include "bsp/ram/hyperram.h"

/** Internal includes - we don't want to expose xip in user api **/
//#include "pmsis/implem/drivers/xip/xip_internal.h"

/* Variables used. */
#define XIP_PAGE_SIZE    (0) //(XIP_PAGE_SIZE_512B)
#define BUFFER_ALIGN_SIZE      ( 512 << XIP_PAGE_SIZE )
#define BUFFER_SIZE ( BUFFER_ALIGN_SIZE * 2)

#define END_OF_L2_ADDR ((uint32_t*)0x1C101200)
#define SPECIAL_RANGE  ((uint32_t*)0x1F000000)

#define XIP_NB_DCACHE_PAGE 8

pi_task_t task_block_illegal;

uint32_t nb_xip_irq_call = 0;
uint32_t nb_ro_violation = 0;
#if 0
void xip_irq_user_handler (void *arg)
{
    uint32_t page = (uint32_t)arg;
    pi_xip_data_err_t *data_err = arg;
    printf("user xip irq handler: page=0x%x\n",data_err->error_addr);
    nb_xip_irq_call++;
}

void fc_ro_violation_handler(void *arg)
{
    uint32_t page = (uint32_t)arg;
    pi_xip_data_err_t *data_err = arg;
    printf("fc_ro_violation_handler: page=0x%x\n",data_err->error_addr);
    nb_ro_violation++;
}
#endif
static uint8_t *buff, *rcv_buff;
static uint32_t hyper_buff = 0;
static struct pi_device ram = {0};
static struct pi_hyperram_conf conf = {0};

static uint8_t *xip_buff = (uint8_t*)0x21000000;


#if defined(USE_CLUSTER) || defined(CLUSTER_ILL_DATA_ACCESS)
void cluster_entry_no_dma(void *arg)
{
    printf("cluster_no_dma entry\n");
    uint32_t *errors = (uint32_t)arg;
#if 1
    /* Verification for xip read */
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        if (xip_buff[i] != rcv_buff[i])
        {
            printf("%i: %x - %x \n", i, xip_buff[i], rcv_buff[i]);
            (*errors)++;
        }
    }
#endif
    printf("cluster_no_dma done\n");
}

#if defined(CLUSTER_DMA)
void cluster_entry_dma(void *arg)
{
    uint8_t *dma_rcv_buff = pmsis_l1_malloc(BUFFER_SIZE);
    uint32_t *errors = (uint32_t)arg;
    /* dma xip transfer */
    pi_cl_dma_copy_t copy;
    copy.dir = PI_CL_DMA_DIR_EXT2LOC;
    copy.merge = 0;
    copy.size = (uint16_t) BUFFER_SIZE;
    copy.id = 0;
    copy.ext = (uint32_t) xip_buff;
    copy.loc = (uint32_t) dma_rcv_buff;

    pi_cl_dma_memcpy(&copy);
    pi_cl_dma_wait(&copy);

    /* Verification for xip read dma */
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        if (dma_rcv_buff[i] != rcv_buff[i])
        {
            printf("%i: %x - %x \n", i, dma_rcv_buff[i], rcv_buff[i]);
            (*errors)++;
        }
    }
    printf("cluster_dma done\n");
}
#endif
#endif

#ifdef CLUSTER_ILL_DATA_ACCESS

void cluster_entry_no_dma_illegal(void *arg)
{
    printf("cluster_no_dma entry\n");
    uint32_t *errors = (uint32_t)arg;
#if 1
    /* Verification for xip read */
    for (uint32_t i=BUFFER_SIZE; i<(uint32_t) BUFFER_SIZE+10; i++)
    {
        if (xip_buff[i] == rcv_buff[i])
        {
            printf("%i: %x - %x \n", i, xip_buff[i], rcv_buff[i]);
            (*errors)++;
        }
    }
#endif
    printf("cluster_no_dma done\n");
}

#ifdef CLUSTER_DMA
    #define NB_XIP_IRQ_CALL 2
void cluster_entry_dma_illegal(void *arg)
{
    printf("cluster_dma_illegal entry\n");
    uint8_t *dma_rcv_buff = pmsis_l1_malloc(BUFFER_SIZE+10);
    printf("0\n");
    uint32_t *errors = (uint32_t)arg;
    /* dma xip transfer */
    pi_cl_dma_copy_t copy;
    copy.dir = PI_CL_DMA_DIR_EXT2LOC;
    copy.merge = 0;
    copy.size = (uint16_t) 10;
    copy.id = 0;
    copy.ext = (uint32_t) xip_buff+BUFFER_SIZE;
    copy.loc = (uint32_t) dma_rcv_buff;

    printf("1\n");
    pi_cl_dma_memcpy(&copy);
    printf("2\n");
    pi_cl_dma_wait(&copy);
    printf("3\n");

    /* Verification for xip read dma */
    for (uint32_t i=BUFFER_SIZE; i<(uint32_t) BUFFER_SIZE+10; i++)
    {
        if (dma_rcv_buff[i] == rcv_buff[i])
        {
            printf("%i: %x - %x \n", i, dma_rcv_buff[i], rcv_buff[i]);
            (*errors)++;
        }
    }
    printf("cluster_dma_illegal done\n");
}
#else
    #define NB_XIP_IRQ_CALL 1
#endif

#endif

void test_hyper_ram(void)
{
    printf("Entering main controller\n\n");

    uint32_t errors = 0;

    buff = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE+10);
    if (buff == NULL)
    {
        printf("buff alloc failed !\n");
        pmsis_exit(-1);
    }

    rcv_buff = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE+10);
    if (rcv_buff == NULL)
    {
        printf("rcv_buff alloc failed !\n");
        pmsis_exit(-2);
    }

    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE+10; i++)
    {
        buff[i] = i & 0xFF;
        rcv_buff[i] = 0;
    }

    /* Init & open flash. */
    pi_hyperram_conf_init(&conf);
    (&conf)->xip_en = 1; // we want to use xip on this device
    (&conf)->hyper_itf = 1; // we want to use xip on this device
    //(&conf)->baudrate = 50*1000*1000;
    pi_open_from_conf(&ram, &conf);
    if (pi_ram_open(&ram))
    {
        printf("Error flash open !\n");
        pmsis_exit(-3);
    }
    /* Retrieve info on flash. */
    uint32_t hyper_buff_raw = 0;
    if (pi_ram_alloc(&ram, &hyper_buff_raw, (uint32_t) BUFFER_SIZE+BUFFER_ALIGN_SIZE))
    {
        printf("Ram malloc failed !\n");
        pmsis_exit(-4);
    }
    else
    {
        printf("Ram allocated : %lx %ld.\n", hyper_buff, (uint32_t) BUFFER_SIZE+BUFFER_ALIGN_SIZE);
    }
    hyper_buff = (hyper_buff_raw + BUFFER_ALIGN_SIZE) & (~(BUFFER_ALIGN_SIZE-1));
        //((flash_info.flash_start + flash_info.sector_size - 1) &
                 // ~(flash_info.sector_size - 1)) + 128;
    printf("hyper_buff= 0x%x\n",hyper_buff);
    /* Erase a sector in flash, write a buffer then read back from flash. */
    pi_ram_write(&ram, hyper_buff, buff, (uint32_t) BUFFER_SIZE);
    printf("Write sync done.\n");
    pi_ram_read(&ram, hyper_buff, rcv_buff, (uint32_t) BUFFER_SIZE);
    printf("Read sync done.\n");

    /* Verification for normal read / write */
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        if (buff[i] != rcv_buff[i])
        {
            errors++;
            printf("%2x-%2x ", buff[i], rcv_buff[i]);
        }
    }
    uint32_t page_mask = 0;
#if 1
    pi_device_t *xip_device = pmsis_l2_malloc(sizeof(pi_device_t));
    for( int i = 8; i<8+XIP_NB_DCACHE_PAGE; i++)
    {
        pi_xip_dcache_page_alloc(i, XIP_PAGE_SIZE);
        //printf("dcache page addr= 0x%x\n",hal_xip_dcache_page_l2_addr_get(i));
    }
    pi_xip_free_page_mask_get(&page_mask,XIP_NB_DCACHE_PAGE);
    printf("page mask = 0x%x\n",page_mask);
    pi_xip_conf_t xip_conf = {0};
    xip_conf.ro = 0;
    xip_conf.per_id = XIP_DEVICE_HYPER1;
    xip_conf.page_size = XIP_PAGE_SIZE;
    xip_conf.page_mask = page_mask;
#if defined(CLUSTER_ILL_DATA_ACCESS)
    xip_conf.cl_irq_handler = xip_irq_user_handler;
#endif

#if defined(XIP_RO_FC)
    // TODO
    xip_conf.data_exception_handler = fc_ro_violation_handler;
#endif
    printf("xip page alloc done\n");

    uint32_t mount_size = BUFFER_SIZE / (512 << XIP_PAGE_SIZE);

    xip_device->config = &xip_conf;
    int ret = 0;
    if((ret =pi_xip_mount(xip_device,xip_buff,hyper_buff,mount_size,0)))
    {
        printf("xip open failed: %x\n",ret);
        pmsis_exit(-1);
    }
#endif
    printf("xip open success\n");
    /* Verification for xip read */
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        if (xip_buff[i] != rcv_buff[i])
        {
            errors++;
            printf("%i: %x - %x \n", i, xip_buff[i], rcv_buff[i]);
        }
    }
    printf("xip verif done\n");


#if defined(XIP_RO_FC)
    /* Verification for xip ro violation */
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        xip_buff[i] = 0x55;
        if (xip_buff[i] == 0x55)
        {
            errors++;
        }
    }
    if(nb_ro_violation != BUFFER_SIZE)
    {
        errors++;
    }
#endif

#if defined(USE_CLUSTER) || defined(CLUSTER_ILL_DATA_ACCESS)
    struct pi_device cluster_dev;
    struct pi_cluster_conf conf;
    struct pi_cluster_task cluster_task;

    pi_cluster_conf_init(&conf);
    conf.id = 0;   

    pi_open_from_conf(&cluster_dev, &conf);

    printf("cluster open\n");
    pi_cluster_open(&cluster_dev);
    

#if defined(CLUSTER_ILL_DATA_ACCESS)
    // no dma
    pi_cluster_task(&cluster_task, cluster_entry_no_dma_illegal, (void *)&errors);

    pi_task_block(&task_block_illegal);
    printf("cluster send task no dma\n");
    pi_cluster_send_task_to_cl_async(&cluster_dev, &cluster_task,
            &task_block_illegal);
    printf("async out\n");
    pi_task_wait_on(&task_block_illegal);

    printf("Going to close after illegal access\n");
    pi_cluster_close(&cluster_dev);
    printf("cluster closed after illegal access\n");
    // restart cluster from a clean state
    pi_cluster_conf_init(&conf);
    conf.id = 0;   

    pi_open_from_conf(&cluster_dev, &conf);
    printf("cluster open again for dma\n");
    pi_cluster_open(&cluster_dev);
#if 1
#ifdef CLUSTER_DMA
    // dma
    pi_cluster_task(&cluster_task, cluster_entry_dma_illegal, (void *)&errors);

    printf("cluster send task dma\n");
    pi_task_block(&task_block_illegal);
    pi_cluster_send_task_to_cl_async(&cluster_dev, &cluster_task,
            &task_block_illegal);
    pi_task_wait_on(&task_block_illegal);
    printf("Going to close after illegal access\n");
    pi_cluster_close(&cluster_dev);
    printf("cluster closed after illegal access\n");
    // restart cluster from a clean state
    pi_cluster_conf_init(&conf);
    conf.id = 0;   

    pi_open_from_conf(&cluster_dev, &conf);
    printf("cluster open again\n");
    pi_cluster_open(&cluster_dev);
#endif
#endif
#endif

    // no dma
    pi_cluster_task(&cluster_task, cluster_entry_no_dma, (void *)&errors);

    printf("cluster send task no dma\n");
    pi_cluster_send_task_to_cl(&cluster_dev, &cluster_task);

#ifdef CLUSTER_DMA   
    // dma
    pi_cluster_task(&cluster_task, cluster_entry_dma, (void *)&errors);

    printf("cluster send task dma\n");
    pi_cluster_send_task_to_cl(&cluster_dev, &cluster_task);
#endif

    pi_cluster_close(&cluster_dev);
#endif

#if defined(CLUSTER_ILL_DATA_ACCESS)
    if(nb_xip_irq_call != NB_XIP_IRQ_CALL)
    {
        printf("irq was not called enough:0x%x\n",nb_xip_irq_call);
        errors++;
    }
#endif

    pmsis_l2_malloc_free(buff, (uint32_t) BUFFER_SIZE);
    pmsis_l2_malloc_free(rcv_buff, (uint32_t) BUFFER_SIZE);
    
    pi_ram_close(&ram);

    printf("\nHyperflash xip transfer done with %ld error(s) !\n", errors);
    printf("\nTest %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Hyperram Test ***\n\n");
    return pmsis_kickoff((void *) test_hyper_ram);
}
