/*
 * USART.c
 *
 * Created: 2024-12-10 16:21:15
 * Author: Saulius
 *
 * This file contains functions for initializing and using USART communication.
 * It supports two USART channels (USART0 and USART1) for sending and receiving
 * characters, strings, and formatted text messages.
 */

#include "Settings.h"
#include "USARTVar.h"

// Initialize USART0
// Depending on the selected module (RS485 or GNSS), the function sets
// the correct baud rate and routing configuration.
void USART0_init(RS485orGNSS module) {
	PORTMUX.USARTROUTEA = (PORTMUX.USARTROUTEA & ~PORTMUX_USART0_gm) | (module ? PORTMUX_USART0_ALT3_gc : PORTMUX_USART0_ALT2_gc);
	USART0.BAUD = module ? (uint16_t)USART0_BAUD_RATE(115200) : (uint16_t)USART0_BAUD_RATE(38400); // 115.2 kbps for RS485, 38.4 kbps for GNSS
	USART0.CTRLA = module ? USART_RS485_ENABLE_gc : 0;
	USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm | USART_RXMODE_CLK2X_gc; // Enable RX, TX, double-speed mode
	USART0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc; // 8-bit, no parity, 1 stop bit
}

// Read one character from USART0 with a timeout.
// If timeout occurs, RS485.lost_signal_fault is set to true.
char USART0_readChar() {
	USART0.STATUS = USART_RXCIF_bm; // Clear buffer before reading
	uint64_t timeout_counter = 200000;
	while (!(USART0.STATUS & USART_RXCIF_bm)) {
		if (--timeout_counter == 0) {
			RS485.lost_signal_fault = true;
			break;
		}
	}
	return USART0.RXDATAL;
}

// Send a single character through USART0.
void USART0_sendChar(char c) {
	while (!(USART0.STATUS & USART_DREIF_bm)); // Wait until buffer is empty
	USART0.TXDATAL = c;
}

// Send a string through USART0, one character at a time.
void USART0_sendString(char *str) {
	for (size_t i = 0; i < strlen(str); i++) {
		USART0_sendChar(str[i]);
	}
}

// Initialize USART1 with 460800 baud rate and double-speed mode.
void USART1_init() {
	USART1.BAUD = (uint16_t)USART1_BAUD_RATE(460800);
	USART1.CTRLB = USART_RXEN_bm | USART_TXEN_bm | USART_RXMODE_CLK2X_gc; // Enable RX, TX, double-speed
	USART1.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc;
}

// Send one character through USART1.
void USART1_sendChar(char c) {
	while (!(USART1.STATUS & USART_DREIF_bm));
	USART1.TXDATAL = c;
}

// Send a string through USART1, one character at a time.
void USART1_sendString(char *str) {
	for (size_t i = 0; i < strlen(str); i++) {
		USART1_sendChar(str[i]);
	}
}

// Read one character from USART1 with timeout checking.
// If timeout occurs, SIM800L.lost_signal_fault is set to true.
char USART1_readChar() {
	USART1.STATUS = USART_RXCIF_bm; // Clear buffer before reading
	uint32_t timeout_counter = SIM800L_TIMEOUT_COUNTER;
	while (!(USART1.STATUS & USART_RXCIF_bm)) {
		if (--timeout_counter == 0) {
			SIM800L.lost_signal_fault = true;
			break;
		}
	}
	return USART1.RXDATAL;
}

// Read one character from USART1 with RTC overflow as a timeout condition.
// Used when RTC is responsible for timing control.
char USART1_readCharRTC() {
	USART1.STATUS = USART_RXCIF_bm;
	while (!(USART1.STATUS & USART_RXCIF_bm)) {
		if (RTC.INTFLAGS & RTC_OVF_bm) // Timeout detected
			break;
	}
	return USART1.RXDATAL;
}

// Send formatted text through the selected USART channel.
// usart_number = 0 sends via USART0, usart_number = 1 sends via USART1.
void USART_printf(uint8_t usart_number, const char *format, ...) {
	char buffer[128];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	if (usart_number == 0) {
		USART0_sendString(buffer);
	} else if (usart_number == 1) {
		USART1_sendString(buffer);
	}
}

// Convert numerical data into a hexadecimal character buffer for RS485 transmission.
// All values are formatted and stored in RS485data.dataBuffer.
void intData_to_hexChar(){
	sprintf(RS485data.dataBuffer,
	"%04X%04X%04X%02X%X%02X%X%03X%04X%03X%03X%04X",
	RS485data.azimuth,          // 23750 -> 0x5C56
	RS485data.elevation,        // 1230 -> 0x04CE
	RS485data.dayTopElevation,  // 5625 -> 0x15E1
	RS485data.windSpeed,        // 13 -> 0x0D
	RS485data.winDirection,     // 6 -> 0x6
	RS485data.firWindSpeed,     // 10 -> 0x0A
	RS485data.firWindDirection, // 10 -> 0x0A
	RS485data.sunLevel,         // 495 -> 0x1EF
	RS485data.sht21T,           // 2100 -> 0x0834
	RS485data.sht21RH,          // 632 -> 0x278
	RS485data.bmp280P,          // 1001 -> 0x3E9
	RS485data.bmp280T           // 2133 -> 0x0855
	);
}