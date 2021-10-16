#ifndef _MCP2515_H_
#define _MCP2515_H_

#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "nrf_drv_spi.h"
#include "bsp.h"
#include "app_timer.h"
#include "nordic_common.h"
#include "mcp_can_dfs.h"

#define MAX_CHAR_IN_MESSAGE 8

typedef struct
{
	uint8_t m_ext_flag;					//flag for 11 bit or 29 bit
	uint32_t m_id;						//can id
	uint8_t m_len;						//data length counter
	uint8_t m_data[MAX_CHAR_IN_MESSAGE];  //data
	uint8_t m_cs;							//chip select
}mcp_can_t;
	
void mcp_can_setcs(uint8_t cs);
void mcp_spi_init(void);
uint8_t mcp_can_begin(uint8_t speedset, const uint8_t clockset);
uint8_t mcp_can_send_msg(uint32_t id, uint8_t ext, uint8_t len, uint8_t *buf);
uint8_t mcp_can_read_msg(uint32_t *id, uint8_t *len, uint8_t buf[]);
uint8_t mcp_can_check_receive(void);
uint8_t mcp_can_check_error(void);

#endif
