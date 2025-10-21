/*
 * SIM800L.c
 *
 * Created: 2025-10-12 21:57:03
 *  Author: Saulius
 */ 
#include "Settings.h"
#include "SIM800LVar.h"

uint64_t hexToUint64(const char *str) {
	uint64_t result = 0;
	char c;

	while ((c = *str++)) {
		result <<= 4; // kiekvienas hex simbolis = 4 bitai

		if (c >= '0' && c <= '9') {
			result |= (uint64_t)(c - '0');
			} else if (c >= 'A' && c <= 'F') {
			result |= (uint64_t)(c - 'A' + 10);
			} else if (c >= 'a' && c <= 'f') {
			result |= (uint64_t)(c - 'a' + 10);
			} else {
			// netinkamas simbolis
			return 0;
		}
	}
	return result;
}

void SIM800LDataSplitter(char *command) {
	//st7735_draw_text_wrap(0, 52, command, BLUE, BLACK);
	_delay_ms(2000);
}

void SIM800LUARTReceiver() {
	uint8_t index = 0;
	char command[MESSAGE_LENGTH_SIM800L]={0}; // Empty command array
	SIM800L.lost_signal_fault = false;
	while (!SIM800L.lost_signal_fault) {
		char c = USART1_readChar(); // Reading a character from USART
		command[index++] = c; // Store received character in command array
	}
// 		command[index] = '\0';
// 		index = 0;
		SIM800LDataSplitter(command); // Execute the received command //comment when testing lines below

}