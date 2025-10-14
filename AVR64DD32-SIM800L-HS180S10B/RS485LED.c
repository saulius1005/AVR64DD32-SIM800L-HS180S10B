/*
 * RS485LED.c
 *
 * Created: 2025-09-04 18:04:28
 *  Author: Saulius
 */ 
#include "Settings.h"

void RS485_Led(RS485_LED_t LED){

    switch (LED) {
	    case RX_LED_ON:
			PORTC.OUTSET = PIN0_bm;
		break;
	    case RX_LED_OFF:
			PORTC.OUTCLR = PIN0_bm;
		break;
	    case TX_LED_ON:
			PORTA.OUTSET = PIN6_bm;
		break;
	    case TX_LED_OFF:
			PORTA.OUTCLR = PIN6_bm;
		break;
    }
}