/*_____ I N C L U D E S ____________________________________________________*/
// #include <stdio.h>
#include "r_smc_entry.h"

#include "drv_pwm_output.h"
#include "drv_pwm_breath.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/

static volatile breath_state_t g_breath;
static volatile unsigned char g_breath_update_req;

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

void breath_process(void)
{
    unsigned short v;

    if (g_breath_update_req == 0U)
    {
        return;
    }
    g_breath_update_req = 0U;

    v = g_breath.level_x10;

    if (g_breath.dir == 0U)
    {
        /* ramp up */
        if ((unsigned short)(1000U - v) <= g_breath.step_x10)
        {
            v = 1000U;
            g_breath.dir = 1U;
        }
        else
        {
            v = (unsigned short)(v + g_breath.step_x10);
        }
    }
    else
    {
        /* ramp down */
        if (v <= g_breath.step_x10)
        {
            v = 0U;
            g_breath.dir = 0U;
        }
        else
        {
            v = (unsigned short)(v - g_breath.step_x10);
        }
    }

    g_breath.level_x10 = v;

    /* ch3 */
    pwm_out_set_duty_percent_x10(CHANNEL3, g_breath.level_x10);
}


void breath_1ms_isr(void)
{
    if (g_breath.enable)
    {
        g_breath.acc_ms++;
        if (g_breath.acc_ms >= g_breath.update_ms)
        {
            g_breath.acc_ms = 0U;
            g_breath_update_req = 1U;
        }
    }
}

void breath_init(void)
{
    g_breath.level_x10 = 0U;
    g_breath.dir = 0U;
    g_breath.enable = 1U;

    g_breath.acc_ms = 0U;
    g_breath.update_ms = 10U;      /* update every 10ms */

    /* 2s ramp up with 10ms update => 200 steps => step=1000/200=5 (0.5%) */
    g_breath.step_x10 = 5U;
}

