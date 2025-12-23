/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2021, 2025 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name        : Pin.c
* Version          : 1.0.0
* Device(s)        : R5F1216AxSP
* Description      : This file implements SMC pin code generation.
***********************************************************************************************************************/
/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Pins_Create
* Description  : This function initializes Smart Configurator pins
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Pins_Create(void)
{
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

    /* Set RxD2 pin */
    PMC2 &= 0xFEU;
    PM2 |= 0x01U;
    TSSEL1 &= 0xF7U;

    /* Set TxD2 pin */
    PMC2 &= 0xFDU;
    P2 |= 0x02U;
    PM2 &= 0xFDU;
    TSSEL1 &= 0xFBU;

    /* Set _RESET pin */
    /* No register setting has been required for pin */

}

