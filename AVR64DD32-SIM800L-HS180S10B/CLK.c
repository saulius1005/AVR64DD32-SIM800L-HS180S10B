/**
 * @file CLK.c
 * @brief Clock configuration functions for the system.
 *
 * This file contains functions to initialize and configure the external high-frequency
 * crystal oscillator and external clock input for the system.
 * 
 * @author Saulius
 * @date 2024-12-05
 */

#include "Settings.h"

void CLOCK_XOSCHF_clock_init()
{
	/* Enable external (32 MHz) clock input */
	ccp_write_io((uint8_t *) &CLKCTRL.XOSCHFCTRLA, CLKCTRL_SELHF_EXTCLOCK_gc | CLKCTRL_FRQRANGE_24M_gc | CLKCTRL_ENABLE_bm);

	/* Set the main clock to use external clock as source */
	ccp_write_io((uint8_t *) &CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_EXTCLK_gc);

	/* Wait for system oscillator change to complete */
	while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm)
	{};
	/* Configuration complete;*/
}

void CLOCK_INHF_clock_init() {
	ccp_write_io((uint8_t *) &CLKCTRL.OSCHFCTRLA, CLKCTRL_FRQSEL_24M_gc); ///< Set oscillator frequency to 24 MHz.
	// ccp_write_io((uint8_t *) &CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm); ///< Enable clock prescaler if needed (currently not used).
	ccp_write_io((uint8_t *) &CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSCHF_gc/* | CLKCTRL_CLKOUT_bm*/); ///< Set the internal 24 MHz oscillator as the main clock source, with optional clock output on PA4.
	while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm) {}; ///< Wait for oscillator switch to be completed.
}