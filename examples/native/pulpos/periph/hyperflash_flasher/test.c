#include "rt/rt_api.h"
#include "hyper_flash.h"

#define MAX_BUFF_SIZE (350*1024)
#define PAGE_SIZE 256
#define NBR_PAGE_MAX   ( MAX_BUFF_SIZE / PAGE_SIZE )
#define BUFFER_SIZE             128
#define DEVICE_READY_OFFSET     7
#define ERASE_STATUS_OFFSET     5
#define PROGRAM_STATUS_OFFSET   4

// Write and read address
#define SA                      0x4000

static rt_event_sched_t sched;
static rt_flash_t *flash;

typedef struct {
	unsigned int	L2_Addr;
	unsigned int 	Size;
	unsigned int	Ready;
    unsigned int    Exit;
    unsigned int    eotFlash;
} fileDescriptor;

typedef struct {
    unsigned short data;
    unsigned short addr;
} cmdSeq;

/* // Chip erase */
/* RT_L2_DATA  cmdSeq eraseSeq[6] = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x80, 0x555}, {0xAA, 0x555}, {0x55, 0x2AA}, {0x10, 0x555}}; */

// Sector erase
RT_L2_DATA  cmdSeq eraseSeq[6] = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x80, 0x555}, {0xAA, 0x555}, {0x55, 0x2AA}, {0x30, SA}};
RT_L2_DATA  cmdSeq VCRSeq[4]   = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x38, 0x555}, {0x8e0b, 0x0}};
RT_L2_DATA  cmdSeq readSeq     = {0x70, 0x555};

// Write 512/4 = 128 word to Sector addr 0x4xxx
RT_L2_DATA  cmdSeq WBPSeq[4]   = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x25, SA}, {BUFFER_SIZE - 1, SA}};
RT_L2_DATA  cmdSeq WPSeq[3]   = {{0xAA, 0x555}, {0x55, 0x2AA}, {0xA0, 0x555}};
RT_L2_DATA  cmdSeq BPCSeq      = {0x29, SA};

// Do not use high 16 bits for txHyperSamples
RT_L2_DATA  fileDescriptor flashCopyHeader;
RT_L2_DATA  unsigned int checkSum = 0;

RT_L2_DATA  unsigned char L2_Buffer[MAX_BUFF_SIZE];
RT_L2_DATA  unsigned short status_reg = 0;
RT_FC_DATA  unsigned int nPage = 0;
RT_FC_DATA  unsigned int nPage_check = 0;

unsigned short read_reg(rt_flash_t  *flash)
{
    // Read status register
    hyper_flash_write(flash, (unsigned int) &readSeq.data, readSeq.addr << 1, 0);
    hyper_flash_read (flash, (unsigned int) &status_reg, 0, 2, 0);

    return status_reg;
}

void conf_flash(rt_flash_t  *flash)
{
    // Set VCR to 5 delay cycles
    for (int i = 0; i < 4; i++)
        hyper_flash_write(flash, (unsigned int) &VCRSeq[i].data, VCRSeq[i].addr << 1, 0);
}

void erase_page_in_flash(rt_flash_t  *flash)
{
    // Erase sector 0x0E00
    for ( int i = 0; i < 6; i++)
        hyper_flash_write(flash, (unsigned int) &eraseSeq[i].data, eraseSeq[i].addr << 1, 0);
}

void write_page_in_flash(rt_flash_t  *flash, unsigned char *l2_buff, unsigned int flash_addr, unsigned int len)
{
    int i = 0;
    // Write to Buffer command sequence
    for (i = 0; i < 3; i++)
        hyper_flash_write(flash, (unsigned int) &WPSeq[i].data, WPSeq[i].addr << 1, 0);

    // Load buffer data
    rt_event_t *event = rt_event_get_blocking(NULL);
    hyper_flash_burst_write(flash, (unsigned int) l2_buff, (flash_addr) << 1, len, event);
    rt_event_wait(event);
}

void read_page_from_flash(rt_flash_t  *flash, unsigned char *l2_buff, unsigned int flash_addr, unsigned int len)
{
    rt_event_t *event = rt_event_get_blocking(NULL);
    hyper_flash_read(flash, (unsigned int) l2_buff, flash_addr << 1, len, event);
    rt_event_wait(event);
}

int wait_process_end(rt_flash_t *flash, unsigned int err_bit)
{
    unsigned short reg;
    // Wait the end of process
    // Status Register (SR)
    // bit 4 -> program status bit, 0-success ; 1-failure
    // bit 5 -> erase status bit,   0-success ; 1-failure
    // bit 7 -> device ready bit
    do {
        reg = read_reg(flash);

        if ( reg & (1 << err_bit))
        {
#ifdef DEBUG
            printf("ERR bit = 1, an error occurs in  operation\n");
#endif
            return 1;
        }
    } while( !(reg & (1 << DEVICE_READY_OFFSET)));

    return 0;
}

static void hyperflash_init(rt_flash_t *flash){
    // Enable uDMA HYPER
    if( hyper_init() ) printf("init error.\n");

    // Configuration
    conf_flash(flash);
    printf("conf finished.\n");

    // Erase chip
    erase_page_in_flash(flash);
    wait_process_end(flash,  ERASE_STATUS_OFFSET);

#ifdef DEBUG
    printf("Erase finished.\n");
#endif
}

void calculCheckSum(unsigned int sizeData){
    unsigned int i;
    unsigned int check = 0;
    checkSum = 0;
    for (i=0; i<sizeData; i++){
        check += L2_Buffer[i];
    }
    checkSum = check;
#ifdef DEBUG
    printf("checkSum: %X\n", check);
#endif
}

int write_to_hyperflash(rt_flash_t *flash)
{
    unsigned int i;
    unsigned int j;
    unsigned int nbrPage;
    unsigned int sizeData = flashCopyHeader.Size;
    unsigned int sizeLast = 0;

#ifdef DEBUG
    // Initialize the samples
    for (i = 0; i< BUFFER_SIZE; i++)
        L2_Buffer[i] = i;
    sizeData = BUFFER_SIZE;
#endif
    if (sizeData%256){
        nbrPage = (sizeData/256) + 1;
        sizeLast = sizeData%256;
    }else{
        nbrPage = (sizeData/256);
    }
#ifdef DEBUG
    printf ("%d bytes of data, %d Page would be loaded, %d bytes in last page\n", sizeData, nbrPage, sizeLast);
#endif
    calculCheckSum(sizeData);

    // Write buffer
    for (i=0; i<nbrPage; i++){
        if (sizeLast && i == (nbrPage-1))
            write_page_in_flash(flash, &L2_Buffer[i*PAGE_SIZE], ((nPage * PAGE_SIZE)), sizeLast);
        else
            write_page_in_flash(flash, &L2_Buffer[i*PAGE_SIZE], ((nPage * PAGE_SIZE)), PAGE_SIZE);
        wait_process_end(flash,  PROGRAM_STATUS_OFFSET);
        nPage++;
    }

#ifdef DEBUG
    printf("Write finished, %d pages should be writen, %d pages has been done, with i = %d\n", nbrPage, nPage, i);
#endif

    for (i=0; i<MAX_BUFF_SIZE; i++)
        L2_Buffer[i] = 0xff;


    // Read buffer burst
    for (i=0; i<nbrPage; i++){
        if (sizeLast && i == (nbrPage-1))
            read_page_from_flash(flash, &L2_Buffer[i*PAGE_SIZE], ((nPage_check * PAGE_SIZE)), sizeLast);
        else
            read_page_from_flash(flash, &L2_Buffer[i*PAGE_SIZE], ((nPage_check * PAGE_SIZE)), PAGE_SIZE);
        nPage_check++;
    }

    // Dsnable uDMA HYPER
    hyper_close();

    int error = 0;
    unsigned int check = 0;
    for (i=0; i<sizeData; i++) {
        check += L2_Buffer[i];
    }

    if (check !=checkSum){
        error++;
    }

#ifdef DEBUG
    if (error) printf("Test FAILED with %d errors: checkSum tx %X - checkSum rx %X\n", error, checkSum, check);
    else printf("Test SUCCESS with check: %X\n", check);
#endif

    flashCopyHeader.eotFlash = 1;

    return error;
}

int main()
{
  int error = 0;
  printf ("test start\n");

  rt_event_sched_init(&sched);
  if (rt_event_alloc(&sched, 4)) return -1;

  rt_flash_conf_t flash_conf;
  rt_flash_conf_init(&flash_conf);

  flash = rt_flash_open("hyperflash", &flash_conf, NULL);
  if (flash == NULL){
    printf("Hyper flash open failed\n");
    return -1;
  }
  hyperflash_init(flash);
  flashCopyHeader.Exit = 0;

#ifndef DEBUG
  while(((volatile fileDescriptor *) &flashCopyHeader)->Exit == 0){

      flashCopyHeader.Ready = 0;
      printf("Waiting for data copy 2 L2\n");
      while (((volatile fileDescriptor *) &flashCopyHeader)->Ready == 0) {
          if(((volatile fileDescriptor *) &flashCopyHeader)->Exit != 0)
              return error;
      };
#endif
      flashCopyHeader.L2_Addr = (unsigned int) &L2_Buffer;
      error += write_to_hyperflash(flash);
#ifndef DEBUG
      while (((volatile fileDescriptor *) &flashCopyHeader)->eotFlash == 1) {};        // Wait for check the EOT from PC.
  }
#endif

  if (error)
  {
#ifdef DEBUG
    printf("Test failed\n");
#endif
    return -1;
  }
  else
  {
#ifdef DEBUG
    printf("Test success\n");
#endif
    return 0;
  }
}
