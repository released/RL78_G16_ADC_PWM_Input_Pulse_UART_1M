/*_____ I N C L U D E S ____________________________________________________*/
// #include <stdio.h>
#include "r_smc_entry.h"

#include "drv_pwm_output.h"
#include "drv_pwm_sweep.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/

static volatile pwm_sweep_state_t g_pwm_sweep;

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

void pwm_sweep_process(void)
{
    signed short s;
    unsigned char next;

    if (g_pwm_sweep.tick_1s == 0U)
    {
        return;
    }
    g_pwm_sweep.tick_1s = 0U;

    s = (signed short)g_pwm_sweep.duty + (signed short)g_pwm_sweep.step;

    if (s >= 100)
    {
        next = 100U;
        g_pwm_sweep.step = -10;
    }
    else if (s <= 0)
    {
        next = 0U;
        g_pwm_sweep.step = 10;
    }
    else
    {
        next = (unsigned char)s;
    }

    g_pwm_sweep.duty = next;
    pwm_out_set_duty_percent(CHANNEL5, next);
}

void pwm_sweep_1ms_isr(void)
{
    g_pwm_sweep.acc_ms++;

    if (g_pwm_sweep.acc_ms >= 1000U)
    {
        g_pwm_sweep.acc_ms = 0U;
        g_pwm_sweep.tick_1s = 1U;
    }
}

void pwm_sweep_init(void)
{
    unsigned char p;

    p = pwm_out_get_duty_percent(CHANNEL5);
    g_pwm_sweep.duty   = p;
    g_pwm_sweep.acc_ms = 0U;
    g_pwm_sweep.tick_1s = 0U;

    /* Decide direction */
    if (p >= 100U)
    {
        g_pwm_sweep.step = -10;
    }
    else if (p == 0U)
    {
        g_pwm_sweep.step = 10;
    }
    else
    {
        g_pwm_sweep.step = 10;
    }

}
