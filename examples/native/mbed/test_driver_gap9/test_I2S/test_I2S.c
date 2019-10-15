#include <stdio.h>
#include "cmsis.h"
#include "gap_common.h"

#define BUFFER_SIZE       128

uint8_t tx_buffer[BUFFER_SIZE];
uint8_t rx_buffer[BUFFER_SIZE];

/* Datasize. */
#define DATASIZE_08        0x0
#define DATASIZE_16        0x1
#define DATASIZE_32        0x2

/* Check for 1, 2 & 4 words. */
#define WORDS_PER_PHASE(x)  ((uint32_t) x)

/* Check for 8, 16 & 32 bits. */
#define BITS_PER_WORD(x)    ((uint32_t) x)

void i2s_clkcfg_setup(uint8_t mst_div, uint8_t slv_div, uint8_t cmn_div,
                      uint8_t pdm_en,  uint8_t mst_en,  uint8_t slv_en,
                      uint8_t slv_ext, uint8_t slv_num,
                      uint8_t mst_ext, uint8_t mst_num)
{
    I2S->CLKCFG_SETUP = I2S_CLKCFG_MASTER_CLK_DIV(mst_div) |
        I2S_CLKCFG_SLAVE_CLK_DIV  (slv_div) |
        I2S_CLKCFG_COMMON_CLK_DIV (cmn_div) |
        I2S_CLKCFG_PDM_CLK_EN     (pdm_en)  |
        I2S_CLKCFG_MASTER_CLK_EN  (mst_en)  |
        I2S_CLKCFG_SLAVE_CLK_EN   (slv_en)  |
        I2S_CLKCFG_SLAVE_SEL_EXT  (slv_ext) |
        I2S_CLKCFG_SLAVE_SEL_NUM  (slv_num) |
        I2S_CLKCFG_MASTER_SEL_EXT (mst_ext) |
        I2S_CLKCFG_MASTER_SEL_NUM (mst_num);
}

void i2s_master_setup(uint8_t words, uint32_t bits_word, uint8_t lsb,
                      uint8_t both_chan, uint8_t enable)
{
    I2S->MST_SETUP = I2S_MST_WORDS(words) |
        I2S_MST_BITS_WORD (bits_word) |
        I2S_MST_LSB       (lsb)       |
        I2S_MST_2CH       (both_chan) |
        I2S_MST_CLK_EN    (enable);
}

void i2s_slave_setup(uint8_t words, uint32_t bits_word, uint8_t lsb,
                     uint8_t both_chan, uint8_t enable)
{
    I2S->SLV_SETUP = I2S_SLV_WORDS(words) |
        I2S_SLV_BITS_WORD (bits_word) |
        I2S_SLV_LSB       (lsb)       |
        I2S_SLV_2CH       (both_chan) |
        I2S_SLV_CLK_EN    (enable);
}

void i2s_send(void *buffer, uint32_t size, uint8_t datasize)
{
    I2S->UDMA_I2S.TX_SADDR = (uint32_t) buffer;
    I2S->UDMA_I2S.TX_SIZE  = size;
    I2S->UDMA_I2S.TX_CFG   = UDMA_CFG_EN(1);

    ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);
}

void i2s_receive(void *buffer, uint32_t size, uint8_t datasize)
{
    I2S->UDMA_I2S.RX_SADDR = (uint32_t) buffer;
    I2S->UDMA_I2S.RX_SIZE  = size;
    I2S->UDMA_I2S.RX_CFG   = UDMA_CFG_EN(1);

    ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);
}

void i2s_tx(uint8_t *buffer, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++) {
        buffer[i] = i;
    }

    for (int datasize = DATASIZE_08; datasize < DATASIZE_32; datasize++)
    {
        for (int nb_chan = 0; nb_chan < 2; nb_chan++)
        {
            for (int lsb = 0; lsb < 2; lsb++)
            {
                for (int wpp = WORDS_PER_PHASE(1); wpp <= (int) WORDS_PER_PHASE(4); wpp = wpp << 1)
                {
                    for (int bpw = BITS_PER_WORD(8); bpw <= (int) BITS_PER_WORD(0x20); bpw = bpw << 1)
                    {
                        printf("Sending on 0x%x channel(s) : LSB 0x%x WPP 0x%x BPW 0x%x DATASIZE 0x%x :\n",
                               nb_chan, lsb, wpp, bpw, 0x8 << datasize);

                        /* Setup first Master channel. */
                        i2s_master_setup(wpp, bpw, lsb, nb_chan, 1);

                        /* clock configurarion. */
                        i2s_clkcfg_setup(0x1f, 0x1f, 0x00, 0, 1, 0, 0, 0, 0, 0);

                        /* Send I2S data.*/
                        i2s_send(tx_buffer, size, datasize);
                        printf("Done\n");

                        /* Disable Master/Slave before setting it again. */
                        i2s_clkcfg_setup(0x1f, 0x1f, 0x00, 0, 0, 0, 0, 0, 0, 0);
                    }
                }
            }
        }
    }
}

int test_i2s()
{
    int error = 0;

    /* Clock gating disable */
    UDMA_GC->CG |= (1 << PER_ID_I2S );

    /* Attach event unit for end interrupt */
    SOC_EU_SetFCMask(PER_ID_I2S << 2);
    SOC_EU_SetFCMask((PER_ID_I2S << 2) + 1);

    i2s_tx(tx_buffer, BUFFER_SIZE);

    /* Clock gating enable */
    UDMA_GC->CG &= ~(1 << PER_ID_I2S );

    return error;
}

int main()
{
    int error = 0;

    error = test_i2s();

    if (error)
        printf("Test failed\n");
    else
        printf("Test success\n");

    return 0;
}
