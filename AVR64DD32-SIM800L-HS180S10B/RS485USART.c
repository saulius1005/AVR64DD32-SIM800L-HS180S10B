/*
 * RS485USART.c
 *
 * Created: 2025-09-05 11:11:30
 *  Author: Saulius
 */ 
#include "Settings.h"
#include "RS485USARTVar.h"


void RS485DataSplitter(char *command) {
/*	const uint8_t lengths[] = {4, 4, 4, 2, 1, 3}; //tokens length without crc
	char temp[MESSAGE_LENGTH_RS485-2]; //data storage

	strncpy(temp, command, MESSAGE_LENGTH_RS485-2); //copy data without crc
	temp[MESSAGE_LENGTH_RS485-2] = '\0';

	uint8_t datatocheck[(MESSAGE_LENGTH_RS485-2)/2]={0};

	for (uint8_t i = 0; i < (MESSAGE_LENGTH_RS485-2)/2; i++) {
		char buf[3] = { temp[i * 2], temp[i * 2 + 1], '\0' }; // du simboliai + '\0'
		datatocheck[i] = (uint8_t)strtol(buf, NULL, 16);
	}
	strncpy(temp, command + (MESSAGE_LENGTH_RS485-2), 2); //copy only crc
	temp[2] = '\0';

	uint8_t crctocheck = (uint8_t)strtol(temp, NULL, 16); //convert char to int

	//if(verify_crc8_cdma2000_v2(datatocheck, crctocheck)){ //if data valid update it
		//screen_write_formatted_text("data is correct", 1, ALIGN_CENTER);//uncomment if nedded// crc ok
		const char *p = command;

		for (uint8_t i = 0; i < 6; i++) {
			char token[5] = {0}; //longest token length + 1

			memcpy(token, p, lengths[i]);
			token[lengths[i]] = '\0';

			switch (i) {
				case 0: RS485.azimuth   = (uint16_t)strtol(token, NULL, 16) / Angle_Precizion; break;
				case 1: RS485.elevation     = (uint16_t)strtol(token, NULL, 16) / Angle_Precizion; break;
				case 2: RS485.topelevation         = (uint16_t)strtol(token, NULL, 16) / Angle_Precizion; break;
				case 3: RS485.windspeed         = (uint8_t)strtol(token, NULL, 16); break;
				case 4: RS485.winddirection       = (uint8_t)strtol(token, NULL, 16); break;
				case 5: RS485.lightlevel	= (uint16_t)strtol(token, NULL, 16); break;
			}

			p += lengths[i];
		}
	//	RS485.WS_data_fault = false; //reset error
		RS485.WS_lost_signal_fault = false; //reset error

	//}
	//else{
		//uncomment if nedded
		//screen_write_formatted_text("data is corupted!", 1, ALIGN_CENTER); // bad crc
	//	RS485.WS_data_fault = true;
	//}
	*/
}

void RS485Receiver() {
	uint8_t index = 0;
	char command[MESSAGE_LENGTH_RS485] = {0}; // Empty command array
	uint8_t start = 0;
	uint8_t timeout = 0;
	RS485.lost_connecton_fault = false;
	RS485.lost_signal_fault = false;

	while (1) {
		char c = USART0_readChar(); // Reading a character from USART	

		if(RS485.lost_signal_fault){
			if (++timeout == CountForError_RS485) { // Timeout condition if usart1 reading is halted
				RS485.lost_connecton_fault = true;
				break;
			}
		}		
		if (start) {
			if (c == '}') { // If received data end symbol
				RS485_Led(RX_LED_OFF);
				start = 0;
				command[index] = '\0';
				index = 0;
				RS485DataSplitter(command); // Execute the received command //comment when testing lines below
				break;
				} else if (index < MESSAGE_LENGTH_RS485) {
				command[index++] = c; // Store received character in command array
			}
		}
		if (c == '{') { // If received data start symbol
			start = 1;
			index = 0;
			RS485_Led(RX_LED_ON);
		}
	}
}