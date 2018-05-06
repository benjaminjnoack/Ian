/*
 * debug.h
 *
 *  Created on: May 6, 2018
 *      Author: ben
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "fsl_gpio.h"
#include "pin_mux.h"

static inline void debugToggle() {
	GPIO_PortToggle(GPIO, BOARD_INITPINS_FC2_SCLX_PORT, 1 << BOARD_INITPINS_FC2_SCLX_PIN);
}

#endif /* DEBUG_H_ */
