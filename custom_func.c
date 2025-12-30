/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include <string.h>

#include "r_smc_entry.h"
#include "platform.h"
#include "r_cg_userdefine.h"

#include "timer_service.h"
#include "misc_config.h"
#include "custom_func.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

volatile struct flag_32bit flag_PROJ_CTL;
#define FLAG_PROJ_TIMER_PERIOD_1000MS                 	(flag_PROJ_CTL.bit0)
#define FLAG_PROJ_REVERSE1                 	            (flag_PROJ_CTL.bit1)
#define FLAG_PROJ_REVERSE2                 	            (flag_PROJ_CTL.bit2)
#define FLAG_PROJ_REVERSE3                    		    (flag_PROJ_CTL.bit3)
#define FLAG_PROJ_REVERSE4                              (flag_PROJ_CTL.bit4)
#define FLAG_PROJ_REVERSE5                              (flag_PROJ_CTL.bit5)
#define FLAG_PROJ_REVERSE6                              (flag_PROJ_CTL.bit6)
#define FLAG_PROJ_REVERSE7                              (flag_PROJ_CTL.bit7)

/*_____ D E F I N I T I O N S ______________________________________________*/

volatile unsigned long counter_tick = 0;

static signed char g_timer_id_task1 = -1;
static signed char g_timer_id_task2 = -1;
static signed char g_timer_id_task3 = -1;
static signed char g_timer_id_task4 = -1;

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/


unsigned long get_tick(void)
{
	return (counter_tick);
}

void set_tick(unsigned long t)
{
	counter_tick = t;
}

void tick_counter(void)
{
	counter_tick++;
}

void delay_ms(unsigned long ms)
{
	#if 1
	unsigned long start = get_tick();
    while ((unsigned long)(get_tick() - start) < (unsigned long)ms) 
	{
		
	}
	#else
	TIMER_Delay(TIMER0, 1000*ms);
	#endif
}

void Timer_1ms_IRQ(void)
{
    tick_counter();

    // timer
    /* Each interrupt = 1 ms tick (Timer irq : 1ms) */
    if (g_ts_tick_pending < 0xFFu)
    {
        g_ts_tick_pending++;
    }

    // pwm
    breath_1ms_isr();
    pwm_sweep_1ms_isr();
}

/*
    G16 target board
    LED1 connected to P12, LED2 connected to P15
*/
// void LED_Toggle(void)
// {
//     // PIN_WRITE(2,0) = ~PIN_READ(2,0);
//     // PIN_WRITE(2,1) = ~PIN_READ(2,1);
//     P1_bit.no2 = ~P1_bit.no2;
//     P1_bit.no5 = ~P1_bit.no5;
// }

void task_10ms(void *user_data)
{
    breath_process();
    pwm_sweep_process();

    // app_uart_rx_poll_and_dump();
}

void task_50ms(void *user_data)
{
}

void task_100ms(void *user_data)
{
    drv_adc_process_poll();
    app_uart_send_test_packet();
}

void task_1000ms(void *user_data)
{
    drv_pwm_input_log();
    drv_adc_process_log();    
}

void TimerService_CreateTask(void)
{
    /* Create task1 timer: 1000 ms */
    g_timer_id_task1 = TimerService_CreateTimer(1000U, task_1000ms, (void *)0);
    if (g_timer_id_task1 >= 0)
    {
        TimerService_StartTimer((unsigned int)g_timer_id_task1);
        // printf_tiny("task1 id = %u\r\n", g_timer_id_task1);
    }

    /* Create task2 timer: 100 ms */
    g_timer_id_task2 = TimerService_CreateTimer(100U, task_100ms, (void *)0);
    if (g_timer_id_task2 >= 0)
    {
        TimerService_StartTimer((unsigned int)g_timer_id_task2);
        // printf_tiny("task2 id = %u\r\n", g_timer_id_task2);
    }

    /* Create task3 timer: 50 ms */
    g_timer_id_task3 = TimerService_CreateTimer(50U, task_50ms, (void *)0);
    if (g_timer_id_task3 >= 0)
    {
        TimerService_StartTimer((unsigned int)g_timer_id_task3);
        // printf_tiny("task3 id = %u\r\n", g_timer_id_task3);
    }

    /* Create task4 timer: 10 ms */
    g_timer_id_task4 = TimerService_CreateTimer(10U, task_10ms, (void *)0);
    if (g_timer_id_task4 >= 0)
    {
        TimerService_StartTimer((unsigned int)g_timer_id_task4);
        // printf_tiny("task4 id = %u\r\n", g_timer_id_task4);
    }
}

void loop(void)
{   
    /* handle all pending ticks and callbacks */
    TimerService_Process();

    // drv period    
    drv_pwm_input_poll();
    app_uart_rx_poll_and_dump();
}


// G16 EVB , P137/INTP0 , set both edge 
// void Button_Process_long_counter(void)
// {
//     if (FLAG_PROJ_TRIG_BTN2)
//     {
//         btn_tick_counter();
//     }
//     else
//     {
//         btn_set_tick(0);
//     }
// }

// void Button_Process_in_polling(void)
// {
//     static unsigned char cnt = 0;

//     if (FLAG_PROJ_TRIG_BTN1)
//     {
//         FLAG_PROJ_TRIG_BTN1 = 0;
//         printf_tiny("BTN pressed(%d)\r\n",cnt);

//         if (cnt == 0)   //set both edge  , BTN pressed
//         {
//             FLAG_PROJ_TRIG_BTN2 = 1;
//         }
//         else if (cnt == 1)  //set both edge  , BTN released
//         {
//             FLAG_PROJ_TRIG_BTN2 = 0;
//         }

//         cnt = (cnt >= 1) ? (0) : (cnt+1) ;
//     }

//     if ((FLAG_PROJ_TRIG_BTN2 == 1) && 
//         (btn_get_tick() > BTN_PRESSED_LONG))
//     {         
//         printf_tiny("BTN pressed LONG\r\n");
//         btn_set_tick(0);
//         FLAG_PROJ_TRIG_BTN2 = 0;
//     }
// }

// G16 EVB , P137/INTP0
// void Button_Process_in_IRQ(void)    
// {
//     FLAG_PROJ_TRIG_BTN1 = 1;
// }

void UARTx_Process(unsigned char rxbuf)
{    
    if (rxbuf > 0x7F)
    {
        printf_tiny("invalid command\r\n");
    }
    else
    {
        printf_tiny("press:%c(0x%02X)\r\n" , rxbuf,rxbuf);   // %c :  C99 libraries.
        switch(rxbuf)
        {

            case 'X':
            case 'x':
                RL78_soft_reset(7);
                break;
            case 'Z':
            case 'z':
                RL78_soft_reset(1);
                break;
        }
    }
}

/*
    Reset Control Flag Register (RESF) 
    BIT7 : TRAP
    BIT6 : 0
    BIT5 : 0
    BIT4 : WDCLRF
    BIT3 : 0
    BIT2 : 0
    BIT1 : IAWRF
    BIT0 : LVIRF
*/
// void check_reset_source(void)
// {
//     /*
//         Internal reset request by execution of illegal instruction
//         0  Internal reset request is not generated, or the RESF register is cleared. 
//         1  Internal reset request is generated. 
//     */
//     uint8_t src = RESF;
//     printf_tiny("Reset Source <0x%08X>\r\n", src);

//     #if 1   //DEBUG , list reset source
//     if (src & BIT0)
//     {
//         printf_tiny("0)voltage detector (LVD)\r\n");       
//     }
//     if (src & BIT1)
//     {
//         printf_tiny("1)illegal-memory access\r\n");       
//     }
//     if (src & BIT2)
//     {
//         printf_tiny("2)EMPTY\r\n");       
//     }
//     if (src & BIT3)
//     {
//         printf_tiny("3)EMPTY\r\n");       
//     }
//     if (src & BIT4)
//     {
//         printf_tiny("4)watchdog timer (WDT) or clock monitor\r\n");       
//     }
//     if (src & BIT5)
//     {
//         printf_tiny("5)EMPTY\r\n");       
//     }
//     if (src & BIT6)
//     {
//         printf_tiny("6)EMPTY\r\n");       
//     }
//     if (src & BIT7)
//     {
//         printf_tiny("7)execution of illegal instruction\r\n");       
//     }
//     #endif

// }

/*
    7:Internal reset by execution of illegal instruction
    1:Internal reset by illegal-memory access
*/
//perform sofware reset
void _reset_by_illegal_instruction(void)
{
    static const unsigned char illegal_Instruction = 0xFF;
    void (*dummy) (void) = (void (*)(void))&illegal_Instruction;
    dummy();
}
void _reset_by_illegal_memory_access(void)
{
    // #if 1
    // const unsigned char ILLEGAL_ACCESS_ON = 0x80;
    // IAWCTL |= ILLEGAL_ACCESS_ON;            // switch IAWEN on (default off)
    // *(__far volatile char *)0x00000 = 0x00; //write illegal address 0x00000(RESET VECTOR)
    // #else
    // signed char __far* a;                   // Create a far-Pointer
    // IAWCTL |= _80_CGC_ILLEGAL_ACCESS_ON;    // switch IAWEN on (default off)
    // a = (signed char __far*) 0x0000;        // Point to 0x000000 (FLASH-ROM area)
    // *a = 0;
    // #endif
}

void RL78_soft_reset(unsigned char flag)
{
    switch(flag)
    {
        case 7: // do not use under debug mode
            _reset_by_illegal_instruction();        
            break;
        case 1:
            _reset_by_illegal_memory_access();
            break;
    }
}

// retarget printf
int __far putchar(int c)
{
    // G16 , UART0
    STMK0 = 1U;    /* disable INTST0 interrupt */
    TXD0 = (unsigned char)c;
    while(STIF0 == 0)
    {

    }
    STIF0 = 0U;    /* clear INTST0 interrupt flag */
    return c;
}

void hardware_init(void)
{
    BSP_EI();
    R_Config_UART0_Start();         // UART0 , P03 , P04

    /*
        UART2 : 1M bps
        P20 : RX
        P21 : TX    
    */
    R_Config_UART2_Start(); 
    app_uart_init();
        
    R_Config_TAU0_0_Start();        // TIMER , 100us
    R_Config_TAU0_1_Start();        // TIMER , 1ms

    TimerService_Init();
    TimerService_CreateTask();

    /*
        PWM output
        P41/TO03
        P122/TO05
        P22/TO06
        P121/TO07
    */
    R_Config_TAU0_2_Start();   

    /*
        TODO : set duty to 100% , to get correct TDR value
    */
    pwm_out_init_from_hw(BIT3 | BIT5 | BIT6 | BIT7 , CHANNEL2); 

    pwm_out_set_duty_percent(CHANNEL3, 20U);  /* TO03 / ch3 -> 20% */
    pwm_out_set_duty_percent(CHANNEL5, 40U);  /* TO05 / ch5 -> 40% */
    pwm_out_set_duty_percent(CHANNEL6, 60U);  /* TO06 / ch6 -> 60% */      
    pwm_out_set_duty_percent(CHANNEL7, 80U);  /* TO07 / ch7 -> 80% */
    
    breath_init();                      // ch3 will change duty
    pwm_sweep_init();                   // ch5 will change duty

    /*
        PWM capture
        P23/TI04
    */
    drv_pwm_input_init();

    /*
        ADC
        P05/ANI4
        P06/ANI5
        P07/ANI6
    */
    drv_adc_process_init();   


    // check_reset_source();
    // printf("%s finish\r\n\r\n",__func__);
    printf_tiny("%s finish\r\n\r\n",__func__);
}
