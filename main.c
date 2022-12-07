/******************************************************************************
* File Name:   main.c
*
* Description:
* This is the source code for the PMG1 MCU: Basic Device Firmware Upgrade (DFU)
* Example for ModusToolbox.
* This file implements the simple blinky application that can be bootloaded by the
* Bootloader. 
*
* Related Document: See README.md
*
*******************************************************************************
* $ Copyright 2022 Cypress Semiconductor $
*******************************************************************************/

#include "cy_pdl.h"
#include "cybsp.h"
#include "cy_dfu.h"

/*******************************************************************************
* Macros
********************************************************************************/
/* App ID of bootloader. */
#define BOOTLOADER_ID                       (0u)

/* Define LED toggle frequency. */
#define LED_TOGGLE_INTERVAL_MS              (100u)

/* User button interrupt priority. */
#define GPIO_INTERRUPT_PRIORITY             (3u)

/*******************************************************************************
* Function Prototypes
********************************************************************************/
static void user_button_event_handler(void);

/*******************************************************************************
* Global Variables
********************************************************************************/
/* Application signature. */
CY_SECTION(".cy_app_signature") __USED static const uint32_t cy_dfu_appSignature;

volatile static bool is_user_event_detected = false;

/* User button interrupt configuration */
const cy_stc_sysint_t user_button_intr_config =
{
    .intrSrc = CYBSP_USER_BTN_IRQ,    /* Source of interrupt signal */
    .intrPriority = 3u,               /* Interrupt priority */
};

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function for CM0 CPU.
*   1. It blinks LED at "LED_TOGGLE_INTERVAL_MS" interval.
*   2. If user button press event is detected, it will transfer control to 
*      Bootloader. 
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Configure interrupt */
    Cy_SysInt_Init(&user_button_intr_config, &user_button_event_handler);

    /* Clear any pendigng interrupt and enable interrupt. */
    NVIC_ClearPendingIRQ(user_button_intr_config.intrSrc);
    NVIC_EnableIRQ(user_button_intr_config.intrSrc);

    /* Enable global interrupts */
    __enable_irq();

    for (;;)
    {
        /* Invert the USER LED state */
        Cy_GPIO_Inv(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN);

        /* Delay between LED toggles */
        Cy_SysLib_Delay(LED_TOGGLE_INTERVAL_MS);

        /* Switch to bootloader, if user button is pressed. */
        if(is_user_event_detected == true)
        {
            is_user_event_detected = false;
            Cy_DFU_ExecuteApp(BOOTLOADER_ID);
        }
    }
}

/*******************************************************************************
* Function Name: user_button_event_handler
********************************************************************************
* Summary:
*   User Button event handler.
*
* Parameters:
*  void
*
*******************************************************************************/
static void user_button_event_handler(void)
{
    /* Clear the interrupt */
    Cy_GPIO_ClearInterrupt(CYBSP_USER_BTN_PORT, CYBSP_USER_BTN_PIN);
    NVIC_ClearPendingIRQ(user_button_intr_config.intrSrc);
    
    is_user_event_detected = true;
}

/* [] END OF FILE */
