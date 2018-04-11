#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_adc.h"
#include "fsl_clock.h"
#include "fsl_power.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "adc.h"
#include "fsl_gpio.h"
#include "joystick.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function
 */
int main(void)
{
    /* Initialize board hardware. */
    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    BOARD_InitPins();
    BOARD_BootClockPLL220M();
    BOARD_InitDebugConsole();
    GPIO_PortInit(GPIO, 2U);
    GPIO_PortInit(GPIO, 3U);
    /* Enable the power and clock for ADC. */
    BOARD_InitBootPeripherals();
    initializeJoystick();
    initializeADC();
    PRINTF("Configuration Done.\r\n");

	vTaskStartScheduler();

	for (;;)
		__asm("NOP");
}
