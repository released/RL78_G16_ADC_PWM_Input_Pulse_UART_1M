/*_____ I N C L U D E S ____________________________________________________*/
// #include <stdio.h>
#include "r_smc_entry.h"

#include "drv_pwm_input.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/

#define PWM_IN_PORT_SAMPLE()                            (P2_bit.no3)         /* P23 */
#define PWM_IN_MEASURE_START()                          (R_Config_TAU0_4_Start())
#define PWM_IN_MEASURE_STOP()                           (R_Config_TAU0_4_Stop())
#define PWM_IN_IRQ_FLAG                                 (TMIF04)
#define PWM_IN_DEFAULT_CLK_HZ                           (16000000UL)

volatile pwm_capture_state_t g_pwcap;

static volatile unsigned char s_edge_flag = 0xFFU;
static volatile unsigned char s_invalid_cnt = 0U;

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/
extern volatile uint32_t g_tau0_ch4_width;

void drv_pwm_input_clear(void)
{
    g_pwcap.high_cnt = 0UL;
    g_pwcap.low_cnt = 0UL;
    g_pwcap.total_cnt = 0UL;
    g_pwcap.duty_x10 = 0U;
    g_pwcap.freq_hz = 0UL;

    g_pwcap.clk_hz = PWM_IN_DEFAULT_CLK_HZ; /* default */

    g_pwcap.high_done = 0U;
    g_pwcap.low_done = 0U;
    g_pwcap.calc_ready = 0U;

    s_edge_flag = 0xFFU;
    s_invalid_cnt = 0U;
}

/*
    check in smart configurator , input pulse measurement , CLOCK SOURCE
*/
void drv_pwm_input_set_clock_hz(unsigned long clk_hz)
{
    /* Reject 0 to avoid divide-by-zero */
    if (clk_hz == 0UL)
    {
        return;
    }

    g_pwcap.clk_hz = clk_hz;
}

void drv_pwm_input_init(void)
{
    drv_pwm_input_clear();
    PWM_IN_MEASURE_START();
}

void drv_pwm_input_log(void)
{
    if (g_pwcap.calc_ready == 0U)
    {
        return;
    }

    /* Consume one measurement result */
    g_pwcap.calc_ready = 0U;

#if defined(ENABLE_LOG_CAPTURE)
    printf_tiny("Freq:%lu,H:%lu(0x%04lX),L:%lu(0x%04lX),Total:%lu,Duty:%u.%u%%\r\n\r\n",
        g_pwcap.freq_hz,
        g_pwcap.high_cnt, g_pwcap.high_cnt,
        g_pwcap.low_cnt,  g_pwcap.low_cnt,
        g_pwcap.total_cnt,
        (unsigned int)(g_pwcap.duty_x10 / 10U),
        (unsigned int)(g_pwcap.duty_x10 % 10U));
#endif
}

/*
 * Call this in TAU0 ch4 capture interrupt.
 * Rule:
 * - Read port twice to avoid metastability/noise.
 * - If stable and flag condition ok -> store width into struct.
 * - Do NOT calculate total/duty here.
 */
void drv_pwm_input_irq_handler(void)
{
    unsigned char p0;
    unsigned char p1;

    p0 = (unsigned char)PWM_IN_PORT_SAMPLE();
    p1 = (unsigned char)PWM_IN_PORT_SAMPLE();

    if ((0U == PWM_IN_IRQ_FLAG) && ((p0 & 0x01U) == (p1 & 0x01U)))
    {
        s_edge_flag = (unsigned char)(p0 & 0x01U);

        if (0U == s_edge_flag)
        {
            /* Falling edge -> high width captured */
            g_pwcap.high_cnt = (unsigned long)g_tau0_ch4_width;
            g_pwcap.high_done = 1U;
        }
        else
        {
            /* Rising edge -> low width captured */
            g_pwcap.low_cnt = (unsigned long)g_tau0_ch4_width;
            g_pwcap.low_done = 1U;
        }
    }
    else
    {
        s_invalid_cnt++;
        PWM_IN_IRQ_FLAG = 0U;
    }
}

/* Call in main loop: finalize one measurement pair */
void drv_pwm_input_poll(void)
{
    unsigned long total;
    unsigned long clk;

    if ((g_pwcap.high_done != 0U) && (g_pwcap.low_done != 0U))
    {
        g_pwcap.total_cnt = g_pwcap.high_cnt + g_pwcap.low_cnt;

        total = g_pwcap.total_cnt;
        if (total != 0UL)
        {
            /* duty_x10 = round(high * 1000 / total) */
            g_pwcap.duty_x10 = (unsigned short)((g_pwcap.high_cnt * 1000UL + (total / 2UL)) / total);
            if (g_pwcap.duty_x10 > 1000U)
            {
                g_pwcap.duty_x10 = 1000U;
            }

            /* freq_hz = round(clk / total_cnt) */
            clk = g_pwcap.clk_hz;
            if (clk != 0UL)
            {
                g_pwcap.freq_hz = (clk + (total / 2UL)) / total;
            }
            else
            {
                g_pwcap.freq_hz = 0UL;
            }            
        }
        else
        {
            g_pwcap.duty_x10 = 0U;
            g_pwcap.freq_hz = 0UL;
        }

        g_pwcap.calc_ready = 1U;

        /* Prepare next pair */
        g_pwcap.high_done = 0U;
        g_pwcap.low_done = 0U;
    }

}

