#ifndef __DRV_PWM_OUTPUT_H__
#define __DRV_PWM_OUTPUT_H__

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

/* ---- PWM OUT ---- */
#define PWM_OUT_RESOLUTION_100              	(100UL)
#define PWM_OUT_RESOLUTION_1000         		(1000UL)

#define PWM_OUT_CH_MAX                     		(8U)
/* Bit mask helper: BIT(ch) */
#define TAU0_CH_BIT(ch)    						(1UL << (ch))

typedef struct
{
    volatile unsigned long enable_mask;                 	/* which channels are managed */
    volatile unsigned short tdr_default[PWM_OUT_CH_MAX];   	/* default TDRxx per channel */
    volatile unsigned short duty[PWM_OUT_CH_MAX];          	/* current duty count */
} pwm_out_state_t;

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

void pwm_out_init_from_hw(unsigned long enable_mask , unsigned char master_channel);

/* Set duty by percent (0..100) or 0.1% (0..1000) */
void pwm_out_set_duty_percent(unsigned char ch, unsigned char percent);
void pwm_out_set_duty_percent_x10(unsigned char ch, unsigned short percent_x10);

/* Set duty by raw TDR count */
void pwm_out_set_duty_cnt(unsigned char ch, unsigned short duty_cnt);

/* Optional getters */
unsigned short pwm_out_get_default(unsigned char ch);
unsigned short pwm_out_get_duty_cnt(unsigned char ch);
unsigned char pwm_out_get_duty_percent(unsigned char ch);

#endif //__DRV_PWM_OUTPUT_H__
