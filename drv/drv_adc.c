/*_____ I N C L U D E S ____________________________________________________*/
// #include <stdio.h>
#include "r_smc_entry.h"

#include "drv_adc.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/

typedef enum
{
    ADC_FSM_IDLE = 0,
    ADC_FSM_VREF_START,
    ADC_FSM_VREF_WAIT,
    ADC_FSM_CH_START,
    ADC_FSM_CH_WAIT
} adc_fsm_t;

typedef struct
{
    volatile adc_fsm_t fsm;
    volatile unsigned char seq_idx;     /* 0..seq_len-1 */
    volatile unsigned char busy;
    volatile unsigned char cur_is_vref;
    volatile unsigned short last_raw;

    unsigned char seq_len;
    e_ad_channel_t seq[ADC_CH_MAX];    /* channels to scan */
} adc_mgr_t;

static volatile adc_mgr_t g_adc_mgr;
// static volatile adc_data_t g_adc;
volatile adc_data_t g_adc;// for debug purpose

static const e_ad_channel_t g_adc_seq[] =
{
    ADCHANNEL4,
    ADCHANNEL5,
    ADCHANNEL6
};

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

/*
    COPY FROM R_Config_ADC_Create
    channel : internal reference voltage
*/
void ADC_VREF_config_Init(void)
{    	
    ADCEN = 1U;    /* supply AD clock */
    ADMK = 1U;    /* disable INTAD interrupt */
    ADIF = 0U;    /* clear INTAD interrupt flag */
    /* Set INTAD priority */
    ADPR1 = 1U;
    ADPR0 = 1U;
    ADM0 = _00_AD_CONVERSION_CLOCK_8 | _00_AD_TIME_MODE_NORMAL_1;
    ADM2 = _00_AD_RESOLUTION_10BIT;
    ADS = _0D_AD_INPUT_INTERREFVOLT;

}

/*
    COPY FROM R_Config_ADC_Create
*/
void ADC_Channel_config_Init(void)
{    		
    ADCEN = 1U;    /* supply AD clock */
    ADMK = 1U;    /* disable INTAD interrupt */
    ADIF = 0U;    /* clear INTAD interrupt flag */
    /* Set INTAD priority */
    ADPR1 = 1U;
    ADPR0 = 1U;

    /* Set ANI4 pin */
    PMC0 |= 0x20U;
    PM0 |= 0x20U;
    TSSEL0 &= 0xDFU;

    /* Set ANI5 pin */
    PMC0 |= 0x40U;
    PM0 |= 0x40U;
    TSSEL0 &= 0xBFU;

    /* Set ANI6 pin */
    PMC0 |= 0x80U;
    PM0 |= 0x80U;
    TSSEL0 &= 0x7FU;    
    
    ADM0 = _00_AD_CONVERSION_CLOCK_8 | _00_AD_TIME_MODE_NORMAL_1;
    ADM2 = _00_AD_RESOLUTION_10BIT;
    ADS = _04_AD_INPUT_CHANNEL_4;	
		
}

static void adc_start_vref(void)
{
    g_adc.done = 0U;
    g_adc_mgr.busy = 1U;
    g_adc_mgr.cur_is_vref = 1U;

    ADC_VREF_config_Init();
    R_Config_ADC_Set_OperationOn();
    R_Config_ADC_Start();
}

static void adc_start_channel(e_ad_channel_t ch)
{
    g_adc.done = 0U;
    g_adc_mgr.busy = 1U;
    g_adc_mgr.cur_is_vref = 0U;

    R_Config_ADC_Set_ADChannel(ch);
    R_Config_ADC_Set_OperationOn();
    R_Config_ADC_Start();
}

int drv_adc_set_sequence(const e_ad_channel_t *list, unsigned char len)
{
    unsigned char i;

    if ((list == 0) || (len == 0U) || (len > ADC_CH_MAX))
    {
        return -1;
    }

    for (i = 0U; i < len; i++)
    {
        g_adc_mgr.seq[i] = list[i];
    }
    g_adc_mgr.seq_len = len;
    g_adc_mgr.seq_idx = 0U;

    return 0;
}

int drv_adc_get_by_channel(e_ad_channel_t ch, unsigned short *raw, unsigned short *mv)
{
    unsigned char i;

    if ((raw == 0) || (mv == 0))
    {
        return -1;
    }

    for (i = 0U; i < g_adc_mgr.seq_len; i++)
    {
        if (g_adc_mgr.seq[i] == ch)
        {
            *raw = g_adc.raw_seq[i];
            *mv  = g_adc.mv_seq[i];
            return 0;
        }
    }

    return -2; /* not found */
}


void drv_adc_process_log(void)
{
    unsigned short raw = 0; 
    unsigned short mv = 0;
    unsigned char i = 0;
    unsigned char len = 0;

    len = (sizeof(g_adc_seq) / sizeof(g_adc_seq[0]));
    for ( i = 0 ; i < len ; i++)
    {
        drv_adc_get_by_channel(g_adc_seq[i],&raw,&mv);
        
#if defined(ENABLE_LOG_ADC)
        if (i == 0)
        {
            printf_tiny("adc");
        }

        printf_tiny("[%u]:",g_adc_seq[i]);
        printf_tiny("0x%4X",raw);
        printf_tiny("(%4umv),",mv);

        if (i == (len-1))
        {
            printf_tiny("\r\n");
        }
#endif
    }
}

/* call periodically, e.g. every 10ms */
void drv_adc_process_poll(void)
{
    e_ad_channel_t ch;
    unsigned short tmp;
    unsigned short adc_resolution;
    unsigned long mv;

    tmp = 0U;
    adc_resolution = 1024U;
    mv = 0UL;

    switch (g_adc_mgr.fsm)
    {
        default:
        case ADC_FSM_IDLE:
            /* Start with Vref calibration once at boot, or periodically (e.g. 1s) */
            adc_start_vref();
            g_adc_mgr.fsm = ADC_FSM_VREF_WAIT;
            break;

        case ADC_FSM_VREF_WAIT:
            if (g_adc.done != 0U)
            {
                g_adc.done = 0U;
                tmp = g_adc_mgr.last_raw;

                if (tmp == 0U)
                {
                    g_adc.vdd_mv = 0UL;
                }
                else
                {
                    g_adc.vdd_mv = (unsigned long)VBG_VOLTAGE * (unsigned long)adc_resolution / (unsigned long)tmp;
                }

#if defined(ENABLE_LOG_ADC)
                printf_tiny("Read VREF:%u(0x%04X),",tmp,tmp);
                printf_tiny("VDD:%lu",g_adc.vdd_mv);
                printf_tiny("\r\n");
#endif

                /* Switch back to normal channel config */
                ADC_Channel_config_Init();

                g_adc_mgr.fsm = ADC_FSM_CH_START;
            }
            break;

        case ADC_FSM_CH_START:
        {
            if (g_adc_mgr.busy == 0U)
            {
                if (g_adc_mgr.seq_len == 0U)
                {
                    /* No channel configured -> stay idle or just return */
                    break;
                }

                ch = g_adc_mgr.seq[g_adc_mgr.seq_idx];
                adc_start_channel(ch);
                g_adc_mgr.fsm = ADC_FSM_CH_WAIT;
            }
            break;
        }

        case ADC_FSM_CH_WAIT:
        {
            if (g_adc.done != 0U)
            {
                g_adc.done = 0U;

                tmp = g_adc_mgr.last_raw;
                ch = g_adc_mgr.seq[g_adc_mgr.seq_idx];

                /* SAFETY: ensure channel enum can be used as array index */
                if ((unsigned char)ch < (unsigned char)ADC_CH_MAX)
                {
                    g_adc.raw_seq[g_adc_mgr.seq_idx] = tmp;
                    mv = ((unsigned long)g_adc.vdd_mv * (unsigned long)tmp) >> 10;
                    g_adc.mv_seq[g_adc_mgr.seq_idx] = (unsigned short)mv;
                }
                else
                {
                    /* Optional: set an error flag */
                }

                /* next */
                g_adc_mgr.seq_idx++;
                if (g_adc_mgr.seq_idx >= g_adc_mgr.seq_len)
                {
                    g_adc_mgr.seq_idx = 0U;
                }

                g_adc_mgr.fsm = ADC_FSM_CH_START;
            }
            break;
        }

    }
}

void drv_adc_process_in_irq_handler(void)
{
    unsigned short tmp_buffer;

    tmp_buffer = 0U;

    R_Config_ADC_Get_Result_10bit(&tmp_buffer);
    R_Config_ADC_Stop();
    R_Config_ADC_Set_OperationOff();

    g_adc_mgr.last_raw = tmp_buffer;
    g_adc.done = 1U;
    g_adc_mgr.busy = 0U;
}

void drv_adc_process_init(void)
{    
    drv_adc_set_sequence(g_adc_seq, sizeof(g_adc_seq) / sizeof(g_adc_seq[0]));
}

