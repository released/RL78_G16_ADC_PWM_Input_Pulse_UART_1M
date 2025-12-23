#ifndef __DRV_PWM_BREATH_H__
#define __DRV_PWM_BREATH_H__

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
    unsigned short level_x10;   /* 0..1000 */
    unsigned short step_x10;    /* per update step */
    unsigned char  dir;         /* 0: up, 1: down */
    unsigned char  enable;
    unsigned short acc_ms;      /* accumulate time */
    unsigned short update_ms;   /* e.g. 10 */
} breath_state_t;

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

void breath_process(void);
void breath_1ms_isr(void);
void breath_init(void);

#endif //__DRV_PWM_BREATH_H__
