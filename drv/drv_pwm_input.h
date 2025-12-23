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
    volatile unsigned long clk_hz;       	/* optional, capture counter clock in Hz */

    volatile unsigned char high_done;
    volatile unsigned char low_done;
    volatile unsigned char calc_ready;
} pwm_capture_state_t;

/*_____ M A C R O S ________________________________________________________*/

#define ENABLE_LOG_CAPTURE

/*_____ F U N C T I O N S __________________________________________________*/

void drv_pwm_input_clear(void);
void drv_pwm_input_set_clock_hz(unsigned long clk_hz);
void drv_pwm_input_init(void);
void drv_pwm_input_log(void);
void drv_pwm_input_irq_handler(void);     /* call in TAU capture ISR */
void drv_pwm_input_poll(void);            /* call in main loop */

#endif //__DRV_PWM_INPUT_H__
