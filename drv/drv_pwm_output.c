/*_____ I N C L U D E S ____________________________________________________*/
// #include <stdio.h>
#include "r_smc_entry.h"

#include "drv_pwm_output.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/

volatile pwm_out_state_t g_pwm_out;   

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

/* Read TDRxx by channel number */
static unsigned short pwm_out_read_tdr(unsigned char tau_ch)
{
    unsigned short v;
    v = 0U;

    switch (tau_ch)
    {
        case CHANNEL0: v = TDR00; break;
        case CHANNEL1: v = TDR01; break;
        case CHANNEL2: v = TDR02; break;
        case CHANNEL3: v = TDR03; break;
        case CHANNEL4: v = TDR04; break;
        case CHANNEL5: v = TDR05; break;
        case CHANNEL6: v = TDR06; break;
        case CHANNEL7: v = TDR07; break;
        default: v = 0U;    break;
    }
    return v;
}

/* Write TDRxx by channel number */
static void pwm_out_write_tdr(unsigned char tau_ch, unsigned short v)
{
    switch (tau_ch)
    {
        case CHANNEL0: TDR00 = v; break;
        case CHANNEL1: TDR01 = v; break;
        case CHANNEL2: TDR02 = v; break;
        case CHANNEL3: TDR03 = v; break;
        case CHANNEL4: TDR04 = v; break;
        case CHANNEL5: TDR05 = v; break;
        case CHANNEL6: TDR06 = v; break;
        case CHANNEL7: TDR07 = v; break;
        default:            break;
    }
}

static unsigned char pwm_out_is_enabled(unsigned char ch)
{
    if (ch >= PWM_OUT_CH_MAX)
    {
        return 0U;
    }
    if ((g_pwm_out.enable_mask & TAU0_CH_BIT(ch)) == 0UL)
    {
        return 0U;
    }
    return 1U;
}


void pwm_out_init_from_hw(unsigned long enable_mask , unsigned char master_channel)
{
    unsigned char ch;
    unsigned short master_def;

    g_pwm_out.enable_mask = enable_mask;

    master_def = pwm_out_read_tdr(master_channel);
    master_def = (unsigned short)(master_def + 1U); /* if you want +1, keep it here for now */

    for (ch = 0U; ch < PWM_OUT_CH_MAX; ch++)
    {
        g_pwm_out.tdr_default[ch] = 0U;
        g_pwm_out.duty[ch] = 0U;

        if ((g_pwm_out.enable_mask & TAU0_CH_BIT(ch)) != 0UL)
        {
            g_pwm_out.tdr_default[ch] = master_def;
            g_pwm_out.duty[ch] = pwm_out_read_tdr(ch);
        }
    }
}

void pwm_out_set_duty_cnt(unsigned char ch, unsigned short duty_cnt)
{
    unsigned short def;

    if (pwm_out_is_enabled(ch) == 0U)
    {
        return;
    }

    def = g_pwm_out.tdr_default[ch];
    if (duty_cnt > def)
    {
        duty_cnt = def;
    }

    g_pwm_out.duty[ch] = duty_cnt;
    pwm_out_write_tdr(ch, duty_cnt);
}

void pwm_out_set_duty_percent(unsigned char ch, unsigned char percent)
{
    unsigned long tmp;
    unsigned short def;
    unsigned short duty_cnt;

    if (pwm_out_is_enabled(ch) == 0U)
    {
        return;
    }

    if (percent > PWM_OUT_RESOLUTION_100)
    {
        percent = PWM_OUT_RESOLUTION_100;
    }

    def = g_pwm_out.tdr_default[ch];
    tmp = (unsigned long)def * (unsigned long)percent;
    duty_cnt = (unsigned short)(tmp / PWM_OUT_RESOLUTION_100);

    pwm_out_set_duty_cnt(ch, duty_cnt);
}

void pwm_out_set_duty_percent_x10(unsigned char ch, unsigned short percent_x10)
{
    unsigned long tmp;
    unsigned short def;
    unsigned short duty_cnt;

    if (pwm_out_is_enabled(ch) == 0U)
    {
        return;
    }

    if (percent_x10 > PWM_OUT_RESOLUTION_1000)
    {
        percent_x10 = PWM_OUT_RESOLUTION_1000;
    }

    def = g_pwm_out.tdr_default[ch];
    tmp = (unsigned long)def * (unsigned long)percent_x10;
    duty_cnt = (unsigned short)(tmp / PWM_OUT_RESOLUTION_1000);

    pwm_out_set_duty_cnt(ch, duty_cnt);
}

unsigned short pwm_out_get_default(unsigned char ch)
{
    if (ch >= PWM_OUT_CH_MAX) { return 0U; }
    return g_pwm_out.tdr_default[ch];
}

unsigned short pwm_out_get_duty_cnt(unsigned char ch)
{
    if (ch >= PWM_OUT_CH_MAX) { return 0U; }
    return g_pwm_out.duty[ch];
}

unsigned char pwm_out_get_duty_percent(unsigned char ch)
{
    unsigned short duty_cnt;
    unsigned short def_cnt;
    unsigned long tmp;
    unsigned char percent;

    duty_cnt = pwm_out_get_duty_cnt(ch);
    def_cnt  = pwm_out_get_default(ch);

    if (def_cnt == 0U)
    {
        return 0U;
    }

    #if 1
    /* 
       percent = round(duty_cnt * 100 / def_cnt)
       -> (duty_cnt * 100 + def_cnt / 2) / def_cnt
    */
    tmp = (unsigned long)duty_cnt * 100UL;
    tmp = tmp + ((unsigned long)def_cnt >> 1);
    #else    
    /* percent = duty_cnt * 100 / def_cnt */
    tmp = (unsigned long)duty_cnt * 100UL;
    #endif

    percent = (unsigned char)(tmp / (unsigned long)def_cnt);

    if (percent > 100U)
    {
        percent = 100U;
    }

    return percent;
}



