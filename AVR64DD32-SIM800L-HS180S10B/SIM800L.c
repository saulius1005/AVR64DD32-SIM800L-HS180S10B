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
	screen_write_formatted_text(3, ALIGN_CENTER, GREEN, BLACK,"Atsakymai");
	screen_write_formatted_text(5, ALIGN_LEFT, GREEN, BLACK,"%s", command);
}

void SIM800LUARTReceiver() {
	uint8_t index = 0;
	char command[MESSAGE_LENGTH_SIM800L]={0}; // Empty command array
	SIM800L.lost_signal_fault = false;
	while (!SIM800L.lost_signal_fault) {
		char c = USART1_readChar(); // Reading a character from USART
		if(c != '\r') //skip carige return
			command[index++] = c; // Store received character in command array
	}
		SIM800LDataSplitter(command); // Execute the received answer //comment when testing lines below
}

void SIM800LDataSplitter3(char *answer) {

	const char *parts[] = {
		answer
	};

	const uint16_t colors[] = {
		DARK_GREEN,   // "\nANSW: "
		GREEN   // answer
	};

	// 2. Kvieèiame (reikia ásitikinti, kad turite naujà funkcijos pavadinimà)
	screen_write_coloured_text_autoscroll(
	6,              // line (start_line)
	ALIGN_CENTER,     // alignment
	BLACK,          // bg (bendras fonas)
	parts,          // Teksto daliø masyvas
	colors,         // Spalvø masyvas
	1               // Daliø skaièius (4)
	);
	_delay_ms(2000);
}

void SIM800LUARTReceiver3(uint16_t answer_time_ms) {
	RTC_ON(answer_time_ms); //start answer waiting timer
	uint8_t index = 0;
	char answer[MESSAGE_LENGTH_SIM800L] = {0}; // Empty answer array

	while (!(RTC.INTFLAGS & RTC_OVF_bm)) {
		char c = USART1_readCharRTC(); // Read a character

		// Skip only carriage return
		if (c != '\r') {
			if (index < MESSAGE_LENGTH_SIM800L - 1) { // Prevent buffer overflow
				answer[index++] = c;
			}			
		}
		if(c == '\n' && answer[index-2] == '\n')
		index--;
	}
	RTC.INTFLAGS = RTC_OVF_bm; // Clear RTC overflow flag
	RTC_OFF();

	SIM800LDataSplitter3(answer); // Process received answer
	SIM800L.ready = true;
}

void SIM800LDataSplitter2(char *command, char *answer) {
	// 1. Paruoðiame teksto dalis ir spalvas
	char time_str[10]; // "HH:MM:SS" + '\0'

	time_str[0] = (ALLGNSSDATA.hour / 10) + '0';
	time_str[1] = (ALLGNSSDATA.hour % 10) + '0';
	time_str[2] = ':';
	time_str[3] = (ALLGNSSDATA.minute / 10) + '0';
	time_str[4] = (ALLGNSSDATA.minute % 10) + '0';
	time_str[5] = ':';
	time_str[6] = (ALLGNSSDATA.second / 10) + '0';
	time_str[7] = (ALLGNSSDATA.second % 10) + '0';
	time_str[8] = ' ';
	time_str[9] = '\0';

	const char *parts[] = {
		time_str,
		"CMD: ",
		command,
		"ANSW: ",
		answer
	};

	const uint16_t colors[] = {
		DARK_GRAY,
		DARK_GREEN,   // "CMD: "
		GREEN,  // command
		DARK_RED,   // "\nANSW: "
		RED   // answer
	};

	// 2. Kvieèiame (reikia ásitikinti, kad turite naujà funkcijos pavadinimà)
	screen_write_coloured_text_autoscroll(
	3,              // line (start_line)
	ALIGN_LEFT,     // alignment
	BLACK,          // bg (bendras fonas)
	parts,          // Teksto daliø masyvas
	colors,         // Spalvø masyvas
	5               // Daliø skaièius (4)
	);
	//screen_write_formatted_text_autoscroll(3, ALIGN_LEFT, GREEN, BLACK,"CMD: %s\nANSW: %s\n", command, answer);
}

void SIM800LUARTReceiver2(char* command, uint16_t answer_time_ms) {
	BK280_Data_Read(); //read gnss
	display_gps_date_and_time();
	USART_printf(1, command); //send command
	RTC_ON(answer_time_ms); //start answer waiting timer
	uint8_t index = 0;
	char answer[MESSAGE_LENGTH_SIM800L] = {0}; // Empty answer array

	while (!(RTC.INTFLAGS & RTC_OVF_bm)) {
		char c = USART1_readCharRTC(); // Read a character

		// Skip only carriage return
		if (c != '\r' && c!= '\n') {
			if (index < MESSAGE_LENGTH_SIM800L - 1) { // Prevent buffer overflow
				answer[index++] = c;
			}
		}
	}

	RTC.INTFLAGS = RTC_OVF_bm; // Clear RTC overflow flag
	RTC_OFF();

	SIM800LDataSplitter2(command, answer); // Process received answer
}

uint8_t sim800l_check() {
	SIM800LUARTReceiver3(10000);
	if (SIM800L.ready) {
		return 0; // animacija baigta
	}
	return 1; // animacija tæsiama
}