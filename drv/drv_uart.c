/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include "r_smc_entry.h"
#include "r_cg_macrodriver.h"

#include "drv_uart.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/
extern MD_STATUS R_Config_UART2_Send(uint8_t * const tx_buf, uint16_t tx_num);
extern MD_STATUS R_Config_UART2_Receive(uint8_t * const rx_buf, uint16_t rx_num);

uint8_t uart_tx_buf[UART_TX_PKT_LEN];
uint8_t uart_rx_ring[UART_RX_PKT_LEN];
drv_uart_ch_t g_uart_ch2;
/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

/* ===== Ring helpers ===== */

static void ring_init(drv_uart_ring_t *r, uint8_t *buf, uint16_t size)
{
    r->buf = buf;
    r->size = size;
    r->head = 0U;
    r->tail = 0U;
    r->count = 0U;
}

static void ring_push(drv_uart_ch_t *ch, uint8_t b)
{
    drv_uart_ring_t *r;

    r = &ch->rx_ring;

    if (r->count >= r->size)
    {
        ch->flags |= DRV_UART_FLAG_RX_OVF;
        return;
    }

    r->buf[r->head] = b;
    r->head++;
    if (r->head >= r->size)
    {
        r->head = 0U;
    }
    r->count++;
    ch->flags |= DRV_UART_FLAG_RX_ACTIVITY;
}

static uint8_t ring_pop(drv_uart_ring_t *r, uint8_t *b)
{
    if (r->count == 0U)
    {
        return 0U;
    }

    *b = r->buf[r->tail];
    r->tail++;
    if (r->tail >= r->size)
    {
        r->tail = 0U;
    }
    r->count--;
    return 1U;
}

/* ===== Public ===== */

void drv_uart_init(drv_uart_ch_t *ch,
                   drv_uart_send_fn send_fn,
                   drv_uart_recv_fn recv_fn,
                   uint8_t *tx_buf, uint16_t tx_buf_size,
                   uint8_t *rx_ring_buf, uint16_t rx_ring_size)
{
    if (ch == 0)
    {
        return;
    }

    ch->send_fn = send_fn;
    ch->recv_fn = recv_fn;

    ch->tx_buf = tx_buf;
    ch->tx_buf_size = tx_buf_size;
    ch->tx_len = 0U;
    ch->tx_busy = 0U;

    ch->rx_tmp = 0U;

    ch->flags = 0U;
    ch->last_err = 0U;

    ring_init(&ch->rx_ring, rx_ring_buf, rx_ring_size);
}

/* Non-blocking TX: copy to internal TX buffer then call bound send_fn */
MD_STATUS drv_uart_send(drv_uart_ch_t *ch, const uint8_t *data, uint16_t len)
{
    MD_STATUS ret;
    uint16_t i;

    if ((ch == 0) || (data == 0))
    {
        return MD_ERROR;
    }
    if ((len == 0U) || (ch->tx_buf == 0))
    {
        return MD_ERROR;
    }
    if (len > ch->tx_buf_size)
    {
        return MD_ERROR;
    }
    if (ch->send_fn == 0)
    {
        return MD_ERROR;
    }
    if (ch->tx_busy != 0U)
    {
        return MD_BUSY1;
    }

    for (i = 0U; i < len; i++)
    {
        ch->tx_buf[i] = data[i];
    }

    ch->tx_len = len;
    ch->tx_busy = 1U;
    ch->flags &= (uint8_t)~DRV_UART_FLAG_TX_DONE;
    ch->flags |= DRV_UART_FLAG_TX_BUSY;

    ret = ch->send_fn(ch->tx_buf, len);
    if (ret != MD_OK)
    {
        ch->tx_busy = 0U;
        ch->flags &= (uint8_t)~DRV_UART_FLAG_TX_BUSY;
        ch->last_err = (uint8_t)ret;
    }

    return ret;
}

uint8_t drv_uart_is_tx_busy(drv_uart_ch_t *ch)
{
    if (ch == 0)
    {
        return 0U;
    }
    return (uint8_t)(ch->tx_busy != 0U);
}

uint8_t drv_uart_fetch_tx_done(drv_uart_ch_t *ch)
{
    uint8_t v;

    if (ch == 0)
    {
        return 0U;
    }

    v = (uint8_t)((ch->flags & DRV_UART_FLAG_TX_DONE) != 0U);
    ch->flags &= (uint8_t)~DRV_UART_FLAG_TX_DONE;
    return v;
}

/* Arm 1 byte receive (call at init + re-arm in receive callback) */
MD_STATUS drv_uart_rx_arm_1byte(drv_uart_ch_t *ch)
{
    if ((ch == 0) || (ch->recv_fn == 0))
    {
        return MD_ERROR;
    }
    return ch->recv_fn((uint8_t * )&ch->rx_tmp, 1U);
}

uint16_t drv_uart_rx_available(drv_uart_ch_t *ch)
{
    if (ch == 0)
    {
        return 0U;
    }
    return ch->rx_ring.count;
}

uint16_t drv_uart_read(drv_uart_ch_t *ch, uint8_t *out, uint16_t max_len)
{
    uint16_t n;
    uint8_t b;

    if ((ch == 0) || (out == 0))
    {
        return 0U;
    }

    n = 0U;
    while (n < max_len)
    {
        if (ring_pop(&ch->rx_ring, &b) == 0U)
        {
            break;
        }
        out[n] = b;
        n++;
    }
    return n;
}

uint8_t drv_uart_fetch_rx_overflow(drv_uart_ch_t *ch)
{
    uint8_t v;

    if (ch == 0)
    {
        return 0U;
    }

    v = (uint8_t)((ch->flags & DRV_UART_FLAG_RX_OVF) != 0U);
    ch->flags &= (uint8_t)~DRV_UART_FLAG_RX_OVF;
    return v;
}

/* ===== Hook points ===== */

void drv_uart_on_tx_end_isr(drv_uart_ch_t *ch)
{
    if (ch == 0)
    {
        return;
    }

    ch->tx_busy = 0U;
    ch->flags &= (uint8_t)~DRV_UART_FLAG_TX_BUSY;
    ch->flags |= DRV_UART_FLAG_TX_DONE;
}

void drv_uart_on_rx_byte_isr(drv_uart_ch_t *ch, uint8_t b)
{
    if (ch == 0)
    {
        return;
    }
    ring_push(ch, b);
}


// put under driver : r_Config_UARTxx_callback_receiveend
void r_uart_callback_receiveend(void)
{
    /* 1) push received byte */
    drv_uart_on_rx_byte_isr(&g_uart_ch2, (uint8_t)g_uart_ch2.rx_tmp);

    /* 2) re-arm next byte */
    (void)drv_uart_rx_arm_1byte(&g_uart_ch2);
}

// put under driver : r_Config_UARTxx_callback_sendend
void r_uart_callback_sendend(void)
{
    drv_uart_on_tx_end_isr(&g_uart_ch2);
}

void app_uart_init(void)
{
    drv_uart_init(&g_uart_ch2,
                 R_Config_UART2_Send,
                 R_Config_UART2_Receive,
                 uart_tx_buf, (uint16_t)sizeof(uart_tx_buf),
                 uart_rx_ring, (uint16_t)sizeof(uart_rx_ring));

    (void)drv_uart_rx_arm_1byte(&g_uart_ch2);
}


void drv_uart_rx_log(const uint8_t *buf, uint16_t len)
{
#if defined(ENABLE_LOG_UART)
    uint16_t i; 

     if ((buf == 0) || (len == 0U))
    {
        return;
    }

    // printf_tiny("UART:len=%u-", (unsigned int)len);
    // printf_tiny("UART:");

    /* Hex bytes */
    for (i = 0U; i < len; i++)
    {
        printf_tiny("[%u]0x%02X", (unsigned int)i, (unsigned int)buf[i]);
        // printf_tiny("%02Xh", (unsigned int)buf[i]);

        if (i != (len - 1U))
        {
            printf_tiny(",");
        }
    }
    printf_tiny("\r\n");
#endif
}

void app_uart_rx_poll_and_dump(void)
{
    uint8_t buf[32];
    uint16_t n;

    if (drv_uart_fetch_rx_overflow(&g_uart_ch2) != 0U)
    {
        /* overflow happened: increase ring size or read more frequently */
        printf_tiny("rx_overflow\r\n");
    }

    n = drv_uart_read(&g_uart_ch2, buf, (uint16_t)sizeof(buf));
    if (n != 0U)
    {
        drv_uart_rx_log(buf, n);
    }
}


void app_uart_send_test_packet(void)
{
    static uint8_t counter = 0U;
    uint8_t pkt[UART_TEST_PKT_LEN];
    uint8_t i;
    MD_STATUS ret;

    pkt[0] = 0x55U;
    pkt[1] = 0xAAU;

    for (i = 2U; i < (UART_TEST_PKT_LEN - 1U); i++)
    {
        pkt[i] = (uint8_t)(i + 0x10U);
    }

    pkt[UART_TEST_PKT_LEN - 1U] = counter;

    ret = drv_uart_send(&g_uart_ch2, pkt, UART_TEST_PKT_LEN);
    if (ret == MD_OK)
    {
        counter++;
    }
    else
    {
        /* MD_BUSY: previous packet still sending, skip this round */
        /* MD_ERROR: config/binding issue */
    }
}