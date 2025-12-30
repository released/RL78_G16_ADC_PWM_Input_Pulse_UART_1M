#ifndef __DRV_PWM_INPUT_H__
#define __DRV_PWM_INPUT_H__

/*_____ I N C L U D E S ____________________________________________________*/

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

typedef struct
{
    volatile unsigned long high_cnt;
    volatile unsigned long low_cnt;
    volatile unsigned long total_cnt;

    volatile unsigned short duty_x10;     	/* 0.1% unit, 0..1000 */
    volatile unsigned long freq_hz;       	/* optional, need timer clock */

    volatile unsigned long fclk_hz;        /* system fCLK in Hz (e.g. 16000000) */
    volatile unsigned long cap_clk_hz;     /* capture timer clock in Hz (after prescaler) */

    volatile unsigned char high_done;
    volatile unsigned char low_done;
    volatile unsigned char calc_ready;
} pwm_capture_state_t;

/*_____ M A C R O S ________________________________________________________*/

// #define ENABLE_LOG_CAPTURE

/*_____ F U N C T I O N S __________________________________________________*/

/* Set system fCLK (default can be 16000000). */
void drv_pwm_input_set_fclk_hz(unsigned long fclk_hz);

/* Auto detect capture clock from TMR04/TPS0. Call after TAU init, and if TPS0 changes. */
void drv_pwm_input_update_capture_clk_from_hw(void);

unsigned long drv_pwm_input_get_capture_clk_hz(void);

void drv_pwm_input_clear(void);
void drv_pwm_input_init(void);
void drv_pwm_input_log(void);
void drv_pwm_input_irq_handler(void);     /* call in TAU capture ISR */
void drv_pwm_input_poll(void);            /* call in main loop */

#endif //__DRV_PWM_INPUT_H__
