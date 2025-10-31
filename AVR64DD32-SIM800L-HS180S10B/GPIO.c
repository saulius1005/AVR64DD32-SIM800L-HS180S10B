/*
 * GPIO.c
 *
 * GPIO Initialization for various peripherals and sensors.
 *
 * Created: 2024-12-04 16:54:46
 * Author: Saulius
 */

#include "Settings.h"

/*
 * Initialize GPIO pins for USART, SPI, screen, and other peripherals.
 */
void GPIO_init() {
    // Configure USART0 and USART1 pin routing
    PORTMUX.USARTROUTEA = PORTMUX_USART1_DEFAULT_gc /*| PORTMUX_USART0_ALT2_gc*/; // USART0 default pins, USART1 default
    PORTMUX.SPIROUTEA = PORTMUX_SPI0_DEFAULT_gc; // SPI0 default routing

    // Configure PORTA pins as output: PA4=MOSI, PA6=SCK, PA7=CS
    PORTA.DIRSET = PIN2_bm | PIN3_bm | PIN4_bm | PIN6_bm | PIN7_bm;

    // Configure PORTD pins as output for screen control: PD2=DC, PD3=RES
    PORTD.DIRSET = PIN2_bm | PIN3_bm;

    // Configure additional PORTD pins: PD4=RS485 TX, PD7=XDIR
    PORTD.DIRSET = PIN4_bm | PIN7_bm;

    // Configure PORTD pin PD5 as input (RS485 RX)
    PORTD.DIRCLR = PIN5_bm;

    // Enable pull-up resistors for RS485 TX/RX
    PORTD.PIN4CTRL = PORT_PULLUPEN_bm; // TX
    PORTD.PIN5CTRL = PORT_PULLUPEN_bm; // RX

    // Configure PORTA pins for GNSS USART0: PA2=TX, PA3=RX
    PORTA.DIRSET = PIN2_bm; // TX output
    PORTA.DIRCLR = PIN3_bm; // RX input
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm; // Enable pull-up for TX
    PORTA.PIN3CTRL = PORT_PULLUPEN_bm; // Enable pull-up for RX

    // Configure PORTC pins for USART1: PC0=TX, PC1=RX
    PORTC.DIRSET = PIN0_bm; // TX output
    PORTC.DIRCLR = PIN1_bm; // RX input
    PORTC.PIN0CTRL = PORT_PULLUPEN_bm; // Enable pull-up for TX
    PORTC.PIN1CTRL = PORT_PULLUPEN_bm; // Enable pull-up for RX
}
