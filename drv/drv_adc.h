#ifndef __DRV_ADC_H__
#define __DRV_ADC_H__

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

/* ---- ADC ---- */

/*G16 V_BGR : 0.815 V*/
#define VBG_VOLTAGE                             (815)  // mv
#define ADC_CH_MAX                      		(11U)

typedef struct
{
    unsigned short raw_seq[ADC_CH_MAX];
    unsigned short mv_seq[ADC_CH_MAX];
    unsigned long  vdd_mv;
    volatile unsigned char done;
} adc_data_t;

/*_____ M A C R O S ________________________________________________________*/

// #define ENABLE_LOG_ADC

/*_____ F U N C T I O N S __________________________________________________*/

void drv_adc_process_log(void);
void drv_adc_process_poll(void);
void drv_adc_process_in_irq_handler(void);
void drv_adc_process_init(void);

#endif //__DRV_ADC_H__
