/**
 * @file GPIO.c
 * @brief GPIO Initialization for various peripherals and sensors
 * 
 * Created: 2024-12-04 16:54:46
 * Author: Saulius
 */

#include "Settings.h"

void GPIO_init(){
    // Configure USART0 and USART1 pin routing
    PORTMUX.USARTROUTEA = PORTMUX_USART1_DEFAULT_gc /*| PORTMUX_USART0_ALT2_gc*/; // Set USART0 to alternative pins set default
	PORTMUX.SPIROUTEA = PORTMUX_SPI0_DEFAULT_gc;
	
	// PA4=MOSI, PA6=SCK, PA7=CS kaip out
	PORTA.DIRSET = PIN2_bm | PIN3_bm | PIN4_bm | PIN6_bm | PIN7_bm;
	//PD2=DC, PD3=RES
	PORTD.DIRSET = PIN2_bm | PIN3_bm; //screen DC and RES

	PORTD.DIRSET = PIN4_bm | PIN7_bm; //USART0 RS485 TX, XDIR 
	PORTD.DIRCLR = PIN5_bm; //USART0 RS485 RX
	PORTD.PIN4CTRL = PORT_PULLUPEN_bm; // Enable pull-up(USART0 RX485 TX)
	PORTD.PIN5CTRL = PORT_PULLUPEN_bm; // Enable pull-up(USART0 RX485 RX)

	PORTA.DIRSET = PIN2_bm; //USART0 GNSS TX
	PORTA.DIRCLR = PIN3_bm; //USART0 GNSS RX
	PORTA.PIN2CTRL = PORT_PULLUPEN_bm; // Enable pull-up(USART0 GNSS TX)
	PORTA.PIN3CTRL = PORT_PULLUPEN_bm; // Enable pull-up(USART0 GNSS RX)


	PORTC.DIRSET = PIN0_bm; //USART1 TX
	PORTC.DIRCLR = PIN1_bm; //USART1 RX
	PORTC.PIN0CTRL = PORT_PULLUPEN_bm; // Enable pull-up(USART1 TX)
	PORTC.PIN1CTRL = PORT_PULLUPEN_bm; // Enable pull-up(USART1 RX)

}
