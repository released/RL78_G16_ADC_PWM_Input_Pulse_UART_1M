#ifndef __DRV_UART_H__
#define __DRV_UART_H__

/*_____ I N C L U D E S ____________________________________________________*/
#include "r_cg_macrodriver.h"
/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/

/*  
	template
	typedef struct _peripheral_manager_t
	{
		uint16_t* pu16Far;
		uint8_t u8Cmd;
		uint8_t au8Buf[33];
		uint8_t u8RecCnt;
		uint8_t bByPass;
	}PERIPHERAL_MANAGER_T;

	volatile PERIPHERAL_MANAGER_T g_PeripheralManager = 
	{
		.pu16Far = NULL,	//.pu16Far = 0	
		.u8Cmd = 0,
		.au8Buf = {0},		//.au8Buf = {100U, 200U},
		.u8RecCnt = 0,
		.bByPass = FALSE,
	};
	extern volatile PERIPHERAL_MANAGER_T g_PeripheralManager;
	
	/////////////////////////////////////////////////////////////
	
*/
/* === Flags === */
#define DRV_UART_FLAG_TX_BUSY     (1U << 0)
#define DRV_UART_FLAG_TX_DONE     (1U << 1)
#define DRV_UART_FLAG_RX_OVF      (1U << 2)
#define DRV_UART_FLAG_RX_ACTIVITY (1U << 3)

/* === Function pointer types === */
typedef MD_STATUS (*drv_uart_send_fn)(uint8_t * const tx_buf, uint16_t tx_num);
typedef MD_STATUS (*drv_uart_recv_fn)(uint8_t * const rx_buf, uint16_t rx_num);

/* === Ring buffer === */
typedef struct _drv_uart_ring_t
{
    uint8_t  *buf;
    uint16_t size;
    uint16_t head;
    uint16_t tail;
    uint16_t count;
} drv_uart_ring_t;

/* === Channel object === */
typedef struct _drv_uart_ch_t
{
    /* Binding to specific UART port driver */
    drv_uart_send_fn send_fn;
    drv_uart_recv_fn recv_fn;

    /* TX */
    uint8_t   *tx_buf;
    uint16_t  tx_buf_size;
    volatile uint16_t tx_len;
    volatile uint8_t  tx_busy;

    /* RX (ring) */
    drv_uart_ring_t rx_ring;

    /* 1-byte receive temp buffer for re-arm strategy */
    volatile uint8_t rx_tmp;

    /* State flags / error */
    volatile uint8_t flags;
    volatile uint8_t last_err;
} drv_uart_ch_t;


#define UART_RX_PKT_LEN                                 (256U)
#define UART_TX_PKT_LEN                                 (128U)
#define UART_TEST_PKT_LEN                               (16U)

/*_____ M A C R O S ________________________________________________________*/

// #define ENABLE_LOG_UART

/*_____ F U N C T I O N S __________________________________________________*/
extern drv_uart_ch_t g_uart_ch2;

/* === Init / bind === */
void drv_uart_init(drv_uart_ch_t *ch,
                   drv_uart_send_fn send_fn,
                   drv_uart_recv_fn recv_fn,
                   uint8_t *tx_buf, uint16_t tx_buf_size,
                   uint8_t *rx_ring_buf, uint16_t rx_ring_size);

/* === TX API === */
MD_STATUS drv_uart_send(drv_uart_ch_t *ch, const uint8_t *data, uint16_t len);
uint8_t   drv_uart_is_tx_busy(drv_uart_ch_t *ch);
uint8_t   drv_uart_fetch_tx_done(drv_uart_ch_t *ch);

/* === RX API === */
MD_STATUS drv_uart_rx_arm_1byte(drv_uart_ch_t *ch);      /* call once at init and again in rx callback */
uint16_t  drv_uart_rx_available(drv_uart_ch_t *ch);
uint16_t  drv_uart_read(drv_uart_ch_t *ch, uint8_t *out, uint16_t max_len);
uint8_t   drv_uart_fetch_rx_overflow(drv_uart_ch_t *ch);

/* === Hook points: call from UARTx callbacks/ISR === */
void drv_uart_on_tx_end_isr(drv_uart_ch_t *ch);
void drv_uart_on_rx_byte_isr(drv_uart_ch_t *ch, uint8_t b);


// application
void r_uart_callback_receiveend(void);
void r_uart_callback_sendend(void);
void app_uart_init(void);

void app_uart_rx_poll_and_dump(void);
void app_uart_send_test_packet(void);

#endif //__DRV_UART_H__
