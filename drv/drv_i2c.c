/*_____ I N C L U D E S ____________________________________________________*/
// #include <stdio.h>
#include "r_smc_entry.h"
#include "r_cg_macrodriver.h"

#include "drv_i2c.h"
/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/
drv_i2c_t g_i2c0;

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

void drv_i2c_init(drv_i2c_t *i2c,
                  drv_i2c_send_fn send_fn,
                  drv_i2c_recv_fn recv_fn)
{
    if (i2c == 0)
    {
        return;
    }

    i2c->send_fn = send_fn;
    i2c->recv_fn = recv_fn;

    i2c->busy = 0U;
    i2c->done = 0U;
    i2c->err  = DRV_I2C_ERR_NONE;

    i2c->op = DRV_I2C_OP_NONE;
    i2c->addr = 0U;

    i2c->wbuf = 0;
    i2c->wlen = 0U;
    i2c->rbuf = 0;
    i2c->rlen = 0U;

    i2c->stage = 0U;
    i2c->deadline = 0UL;
}

static void drv_i2c_start(drv_i2c_t *i2c,
                          drv_i2c_op_e op,
                          uint8_t addr,
                          uint32_t now,
                          uint32_t timeout)
{
    i2c->busy = 1U;
    i2c->done = 0U;
    i2c->err  = DRV_I2C_ERR_NONE;

    i2c->op   = op;
    i2c->addr = addr;
    i2c->deadline = now + timeout;
}

MD_STATUS drv_i2c_write(drv_i2c_t *i2c,
                        uint8_t addr,
                        uint8_t *buf,
                        uint16_t len,
                        uint32_t now,
                        uint32_t timeout)
{
    MD_STATUS ret;

    if ((i2c == 0) || (buf == 0) || (len == 0U))
    {
        return MD_ERROR;
    }
    if (i2c->busy != 0U)
    {
        return MD_BUSY1;
    }

    drv_i2c_start(i2c, DRV_I2C_OP_WRITE, addr, now, timeout);

    i2c->wbuf = buf;
    i2c->wlen = len;

    ret = i2c->send_fn(addr, buf, len);
    if (ret != MD_OK)
    {
        i2c->busy = 0U;
        i2c->err = DRV_I2C_ERR_BUSY;
    }
    return ret;
}

MD_STATUS drv_i2c_read(drv_i2c_t *i2c,
                       uint8_t addr,
                       uint8_t *buf,
                       uint16_t len,
                       uint32_t now,
                       uint32_t timeout)
{
    MD_STATUS ret;

    if ((i2c == 0) || (buf == 0) || (len == 0U))
    {
        return MD_ERROR;
    }
    if (i2c->busy != 0U)
    {
        return MD_BUSY1;
    }

    drv_i2c_start(i2c, DRV_I2C_OP_READ, addr, now, timeout);

    i2c->rbuf = buf;
    i2c->rlen = len;

    ret = i2c->recv_fn(addr, buf, len);
    if (ret != MD_OK)
    {
        i2c->busy = 0U;
        i2c->err = DRV_I2C_ERR_BUSY;
    }
    return ret;
}

MD_STATUS drv_i2c_write_read(drv_i2c_t *i2c,
                             uint8_t addr,
                             uint8_t *wbuf,
                             uint16_t wlen,
                             uint8_t *rbuf,
                             uint16_t rlen,
                             uint32_t now,
                             uint32_t timeout)
{
    MD_STATUS ret;

    if ((i2c == 0) || (wbuf == 0) || (rbuf == 0))
    {
        return MD_ERROR;
    }
    if (i2c->busy != 0U)
    {
        return MD_BUSY1;
    }

    drv_i2c_start(i2c, DRV_I2C_OP_WRITE_READ, addr, now, timeout);

    i2c->wbuf = wbuf;
    i2c->wlen = wlen;
    i2c->rbuf = rbuf;
    i2c->rlen = rlen;
    i2c->stage = 1U;

    ret = i2c->send_fn(addr, wbuf, wlen);
    if (ret != MD_OK)
    {
        i2c->busy = 0U;
        i2c->err = DRV_I2C_ERR_BUSY;
    }
    return ret;
}

void drv_i2c_poll(drv_i2c_t *i2c, uint32_t now)
{
    MD_STATUS ret;

    if ((i2c == 0) || (i2c->busy == 0U))
    {
        return;
    }

    if ((uint32_t)(now - i2c->deadline) < 0x80000000UL)
    {
        i2c->busy = 0U;
        i2c->err = DRV_I2C_ERR_TIMEOUT;
        return;
    }

    if ((i2c->op == DRV_I2C_OP_WRITE_READ) && (i2c->stage == 2U))
    {
        i2c->stage = 3U;
        ret = i2c->recv_fn(i2c->addr, i2c->rbuf, i2c->rlen);
        if (ret != MD_OK)
        {
            i2c->busy = 0U;
            i2c->err = DRV_I2C_ERR_BUSY;
        }
    }
}

uint8_t drv_i2c_fetch_done(drv_i2c_t *i2c)
{
    uint8_t d;

    if (i2c == 0)
    {
        return 0U;
    }

    d = i2c->done;
    i2c->done = 0U;
    return d;
}

drv_i2c_err_e drv_i2c_get_error(drv_i2c_t *i2c)
{
    if (i2c == 0)
    {
        return DRV_I2C_ERR_HW;
    }
    return i2c->err;
}

/* ===== ISR hooks ===== */

void drv_i2c_on_send_end_isr(drv_i2c_t *i2c)
{
    if (i2c == 0)
    {
        return;
    }

    if (i2c->op == DRV_I2C_OP_WRITE_READ)
    {
        i2c->stage = 2U;
        return;
    }

    i2c->busy = 0U;
    i2c->done = 1U;
}

void drv_i2c_on_recv_end_isr(drv_i2c_t *i2c)
{
    if (i2c == 0)
    {
        return;
    }

    i2c->busy = 0U;
    i2c->done = 1U;
}

void drv_i2c_on_error_isr(drv_i2c_t *i2c, MD_STATUS flag)
{
    if (i2c == 0)
    {
        return;
    }

    i2c->busy = 0U;
    i2c->done = 0U;

    /* Basic mapping (extend if needed) */
    if (flag == MD_ERROR)
    {
        i2c->err = DRV_I2C_ERR_HW;
    }
    else
    {
        i2c->err = DRV_I2C_ERR_NACK;
    }
}

void r_iica0_callback_master_sendend(void)
{
    drv_i2c_on_send_end_isr(&g_i2c0);
}

void r_iica0_callback_master_receiveend(void)
{
    drv_i2c_on_recv_end_isr(&g_i2c0);
}

void r_iica0_callback_master_error(MD_STATUS flag)
{
    drv_i2c_on_error_isr(&g_i2c0, flag);
}

void app_i2c0_init(void)
{
    drv_i2c_init(&g_i2c0,
                 R_Config_IICA0_Master_Send,
                 R_Config_IICA0_Master_Receive);
}

