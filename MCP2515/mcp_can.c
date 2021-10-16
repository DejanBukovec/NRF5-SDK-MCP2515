#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "nrf_drv_spi.h"
#include "bsp.h"
#include "app_timer.h"
#include "nordic_common.h"
#include "mcp_can.h"

#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

static volatile bool spi_xfer_done = true;
mcp_can_t m_mcp_can;

#define SPI_CAN_INSTANCE  0                                                   /**< SPI instance index. */
static const nrf_drv_spi_t can_spi = NRF_DRV_SPI_INSTANCE(SPI_CAN_INSTANCE);  /**< SPI instance. */

/**@brief Function for handling MCP2515 INT Pin events.
 *
 * @param[in] 
 */ 
void mcp2515_int_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (nrf_gpio_pin_read(pin))
    {
        nrf_gpio_pin_set(BSP_LED_3);

        #if MCP2515_DEBUG_MODE
            NRF_LOG_DEBUG("mcp2515_int_pin_handler State: LO To HI");
        #endif        
    }
    else
    {
        nrf_gpio_pin_clear(BSP_LED_3);
            
        #if MCP2515_DEBUG_MODE
            NRF_LOG_DEBUG("mcp2515_int_pin_handler State: HI To LO"); 
        #endif
    }
}

void spi_can_event_handler(nrf_drv_spi_evt_t const * p_event, void * p_context)
{	
    switch(p_event->type)
    {
	case NRF_DRV_SPI_EVENT_DONE:
            spi_xfer_done = true;
            #if MCP2515_DEBUG_MODE
                NRF_LOG_DEBUG("SPI transfer complete"); 
            #endif
            break;
		
	default:
            break;
    }
}

void mcp_spi_init()
{
    uint32_t err_code = NRF_SUCCESS;

    mcp_can_setcs(SPI_SS_PIN);

    nrf_drv_gpiote_in_config_t mcp2515_int_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    mcp2515_int_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(MCP2515_PIN_INT, &mcp2515_int_config, mcp2515_int_pin_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(MCP2515_PIN_INT, true);

    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.frequency = NRF_DRV_SPI_FREQ_125K;
    spi_config.mode = NRF_DRV_SPI_MODE_0;
    spi_config.ss_pin   = SPI_SS_PIN;
    spi_config.miso_pin = SPI_MISO_PIN;
    spi_config.mosi_pin = SPI_MOSI_PIN;
    spi_config.sck_pin  = SPI_SCK_PIN;
    spi_config.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;

    err_code = nrf_drv_spi_init(&can_spi, &spi_config, spi_can_event_handler, NULL);
    APP_ERROR_CHECK(err_code);

    #if MCP2515_DEBUG_MODE
        NRF_LOG_DEBUG("mcp_spi_init complete"); 
    #endif
}

void mcp2515_select()   
{
    nrf_gpio_pin_clear(m_mcp_can.m_cs);
}

void mcp2515_unselect()
{
    nrf_gpio_pin_set(m_mcp_can.m_cs);
}

void mcp2515_reset()
{
    mcp2515_select();

    uint8_t m_tx_buf[1] = {MCP_RESET};
    uint8_t m_length = sizeof(m_tx_buf);
    
    spi_xfer_done = false;

    APP_ERROR_CHECK(nrf_drv_spi_transfer(&can_spi, m_tx_buf, m_length, NULL, 0));

    while (!spi_xfer_done)
    {
        if (nrf_sdh_is_enabled())
        {
            sd_app_evt_wait();
        }
        else
        {
            __WFE();
        }
    }

    mcp2515_unselect();
}

uint8_t mcp2515_readRegister(const uint8_t address)
{
    mcp2515_select();

    uint8_t m_tx_buf[] = {MCP_READ, address, 0x00};
    uint8_t m_tx_length = sizeof(m_tx_buf);

    uint8_t m_rx_buf[3];      
    uint8_t m_rx_length = sizeof(m_rx_buf);

    memset(m_rx_buf, 0, m_rx_length);
        
    spi_xfer_done = false;

    APP_ERROR_CHECK(nrf_drv_spi_transfer(&can_spi, m_tx_buf, m_tx_length, m_rx_buf, m_rx_length));

    while (!spi_xfer_done)
    {
        if (nrf_sdh_is_enabled())
        {
            sd_app_evt_wait();
        }
        else
        {
            __WFE();
        }
    }

    mcp2515_unselect();

    return m_rx_buf[2];  
}

void  mcp2515_readRegisterS(const uint8_t address, uint8_t values[], const uint8_t n)
{
    uint8_t i;

    mcp2515_select();

    uint8_t m_tx_buf[] = {MCP_READ, address};
    uint8_t m_tx_length = sizeof(m_tx_buf);
        
    uint8_t m_rx_buf[n+2];  
    uint8_t m_rx_length = sizeof(m_rx_buf);
           
    memset(m_rx_buf, 0, m_rx_length);
        
    spi_xfer_done = false;

    APP_ERROR_CHECK(nrf_drv_spi_transfer(&can_spi, m_tx_buf, m_tx_length, m_rx_buf, m_rx_length));

    while (!spi_xfer_done)
    {
        if (nrf_sdh_is_enabled())
        {
            sd_app_evt_wait();
        }
        else
        {
            __WFE();
        }
    }  

    for (i=0; i<n && i<CAN_MAX_CHAR_IN_MESSAGE-2; i++)
    {
        values[i] = m_rx_buf[i+2];
    }

    mcp2515_unselect();

    return; 
}

void  mcp2515_setRegister(const uint8_t address, const uint8_t value)
{
    mcp2515_select();

    uint8_t m_tx_buf[3] = {MCP_WRITE, address, value};
    uint8_t m_length = sizeof(m_tx_buf);
    
    spi_xfer_done = false;

    APP_ERROR_CHECK(nrf_drv_spi_transfer(&can_spi, m_tx_buf, m_length, NULL, 0));

    while (!spi_xfer_done)
    {
        if (nrf_sdh_is_enabled())
        {
            sd_app_evt_wait();
        }
        else
        {
            __WFE();
        }
    }

    mcp2515_unselect();
}

void mcp2515_setRegisterS(const uint8_t address, const uint8_t values[], const uint8_t n)
{
    mcp2515_select();

    uint8_t tx_buf[2] = {MCP_WRITE, address};
    uint8_t m_tx_length = 2+n * sizeof(uint8_t);

    uint8_t* m_tx_buf = nrf_malloc(2+n * sizeof(uint8_t));

    memcpy(m_tx_buf, tx_buf, 2 * sizeof(uint8_t));                  //copy 2 uint8_t from tx_buf to m_tx_buf
    memcpy(m_tx_buf + 2, values, n * sizeof(uint8_t));              //copy n uint8_t from values to m_tx_buf
      
    spi_xfer_done = false;

    APP_ERROR_CHECK(nrf_drv_spi_transfer(&can_spi, m_tx_buf, m_tx_length, NULL, 0));

    while (!spi_xfer_done)
    {
        if (nrf_sdh_is_enabled())
        {
            sd_app_evt_wait();
        }
        else
        {
            __WFE();
        }
    }

    nrf_free(m_tx_buf);

    mcp2515_unselect();
}

void  mcp2515_modifyRegister(const uint8_t address, const uint8_t mask, const uint8_t data)
{
    mcp2515_select();
	       
    uint8_t m_tx_buf[4] = {MCP_BITMOD, address, mask, data};
    uint8_t m_length = sizeof(m_tx_buf);
  
    spi_xfer_done = false;

    APP_ERROR_CHECK(nrf_drv_spi_transfer(&can_spi, m_tx_buf, m_length, NULL, 0));

    while (!spi_xfer_done)
    {
        if (nrf_sdh_is_enabled())
        {
            sd_app_evt_wait();
        }
        else
        {
            __WFE();
        }
    }        

    mcp2515_unselect();
}

uint8_t  mcp2515_readStatus(void)                             
{
    mcp2515_select();

    uint8_t m_tx_buf[2] = {MCP_READ_STATUS, 0x00};
    uint8_t m_length = sizeof(m_tx_buf);

    uint8_t m_rx_buf[2]; 
    
    memset(m_rx_buf, 0, m_length);
        
    spi_xfer_done = false;

    APP_ERROR_CHECK(nrf_drv_spi_transfer(&can_spi, m_tx_buf, m_length, m_rx_buf, m_length));

    while (!spi_xfer_done)
    {
        if (nrf_sdh_is_enabled())
        {
            sd_app_evt_wait();
        }
        else
        {
            __WFE();
        }
    }   

    mcp2515_unselect();

    return m_rx_buf[1];
}

uint8_t  mcp2515_setCANCTRL_Mode(const uint8_t newmode)
{
    uint8_t i;
    
    #if MCP2515_DEBUG_MODE
        NRF_LOG_DEBUG("Modifying MCP_CANCTRL"); 
    #endif

    mcp2515_modifyRegister(MCP_CANCTRL, MODE_MASK, newmode);
    nrf_delay_ms(10);

    #if MCP2515_DEBUG_MODE
        NRF_LOG_DEBUG("Done Modifying MCP_CANCTRL"); 
        NRF_LOG_DEBUG("Reading MCP_CANCTRL");
    #endif

    i = mcp2515_readRegister(MCP_CANCTRL);
    nrf_delay_ms(10);

    #if MCP2515_DEBUG_MODE
        NRF_LOG_DEBUG("Done reading MCP_CANCTRL"); 
    #endif

    i &= MODE_MASK;

    if ( i == newmode ) 
    {
        return MCP2515_OK;
    }

    return MCP2515_FAIL;
}

uint8_t  mcp2515_configRate(const uint8_t canSpeed, const uint8_t clock)            
{
    uint8_t set, cfg1, cfg2, cfg3;
    set = 1;
    switch (clock)
    {
        case (MCP_16MHz):
            switch (canSpeed) 
            {
                case (CAN_5KBPS):
                    cfg1 = MCP_16MHz_5kBPS_CFG1;
                    cfg2 = MCP_16MHz_5kBPS_CFG2;
                    cfg3 = MCP_16MHz_5kBPS_CFG3;
                    break;

                case (CAN_10KBPS):
                    cfg1 = MCP_16MHz_10kBPS_CFG1;
                    cfg2 = MCP_16MHz_10kBPS_CFG2;
                    cfg3 = MCP_16MHz_10kBPS_CFG3;
                    break;

                case (CAN_20KBPS):
                    cfg1 = MCP_16MHz_20kBPS_CFG1;
                    cfg2 = MCP_16MHz_20kBPS_CFG2;
                    cfg3 = MCP_16MHz_20kBPS_CFG3;
                    break;
        
                case (CAN_31K25BPS):
                    cfg1 = MCP_16MHz_31k25BPS_CFG1;
                    cfg2 = MCP_16MHz_31k25BPS_CFG2;
                    cfg3 = MCP_16MHz_31k25BPS_CFG3;
                    break;

                case (CAN_40KBPS):
                    cfg1 = MCP_16MHz_40kBPS_CFG1;
                    cfg2 = MCP_16MHz_40kBPS_CFG2;
                    cfg3 = MCP_16MHz_40kBPS_CFG3;
                    break;

                case (CAN_50KBPS):
                    cfg1 = MCP_16MHz_50kBPS_CFG1;
                    cfg2 = MCP_16MHz_50kBPS_CFG2;
                    cfg3 = MCP_16MHz_50kBPS_CFG3;
                    break;

                case (CAN_80KBPS):
                    cfg1 = MCP_16MHz_80kBPS_CFG1;
                    cfg2 = MCP_16MHz_80kBPS_CFG2;
                    cfg3 = MCP_16MHz_80kBPS_CFG3;
                    break;

                case (CAN_100KBPS):                                             /* 100KBPS                  */
                    cfg1 = MCP_16MHz_100kBPS_CFG1;
                    cfg2 = MCP_16MHz_100kBPS_CFG2;
                    cfg3 = MCP_16MHz_100kBPS_CFG3;
                    break;

                case (CAN_125KBPS):
                    cfg1 = MCP_16MHz_125kBPS_CFG1;
                    cfg2 = MCP_16MHz_125kBPS_CFG2;
                    cfg3 = MCP_16MHz_125kBPS_CFG3;
                    break;

                case (CAN_200KBPS):
                    cfg1 = MCP_16MHz_200kBPS_CFG1;
                    cfg2 = MCP_16MHz_200kBPS_CFG2;
                    cfg3 = MCP_16MHz_200kBPS_CFG3;
                    break;

                case (CAN_250KBPS):
                    cfg1 = MCP_16MHz_250kBPS_CFG1;
                    cfg2 = MCP_16MHz_250kBPS_CFG2;
                    cfg3 = MCP_16MHz_250kBPS_CFG3;
                    break;

                case (CAN_500KBPS):
                    cfg1 = MCP_16MHz_500kBPS_CFG1;
                    cfg2 = MCP_16MHz_500kBPS_CFG2;
                    cfg3 = MCP_16MHz_500kBPS_CFG3;
                    break;
        
                case (CAN_1000KBPS):
                    cfg1 = MCP_16MHz_1000kBPS_CFG1;
                    cfg2 = MCP_16MHz_1000kBPS_CFG2;
                    cfg3 = MCP_16MHz_1000kBPS_CFG3;
                    break;  

                default:
                    set = 0;
                    break;
            }
            break;

        case (MCP_8MHz):
            switch (canSpeed) 
            {
                case (CAN_5KBPS):
                    cfg1 = MCP_8MHz_5kBPS_CFG1;
                    cfg2 = MCP_8MHz_5kBPS_CFG2;
                    cfg3 = MCP_8MHz_5kBPS_CFG3;
                    break;

                case (CAN_10KBPS):
                    cfg1 = MCP_8MHz_10kBPS_CFG1;
                    cfg2 = MCP_8MHz_10kBPS_CFG2;
                    cfg3 = MCP_8MHz_10kBPS_CFG3;
                    break;

                case (CAN_20KBPS):
                    cfg1 = MCP_8MHz_20kBPS_CFG1;
                    cfg2 = MCP_8MHz_20kBPS_CFG2;
                    cfg3 = MCP_8MHz_20kBPS_CFG3;
                    break;
        
                case (CAN_31K25BPS):
                    cfg1 = MCP_8MHz_31k25BPS_CFG1;
                    cfg2 = MCP_8MHz_31k25BPS_CFG2;
                    cfg3 = MCP_8MHz_31k25BPS_CFG3;
                    break;

                case (CAN_40KBPS):
                    cfg1 = MCP_8MHz_40kBPS_CFG1;
                    cfg2 = MCP_8MHz_40kBPS_CFG2;
                    cfg3 = MCP_8MHz_40kBPS_CFG3;
                    break;

                case (CAN_50KBPS):
                    cfg1 = MCP_8MHz_50kBPS_CFG1;
                    cfg2 = MCP_8MHz_50kBPS_CFG2;
                    cfg3 = MCP_8MHz_50kBPS_CFG3;
                    break;

                case (CAN_80KBPS):
                    cfg1 = MCP_8MHz_80kBPS_CFG1;
                    cfg2 = MCP_8MHz_80kBPS_CFG2;
                    cfg3 = MCP_8MHz_80kBPS_CFG3;
                    break;

                case (CAN_100KBPS):                                             /* 100KBPS                  */
                    cfg1 = MCP_8MHz_100kBPS_CFG1;
                    cfg2 = MCP_8MHz_100kBPS_CFG2;
                    cfg3 = MCP_8MHz_100kBPS_CFG3;
                    break;

                case (CAN_125KBPS):
                    cfg1 = MCP_8MHz_125kBPS_CFG1;
                    cfg2 = MCP_8MHz_125kBPS_CFG2;
                    cfg3 = MCP_8MHz_125kBPS_CFG3;
                    break;

                case (CAN_200KBPS):
                    cfg1 = MCP_8MHz_200kBPS_CFG1;
                    cfg2 = MCP_8MHz_200kBPS_CFG2;
                    cfg3 = MCP_8MHz_200kBPS_CFG3;
                    break;

                case (CAN_250KBPS):
                    cfg1 = MCP_8MHz_250kBPS_CFG1;
                    cfg2 = MCP_8MHz_250kBPS_CFG2;
                    cfg3 = MCP_8MHz_250kBPS_CFG3;
                    break;

                case (CAN_500KBPS):
                    cfg1 = MCP_8MHz_500kBPS_CFG1;
                    cfg2 = MCP_8MHz_500kBPS_CFG2;
                    cfg3 = MCP_8MHz_500kBPS_CFG3;
                    break;
        
                case (CAN_1000KBPS):
                    cfg1 = MCP_8MHz_1000kBPS_CFG1;
                    cfg2 = MCP_8MHz_1000kBPS_CFG2;
                    cfg3 = MCP_8MHz_1000kBPS_CFG3;
                    break;  

                default:
                    set = 0;
                    break;
            }
            break;

        default:
            set = 0;
            break;
    }

    if (set)
    {
        mcp2515_setRegister(MCP_CNF1, cfg1);
        mcp2515_setRegister(MCP_CNF2, cfg2);
        mcp2515_setRegister(MCP_CNF3, cfg3);
        return MCP2515_OK;
    }
    else
    {
        return MCP2515_FAIL;
    }
}

void  mcp2515_initCANBuffers(void)
{
    uint8_t i, a1, a2, a3;

    a1 = MCP_TXB0CTRL;
    a2 = MCP_TXB1CTRL;
    a3 = MCP_TXB2CTRL;
    for (i = 0; i < 14; i++)                                              /* in-buffer loop               */
    {                                         
        mcp2515_setRegister(a1, 0);
        mcp2515_setRegister(a2, 0);
        mcp2515_setRegister(a3, 0);
        a1++;
        a2++;
        a3++;
    }
    mcp2515_setRegister(MCP_RXB0CTRL, 0);
    mcp2515_setRegister(MCP_RXB1CTRL, 0);
}

uint8_t  mcp2515_init(const uint8_t canSpeed, const uint8_t clock)                       /* mcp2515init                  */
{
    #if MCP2515_DEBUG_MODE
        NRF_LOG_DEBUG("Entering mcp2515_ini"); 
    #endif

    uint8_t res;

    mcp2515_reset();
    nrf_delay_ms(10);   

    #if MCP2515_DEBUG_MODE
        NRF_LOG_DEBUG("Entering CANCTRL Mode"); 
    #endif

    res = mcp2515_setCANCTRL_Mode(MODE_CONFIG);
    if(res > 0)
    {
        #if MCP2515_DEBUG_MODE
            NRF_LOG_DEBUG("Enter Setting Mode Fail!!!"); 
        #else
            nrf_delay_ms(10);
        #endif

        return res;
    }

    #if MCP2515_DEBUG_MODE
        NRF_LOG_DEBUG("Enter Setting Mode Success"); 
    #else
        nrf_delay_ms(10);
    #endif

    /* set baudrate                 */
    if(mcp2515_configRate(canSpeed, clock))
    {
        #if MCP2515_DEBUG_MODE
            NRF_LOG_DEBUG("Set Rate Fail!!!"); 
        #else
            nrf_delay_ms(10);
        #endif

        return res;
    }

    #if MCP2515_DEBUG_MODE
        NRF_LOG_DEBUG("Set Rate Success"); 
    #else
        nrf_delay_ms(10);
    #endif

    if ( res == MCP2515_OK )
    {
        mcp2515_initCANBuffers();                                       /* init canbuffers              */
        
        mcp2515_setRegister(MCP_CANINTE, MCP_RX0IF | MCP_RX1IF);        /* interrupt mode               */

        #if (DEBUG_RXANY == 1)
            mcp2515_modifyRegister(MCP_RXB0CTRL, MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK, MCP_RXB_RX_ANY | MCP_RXB_BUKT_MASK);      /* enable both receive-buffers to receive any message and enable rollover */
            mcp2515_modifyRegister(MCP_RXB1CTRL, MCP_RXB_RX_MASK, MCP_RXB_RX_ANY);
        #else
            mcp2515_modifyRegister(MCP_RXB0CTRL, MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK, MCP_RXB_RX_STDEXT | MCP_RXB_BUKT_MASK );  /* enable both receive-buffers to receive messages with std. and ext. identifiers and enable rollover */
            mcp2515_modifyRegister(MCP_RXB1CTRL, MCP_RXB_RX_MASK, MCP_RXB_RX_STDEXT);
        #endif
                                                                        
        res = mcp2515_setCANCTRL_Mode(MODE_NORMAL);                     /* enter normal mode            */
        if(res)
        {
            #if MCP2515_DEBUG_MODE
                NRF_LOG_DEBUG("Enter Normal Mode Fail!!!"); 
            #else
                nrf_delay_ms(10);
            #endif
          
            return res;
        }

        #if MCP2515_DEBUG_MODE
            NRF_LOG_DEBUG("Enter Normal Mode Success"); 
        #else
            nrf_delay_ms(10);
        #endif
    }
    return res;
}

void  mcp2515_write_id( const uint8_t mcp_addr, const uint8_t ext, const uint32_t id )
{
    uint16_t canid;
    uint8_t tbufdata[4];

    canid = (uint16_t)(id & 0x0FFFF);

    if ( ext == 1) 
    {
        tbufdata[MCP_EID0] = (uint8_t) (canid & 0xFF);
        tbufdata[MCP_EID8] = (uint8_t) (canid >> 8);
        canid = (uint16_t)(id >> 16);
        tbufdata[MCP_SIDL] = (uint8_t) (canid & 0x03);
        tbufdata[MCP_SIDL] += (uint8_t) ((canid & 0x1C) << 3);
        tbufdata[MCP_SIDL] |= MCP_TXB_EXIDE_M;
        tbufdata[MCP_SIDH] = (uint8_t) (canid >> 5 );
    }
    else 
    {
        tbufdata[MCP_SIDH] = (uint8_t) (canid >> 3 );
        tbufdata[MCP_SIDL] = (uint8_t) ((canid & 0x07 ) << 5);
        tbufdata[MCP_EID0] = 0;
        tbufdata[MCP_EID8] = 0;
    }

    mcp2515_setRegisterS( mcp_addr, tbufdata, 4 );
}

void  mcp2515_read_id( const uint8_t mcp_addr, uint8_t* ext, uint32_t* id )
{
    uint8_t tbufdata[4];

    *ext = 0;
    *id = 0;

    mcp2515_readRegisterS( mcp_addr, tbufdata, 4 );

    *id = (tbufdata[MCP_SIDH]<<3) + (tbufdata[MCP_SIDL]>>5);

    if ( (tbufdata[MCP_SIDL] & MCP_TXB_EXIDE_M) ==  MCP_TXB_EXIDE_M )       /* extended id                  */
    {
        *id = (*id<<2) + (tbufdata[MCP_SIDL] & 0x03);
        *id = (*id<<8) + tbufdata[MCP_EID8];
        *id = (*id<<8) + tbufdata[MCP_EID0];
        *ext = 1;
    }
}

void  mcp2515_write_canMsg( const uint8_t buffer_sidh_addr )
{
    uint8_t mcp_addr;
    mcp_addr = buffer_sidh_addr;

    mcp2515_setRegisterS((mcp_addr+5), m_mcp_can.m_data, m_mcp_can.m_len );     /* write data bytes             */
    mcp2515_setRegister((mcp_addr+4), m_mcp_can.m_len );                        /* write the RTR and DLC        */
    mcp2515_write_id(mcp_addr, m_mcp_can.m_ext_flag, m_mcp_can.m_id );          /* write CAN id                 */
}

void  mcp2515_read_canMsg( const uint8_t buffer_sidh_addr )                     /* read can msg                 */
{
    uint8_t mcp_addr, ctrl;

    mcp_addr = buffer_sidh_addr;

    mcp2515_read_id( mcp_addr, &m_mcp_can.m_ext_flag, &m_mcp_can.m_id );

    ctrl = mcp2515_readRegister( mcp_addr-1 );
    m_mcp_can.m_len = mcp2515_readRegister( mcp_addr+4 );

    m_mcp_can.m_len &= MCP_DLC_MASK;
    mcp2515_readRegisterS( mcp_addr+5, &(m_mcp_can.m_data[0]), m_mcp_can.m_len );
}

void  mcp2515_start_transmit( const uint8_t mcp_addr )                          /* start transmit               */
{
    mcp2515_modifyRegister( mcp_addr-1 , MCP_TXB_TXREQ_M, MCP_TXB_TXREQ_M );
}

uint8_t  mcp2515_getNextFreeTXBuf( uint8_t *txbuf_n )                           /* get Next free txbuf          */
{
    uint8_t res, i, ctrlval;
    uint8_t ctrlregs[MCP_N_TXBUFFERS] = { MCP_TXB0CTRL, MCP_TXB1CTRL, MCP_TXB2CTRL };

    res = MCP_ALLTXBUSY;
    *txbuf_n = 0x00;

    for (i=0; i<MCP_N_TXBUFFERS; i++)                                           /* check all 3 TX-Buffers       */
    {
        ctrlval = mcp2515_readRegister( ctrlregs[i] );
        if ( (ctrlval & MCP_TXB_TXREQ_M) == 0 )
        {
            *txbuf_n = ctrlregs[i]+1;                                           /* return SIDH-address of Buffer */
            res = MCP2515_OK;

            return res;                                                         /* ! function exit              */
        }
    }

    return res;
}

uint8_t  setMsg( uint32_t id, uint8_t ext, uint8_t len, uint8_t *pData )
{
    int i = 0;
    m_mcp_can.m_ext_flag = ext;
    m_mcp_can.m_id = id;
    m_mcp_can.m_len = len;

    for(i = 0; i<MAX_CHAR_IN_MESSAGE; i++)
    {
        m_mcp_can.m_data[i] = *(pData+i);
    }

    return MCP2515_OK;
}

uint8_t  clearMsg()
{
    for(int i = 0; i<m_mcp_can.m_len; i++)
    {
        m_mcp_can.m_data[i] = 0x00;
    }

    m_mcp_can.m_id = 0;
    m_mcp_can.m_len = 0;
    m_mcp_can.m_ext_flag = 0;
    
    return MCP2515_OK;
}

uint8_t  sendMsg()
{
    uint8_t res, res1, txbuf_n;
    uint16_t uiTimeOut = 0;

    do
    {
        res = mcp2515_getNextFreeTXBuf(&txbuf_n);                       /* info = addr. */
        uiTimeOut++;
    } while (res == MCP_ALLTXBUSY && (uiTimeOut < TIMEOUTVALUE));

    if(uiTimeOut == TIMEOUTVALUE) 
    {   
        return CAN_GETTXBFTIMEOUT;                                      /* get tx buff time out */
    }

    uiTimeOut = 0;
    mcp2515_write_canMsg( txbuf_n);
    mcp2515_start_transmit( txbuf_n );

    do
    {
        uiTimeOut++;        
        res1= mcp2515_readRegister(txbuf_n-1);                           /* the ctrl reg is located at txbuf_n-1 read send buff ctrl reg */
        res1 = res1 & 0x08;                               		
    } while(res1 && (uiTimeOut < TIMEOUTVALUE));   

    if(uiTimeOut == TIMEOUTVALUE)                                        /* send msg timeout */	
    {
        return CAN_SENDMSGTIMEOUT;
    }

    return CAN_OK;
}

uint8_t  readMsg()
{
    uint8_t stat, res;

    stat = mcp2515_readStatus();

    if ( stat & MCP_STAT_RX0IF )                                        /* Msg in Buffer 0 */
    {
        mcp2515_read_canMsg( MCP_RXBUF_0);
        mcp2515_modifyRegister(MCP_CANINTF, MCP_RX0IF, 0);
        res = CAN_OK;
    }
    else if ( stat & MCP_STAT_RX1IF )                                   /* Msg in Buffer 1 */
    {
        mcp2515_read_canMsg( MCP_RXBUF_1);
        mcp2515_modifyRegister(MCP_CANINTF, MCP_RX1IF, 0);
        res = CAN_OK;
    }
    else 
    {
        res = CAN_NOMSG;
    }

    return res;
}

void  mcp_can_setcs( uint8_t cs )
{
    m_mcp_can.m_cs = cs;
    nrf_gpio_cfg_output(m_mcp_can.m_cs);
    mcp2515_unselect();
}

uint8_t  mcp_can_begin( uint8_t speedset, const uint8_t clockset )
{
    uint8_t res;

    res = mcp2515_init(speedset, clockset);

    #if MCP2515_DEBUG_MODE
        NRF_LOG_DEBUG("MCP2515 Speed init done"); 
    #endif

    if (res == MCP2515_OK)
    {
        return CAN_OK;
    }
    else
    { 
        return CAN_FAILINIT;
    }
}

uint8_t  mcp_can_send_msg( uint32_t id, uint8_t ext, uint8_t len, uint8_t *buf )
{
    setMsg(id, ext, len, buf);

    return sendMsg();
}

uint8_t  mcp_can_read_msg( uint32_t *ID, uint8_t *len, uint8_t buf[] )
{
    uint8_t rc;

    rc = readMsg();

    if (rc == CAN_OK)
    {
        *len = m_mcp_can.m_len;
        *ID = m_mcp_can.m_id;
       
        for(int i = 0; i<m_mcp_can.m_len && i < MAX_CHAR_IN_MESSAGE; i++)
        {
            buf[i] = m_mcp_can.m_data[i];
        }
    }
    else
    {
       *len = 0;
    }

    return rc;
}

uint8_t  mcp_can_check_receive(void)
{
    uint8_t res;
    res = mcp2515_readStatus();                                         /* RXnIF in Bit 1 and 0 */

    if ( res & MCP_STAT_RXIF_MASK ) 
    {
        return CAN_MSGAVAIL;
    }
    else 
    {
        return CAN_NOMSG;
    }
}

uint8_t  mcp_can_check_error(void)
{
    uint8_t eflg = mcp2515_readRegister(MCP_EFLG);

    if ( eflg & MCP_EFLG_ERRORMASK ) 
    {
        return CAN_CTRLERROR;
    }
    else 
    {
        return CAN_OK;
    }
}
