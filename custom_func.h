#ifndef __CUSTOM_FUNC_H__
#define __CUSTOM_FUNC_H__

/*_____ I N C L U D E S ____________________________________________________*/
#include "drv_adc.h"
#include "drv_pwm_output.h"
#include "drv_pwm_breath.h"
#include "drv_pwm_sweep.h"
#include "drv_pwm_input.h"
#include "drv_uart.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/
#define bool  _Bool

#ifndef TRUE
#define TRUE  (1)
#endif

#ifndef true
#define true  (1)
#endif

#ifndef FALSE
#define FALSE  (0)
#endif

#ifndef false
#define false  (0)
#endif

#ifndef BIT0
#define BIT0     (0x00000001UL)       ///< Bit 0 mask of an 32 bit integer
#endif

#ifndef BIT1
#define BIT1     (0x00000002UL)       ///< Bit 1 mask of an 32 bit integer
#endif

#ifndef BIT2
#define BIT2     (0x00000004UL)       ///< Bit 2 mask of an 32 bit integer
#endif

#ifndef BIT3
#define BIT3     (0x00000008UL)       ///< Bit 3 mask of an 32 bit integer
#endif

#ifndef BIT4
#define BIT4     (0x00000010UL)       ///< Bit 4 mask of an 32 bit integer
#endif

#ifndef BIT5
#define BIT5     (0x00000020UL)       ///< Bit 5 mask of an 32 bit integer
#endif

#ifndef BIT6
#define BIT6     (0x00000040UL)       ///< Bit 6 mask of an 32 bit integer
#endif

#ifndef BIT7
#define BIT7     (0x00000080UL)       ///< Bit 7 mask of an 32 bit integer
#endif

/*_____ D E F I N I T I O N S ______________________________________________*/

/*  
	template
	typedef struct _peripheral_manager_t
	{
		uint8_t u8Cmd;
		uint8_t au8Buf[33];
		uint8_t u8RecCnt;
		uint8_t bByPass;
		uint16_t* pu16Far;
	}PERIPHERAL_MANAGER_T;

	volatile PERIPHERAL_MANAGER_T g_PeripheralManager = 
	{
		.u8Cmd = 0,
		.au8Buf = {0},		//.au8Buf = {100U, 200U},
		.u8RecCnt = 0,
		.bByPass = FALSE,
		.pu16Far = NULL,	//.pu16Far = 0	
	};
	extern volatile PERIPHERAL_MANAGER_T g_PeripheralManager;
*/

typedef struct
{
    unsigned short period_ms;
    volatile unsigned short remain_ms;
    volatile unsigned char flag;
} soft_timer_t;

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

void Timer_1ms_IRQ(void);

// void Button_Process_long_counter(void);
// void Button_Process_in_polling(void);
// void Button_Process_in_IRQ(void);
void UARTx_Process(unsigned char rxbuf);

//void LED_Toggle(void);
void loop(void);

//void check_reset_source(void);
void RL78_soft_reset(unsigned char flag);
void hardware_init(void);

#endif //__CUSTOM_FUNC_H__
