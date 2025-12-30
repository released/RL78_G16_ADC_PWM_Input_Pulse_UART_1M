# RL78_G16_ADC_PWM_Input_Pulse_UART_1M
RL78_G16_ADC_PWM_Input_Pulse_UART_1M

update @ 2025/12/23

1. base on EVM : RL78 G16 (MCU pin assignment set to 20PIN) , to init below function

- UART0 : P03(TX) , P04(RX) , for debug purpose  , baud rate : 115200

- UART2 : P21(TX) , P20(RX) , to send TX packet and receive RX data , baud rate : 1000000

- TIMER(TAU0_0) : for 100us interrupt tick

- TIMER(TAU0_1) : for 1ms interrupt tick

- PWM output : (same master clock : 10KHz)

	- P41/TO03 : duty change as breathing LED
	
	- P122/TO05 : duty change as sweep LED
	
	- P22/TO06 : duty fixed at 60% when initial
        
	- P121/TO07 : duty fixed at 80% when initial
	
- PWM input : P23/TI04

	- CLOCK source set to 16M (SYS clock)
	
- ADC : P05/ANI4 , P06/ANI5 , P07/ANI6


2. if need to monitor global vaule/structure in watch windows , make sure setting as below , 


![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/enable_watch_windows.jpg)


3. MCU pin assignment set to 20PIN


![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/mcu_package_view.jpg)


4. below is log message : ADC , when enable below define in drv_adc.h

```c
// #define ENABLE_LOG_ADC
```

![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/log_adc.jpg)


when monitor result in watch window , 


![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/watch_adc.jpg)


5. below is log message : UART 2 receive packet , when enable below define in drv_uart.h

```c
// #define ENABLE_LOG_UART
```

![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/log_uart_rx.jpg)


TX scenario , will send packet per 100ms , and count at TAIL byte (CH2 : TX)


![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/scope_ch2_tx_100ms.jpg)


![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/scope_ch2_tx_decode.jpg)


when connect TX pin to RX pin for loop back test (CH1 : RX , CH2 : TX)


![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/scope_ch2_tx_rx_100ms.jpg)


![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/scope_ch2_tx_rx_head.jpg)


![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/scope_ch2_tx_rx_tail.jpg)


when monitor result in watch window , 


![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/watch_uart.jpg)


6. below is PWM output , DUTY changes


monitor P41/TO03 (breathing LED) , P122/TO05 (sweep LED)


![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/scope_pwm_out_1.jpg)


![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/scope_pwm_out_2.jpg)


when monitor result in watch window , 


SLAVE channel TDR result will be different , ex : set to 50 % duty 

![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/watch_pwm_out_1.jpg)


SLAVE channel TDR result will be different , ex : set to 100 % duty 

![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/watch_pwm_out_2.jpg)


when smart configurator , PWM OUT set 10K frequency

![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/smc_pwm_output_10K.jpg)


monitor P22/TO06 (duty fix at 60%)

![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/scope_pwm_out_10KHz.jpg)


when smart configurator , PWM OUT set 100Hz frequency

![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/smc_pwm_output_100Hz.jpg)


monitor P22/TO06 (duty fix at 60%)

![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/scope_pwm_out_100Hz..jpg)


when smart configurator , PWM OUT set 200Hz frequency

![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/smc_pwm_output_200Hz.jpg)


monitor P22/TO06 (duty fix at 60%)

![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/scope_pwm_out_200Hz..jpg)




7. below is PWM input capture log message , when enable below define in drv_pwm_input.h

```c
// #define ENABLE_LOG_CAPTURE
```

log : when receive 10KHz pulse

![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/log_pwm_input_1.jpg)


when monitor result in watch window (10KHz)


![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/watch_pwm_input_1.jpg)


log : when receive 100Hz pulse

![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/log_pwm_input_2.jpg)


when monitor result in watch window (100Hz)


![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/watch_pwm_input_2.jpg)


log : when receive 200Hz pulse

![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/log_pwm_input_3.jpg)


when monitor result in watch window (200Hz)


![image](https://github.com/released/RL78_G16_ADC_PWM_Input_Pulse_UART_1M/blob/main/watch_pwm_input_3.jpg)


