#include "wifi_lp.h"
#include "pmsis.h"


#define PAD_UART1_RX        (PI_PAD_044)
#define PAD_UART1_TX        (PI_PAD_045)
#define PAD_RST             (PI_PAD_086)
#define PAD_CS              (PI_PAD_034)
#define PAD_GPIO_3V3_PERIPH (PI_PAD_035)


char * l_trim( char * string )
{
    char * original;
    char * p;
    int16_t trimmed;

    trimmed = 0;
    original = string;
    p = original;

    do
    {
        if  (*original != ' ' || trimmed )
        {
            trimmed = 1;
            *p++ = *original;
        }
    } while ( *original++ != 0 );

    return string;
}

void uint8_to_str( uint8_t input, char * output )
{
    uint8_t digit_pos;

    for ( digit_pos = 0; digit_pos < 3; digit_pos++ )
    {
        output[digit_pos] = ' ';
    }

    output[digit_pos--] = 0;

    while ( 1 )
    {
        output[digit_pos] = ( input % 10u ) + 48;
        input = input / 10u;
        if ( input == 0 )
        {
            break;
        }

        digit_pos--;
    }
}

void uint16_to_str( uint16_t input, char * output )
{
    uint8_t len;

    //--- the result is right justified - so we go from bottom to top
    for ( len = 0 ; len < 5 ; len++ )
    {
        output[len] = ' ';
    }

    output[len--] = 0;

    while ( 1 )
    {
        output[len] = input % 10u + 48;
        input = input / 10u;

        if ( input == 0 )
        {
            break;
        }

        len--;
    }
}

static int enable_3v3_periph(void)
{
    pi_device_t gpio_3v3_periph;
    struct pi_gpio_conf gpio_conf;

    pi_gpio_conf_init(&gpio_conf);

    gpio_conf.port = PAD_GPIO_3V3_PERIPH / 32;

    pi_open_from_conf(&gpio_3v3_periph, &gpio_conf);

    if (pi_gpio_open(&gpio_3v3_periph))
    {
        return -1;
    }

    /* set pad to gpio mode */
    pi_pad_set_function(PAD_GPIO_3V3_PERIPH, PI_PAD_FUNC1);

    /* configure gpio output */
    pi_gpio_flags_e flags = PI_GPIO_OUTPUT;
    pi_gpio_pin_configure(&gpio_3v3_periph, PAD_GPIO_3V3_PERIPH, flags);

    pi_gpio_pin_write(&gpio_3v3_periph, PAD_GPIO_3V3_PERIPH, 1);

    /* wait some time to let voltage rise */
    pi_time_wait_us(20000);

    return 0;
}


static int8_t enable_rst(void)
{
    pi_device_t gpio_rst;
    struct pi_gpio_conf gpio_rst_conf;

    pi_gpio_conf_init(&gpio_rst_conf);
    gpio_rst_conf.port = PAD_RST / 32; //GPIO86
    pi_open_from_conf(&gpio_rst, &gpio_rst_conf);

    if (pi_gpio_open(&gpio_rst))
    {
        return -1;
    }

    /* configure gpio output */
    pi_gpio_flags_e flags = PI_GPIO_OUTPUT;
    pi_gpio_pin_configure(&gpio_rst, PAD_RST, flags);

    pi_gpio_pin_write(&gpio_rst, PAD_RST, 1);
    pi_time_wait_us(20000);

    return 0;
}

static int8_t enable_cs(void)
{
    pi_device_t gpio_cs;
    struct pi_gpio_conf gpio_cs_conf;

    pi_gpio_conf_init(&gpio_cs_conf);
    gpio_cs_conf.port = PAD_CS / 32; //GPIO34
    pi_open_from_conf(&gpio_cs, &gpio_cs_conf);

    if (pi_gpio_open(&gpio_cs))
    {
        return -1;
    }

    /* configure gpio output */
    pi_gpio_flags_e flags = PI_GPIO_OUTPUT;
    pi_gpio_pin_configure(&gpio_cs, PAD_CS, flags);

    pi_gpio_pin_write(&gpio_cs, PAD_CS, 1);
    pi_time_wait_us(20000);

    return 0;
}

static void setup_pads(void)
{
    pi_pad_set_function(PAD_UART1_RX, PI_PAD_FUNC0);
    pi_pad_set_function(PAD_UART1_TX, PI_PAD_FUNC0);

    pi_pad_set_mux_group(PAD_UART1_RX, PI_PAD_MUX_GROUP_UART1_RX);
    pi_pad_set_mux_group(PAD_UART1_TX, PI_PAD_MUX_GROUP_UART1_TX);

    pi_pad_set_function(PAD_CS, PI_PAD_FUNC1);
    pi_pad_set_function(PAD_RST, PI_PAD_FUNC1);
}


int8_t lpwifi_init(struct pi_device* uart, int baudrate)
{
    if (enable_3v3_periph())
    {
        printf("3V3 PERIPH enable failed\n");
        return -1;
    }

    setup_pads();

    struct pi_uart_conf conf;

    pi_uart_conf_init(&conf);
    conf.baudrate_bps = baudrate;
    conf.enable_tx = 1;
    conf.enable_rx = 1;
    conf.uart_id = 1;
    conf.stop_bit_count = 1;
    conf.parity_mode = 0;
    pi_open_from_conf(uart, &conf);

    if (pi_uart_open(uart))
    {
        return -1;
    }

/*    int32_t timeout_id = pi_udma_timeout_alloc(PI_UDMA_TIMEOUT_MODE_TRANSFER);
    if (timeout_id == -1)
    {
        pmsis_exit(-2);
    }
    pi_uart_ioctl(&uart, PI_UART_IOCTL_ATTACH_TIMEOUT_RX, (void *) timeout_id);
*/

    return 0;
}


void lpwifi_deinit(struct pi_device* uart)
{
    pi_uart_close(uart);
}

void enable_cs_wakup()
{
    enable_cs();
    enable_rst();
    pi_time_wait_us(1000000);
}


int lpwifi_generic_write ( struct pi_device* uart, char *data_buf, uint16_t len ) 
{
    return pi_uart_write(uart, data_buf, len);
}


/* Factory default S3(CR) & S4(LF) response format character.  */
#define S1char  'O'            /*!< CR char, ASCII 0x0D. */
#define S2char  'K'            /*!< LF char, ASCII 0x0A. */
#define S3char  '\r'            /*!< CR char, ASCII 0x0D. */
#define S4char  '\n'            /*!< LF char, ASCII 0x0A. */
/* Same, but to use in string functions (strcat, strcmp, etc...). */
#define S3str  "\r"             /*!< CR string.  */
#define S4str  "\n"             /*!< LF string. */
//    pi_uart_ioctl(&uart, PI_UART_IOCTL_FLUSH, NULL);

int lpwifi_generic_read (struct pi_device* uart, char *data_buf, uint16_t max_len ) 
{   
    memset(data_buf,0,max_len);
    int read=0;
    int read_error_code=0;
    while(read<max_len){
        pi_uart_read(uart, &(data_buf[read]), 1);
        //pi_time_wait_us(5);
        read++;
        if(read > 4 
            && data_buf[read-1]==S4char 
            && data_buf[read-2]==S3char 
            && data_buf[read-3]=='K' 
            && data_buf[read-4]=='O') break;
        if(read > 5
            && data_buf[read-1]=='R' 
            && data_buf[read-2]=='O'
            && data_buf[read-3]=='R'
            && data_buf[read-4]=='R'
            && data_buf[read-5]=='E'){
                read_error_code=1;
                break;  
        } 
    }
    if(read_error_code==1){
        while(read<max_len){
            pi_uart_read(uart, &(data_buf[read]), 1);
            //pi_time_wait_us(7);
            read++;
            if(read > 2
                && data_buf[read-1]==S4char 
                && data_buf[read-2]==S3char)
                break;
        }
    }

    //data_buf[read-1]=0;
    //data_buf[read-2]=0;
    //

    return read;
}


PI_L2 char cr_lf[ 3 ] = { '\r', '\n', 0 };
PI_L2 char cmd_tot[ 2048 + 100 ];

void lpwifi_send_cmd ( struct pi_device* uart, char *cmd )
{
    int num_char=0;
    memset(cmd_tot,0,2048 +100);
    //printf("Command to send: %s\n",cmd);

    while ( *cmd != 0 )
    {
        cmd_tot[num_char++] = *cmd;
        cmd++;
    }
    cmd_tot[num_char++]=cr_lf[0];
    cmd_tot[num_char++]=cr_lf[1];
    cmd_tot[num_char]=cr_lf[2];
    pi_uart_write(uart, cmd_tot, num_char );
}

void lpwifi_send_cmd_payload(struct pi_device* uart, char *cmd, int size)
{
    int num_char = 0;
    memset(cmd_tot, 0, 2048 + 100);
    //printf("Command to send: %s\n",cmd);

    while (num_char < size)
    {
        cmd_tot[num_char++] = *cmd;
        cmd++;
    }
    cmd_tot[num_char++] = cr_lf[0];
    cmd_tot[num_char++] = cr_lf[1];
    cmd_tot[num_char]   = cr_lf[2];
    pi_uart_write(uart, cmd_tot, num_char);
}

PI_L2 char final_cmd[ DRV_BUFFER_SIZE ];
PI_L2 char check_char[2];

void lpwifi_send_cmd_with_parameter (struct pi_device* uart, char *at_cmd_buf, char *param_buf )
{
    memset(final_cmd,0,DRV_BUFFER_SIZE);
    check_char[ 0 ] = '=';
    check_char[ 1 ] = 0;
    
    strcpy( final_cmd, at_cmd_buf );
    strcat( final_cmd, check_char );
    strcat( final_cmd, param_buf );
    
    lpwifi_send_cmd( uart, final_cmd );
}

PI_L2 char final_cmd_c[ 100 ] = { 0 };
PI_L2 char check_char[ 2 ] = { '?', 0 };

void lpwifi_send_cmd_check ( struct pi_device* uart, char *at_cmd_buf )
{

    check_char[0]='?'; check_char[1]=0;
    memset(final_cmd_c,0,100);
    
    strcpy( final_cmd_c, at_cmd_buf );
    strcat( final_cmd_c, check_char );
    lpwifi_send_cmd( uart, final_cmd_c );
}


void lpwifi_factory_reset_device ( struct pi_device* uart )
{
    lpwifi_send_cmd( uart, LPWIFI_CMD_ATF );
    pi_time_wait_us(3000000); //3 sec wait
}


PI_L2 char param_buf[ 100 ];
PI_L2 char single_quote[ 2 ];
PI_L2 char comma[ 2 ];

void lpwifi_connect_to_ap ( struct pi_device* uart, char *ssid, char *password )
{
    memset(param_buf,0,100);
    single_quote[0]='\''; single_quote[1]=0;
    comma[0]=','; comma[1]=0;
    
    strcpy( param_buf, single_quote );
    strcat( param_buf, ssid );
    strcat( param_buf, single_quote );
    if ( 0 != *password  )
    {
        strcat( param_buf, comma );
        strcat( param_buf, single_quote );
        strcat( param_buf, password );
        strcat( param_buf, single_quote );
    }
    
    lpwifi_send_cmd_with_parameter( uart, LPWIFI_CMD_WFJAPA, param_buf );
}

void lpwifi_set_wifi_mode ( struct pi_device* uart, uint8_t mode )
{
    char mode_buf[ 5 ] = { 0 };
    
    if ( mode > LPWIFI_MODE_SOFT_AP )
    {
        return;
    }
    uint8_to_str ( mode, mode_buf );
    l_trim ( mode_buf );
    lpwifi_send_cmd_with_parameter( uart, LPWIFI_CMD_WFMODE, mode_buf );
}

void lpwifi_create_tcp_server ( struct pi_device* uart, uint16_t port )
{
    char port_buf[ 10 ] = { 0 };
    
    if ( 0 == port )
    {
        return;
    }
    
    uint16_to_str ( port, port_buf );
    l_trim ( port_buf );
    
    lpwifi_send_cmd_with_parameter( uart, LPWIFI_CMD_TRTS, port_buf );
}

void lpwifi_create_udp_socket ( struct pi_device* uart, uint16_t port )
{
    char port_buf[ 10 ] = { 0 };
    
    if ( 0 == port )
    {
        return;
    }
    
    uint16_to_str ( port, port_buf );
    l_trim ( port_buf );
    
    lpwifi_send_cmd_with_parameter( uart, LPWIFI_CMD_TRUSE, port_buf );
}
