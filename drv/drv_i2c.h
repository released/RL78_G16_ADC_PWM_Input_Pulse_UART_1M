#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__

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

typedef enum _drv_i2c_err_e
{
    DRV_I2C_ERR_NONE = 0,
    DRV_I2C_ERR_BUSY,
    DRV_I2C_ERR_NACK,
    DRV_I2C_ERR_ARBLOST,
    DRV_I2C_ERR_HW,
    DRV_I2C_ERR_TIMEOUT
} drv_i2c_err_e;

typedef enum _drv_i2c_op_e
{
    DRV_I2C_OP_NONE = 0,
    DRV_I2C_OP_WRITE,
    DRV_I2C_OP_READ,
    DRV_I2C_OP_WRITE_READ
} drv_i2c_op_e;

/* Bind to Config_IICAx */
typedef MD_STATUS (*drv_i2c_send_fn)(uint8_t adr, uint8_t * const tx, uint16_t len);
typedef MD_STATUS (*drv_i2c_recv_fn)(uint8_t adr, uint8_t * const rx, uint16_t len);

typedef struct _drv_i2c_t
{
    drv_i2c_send_fn send_fn;
    drv_i2c_recv_fn recv_fn;

    volatile uint8_t      busy;
    volatile uint8_t      done;
    volatile drv_i2c_err_e err;

    drv_i2c_op_e          op;
    uint8_t               addr;

    uint8_t              *wbuf;
    uint16_t              wlen;
    uint8_t              *rbuf;
    uint16_t              rlen;

    volatile uint8_t      stage;     /* 0 idle, 1 write, 2 read */

    uint32_t              deadline;
} drv_i2c_t;

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

/* API */
void drv_i2c_init(drv_i2c_t *i2c,
                  drv_i2c_send_fn send_fn,
                  drv_i2c_recv_fn recv_fn);

MD_STATUS drv_i2c_write(drv_i2c_t *i2c,
                        uint8_t addr,
                        uint8_t *buf,
                        uint16_t len,
                        uint32_t now,
                        uint32_t timeout);

MD_STATUS drv_i2c_read(drv_i2c_t *i2c,
                       uint8_t addr,
                       uint8_t *buf,
                       uint16_t len,
                       uint32_t now,
                       uint32_t timeout);

MD_STATUS drv_i2c_write_read(drv_i2c_t *i2c,
                             uint8_t addr,
                             uint8_t *wbuf,
                             uint16_t wlen,
                             uint8_t *rbuf,
                             uint16_t rlen,
                             uint32_t now,
                             uint32_t timeout);

void drv_i2c_poll(drv_i2c_t *i2c, uint32_t now);

uint8_t      drv_i2c_fetch_done(drv_i2c_t *i2c);
drv_i2c_err_e drv_i2c_get_error(drv_i2c_t *i2c);

/* Hook from callbacks */
void drv_i2c_on_send_end_isr(drv_i2c_t *i2c);
void drv_i2c_on_recv_end_isr(drv_i2c_t *i2c);
void drv_i2c_on_error_isr(drv_i2c_t *i2c, MD_STATUS flag);


#endif //__DRV_I2C_H__
