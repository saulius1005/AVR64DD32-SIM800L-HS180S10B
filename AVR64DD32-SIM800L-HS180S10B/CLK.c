/*
 * CLK.c
 *
 * Clock configuration functions for the system.
 *
 * Contains functions to initialize and configure the external high-frequency
 * crystal oscillator and internal high-frequency oscillator.
 *
 * Author: Saulius
 * Date: 2024-12-05
 */

#include "Settings.h"

/*
 * Initialize the system to use an external 32 MHz clock.
 */
void CLOCK_XOSCHF_clock_init()
{
    // Enable external 32 MHz clock input
    ccp_write_io((uint8_t *) &CLKCTRL.XOSCHFCTRLA, 
                 CLKCTRL_SELHF_EXTCLOCK_gc | CLKCTRL_FRQRANGE_24M_gc | CLKCTRL_ENABLE_bm);

    // Set the main clock to use the external clock as source
    ccp_write_io((uint8_t *) &CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_EXTCLK_gc);

    // Wait until the main clock has switched to the external oscillator
    while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm)
        {};

    // Clock configuration complete
}

/*
 * Initialize the system to use the internal 24 MHz high-frequency oscillator.
 */
void CLOCK_INHF_clock_init()
{
    // Set internal oscillator frequency to 24 MHz
    ccp_write_io((uint8_t *) &CLKCTRL.OSCHFCTRLA, CLKCTRL_FRQSEL_24M_gc);

    // Optional: configure main clock prescaler (currently not used)
    // ccp_write_io((uint8_t *) &CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm);

    // Set the internal 24 MHz oscillator as the main clock source
    // Optional: output main clock on PA4 (currently commented out)
    ccp_write_io((uint8_t *) &CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSCHF_gc /* | CLKCTRL_CLKOUT_bm */);

    // Wait until the main clock has switched to the internal oscillator
    while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm)
        {};
}
