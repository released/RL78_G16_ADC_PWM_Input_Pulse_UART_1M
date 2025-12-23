#ifndef __DRV_PWM_SWEEP_H__
#define __DRV_PWM_SWEEP_H__

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
    unsigned char duty;      /* 0..100 */
    signed char  step;       /* +10 or -10 */
    unsigned short acc_ms;   /* accumulate ms */
    unsigned char tick_1s;   /* event flag */
} pwm_sweep_state_t;

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

void pwm_sweep_process(void);
void pwm_sweep_1ms_isr(void);
void pwm_sweep_init(void);

#endif //__DRV_PWM_SWEEP_H__
