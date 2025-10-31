/*
 * USART.h
 *
 * Created: 2025-10-14 22:42:45
 *  Author: Saulius
 */ 


#ifndef USART_H_
#define USART_H_

#define URL_LENGTH 128
#define DATA_BUFFER_SIZE 64

typedef enum {
	GNSS_module,
	RS485_module
} RS485orGNSS;

typedef struct { //Testing structure for dummy data storage
	char url[URL_LENGTH];
	char dataBuffer[DATA_BUFFER_SIZE];
	uint16_t azimuth;
	uint16_t elevation;
	uint16_t dayTopElevation;
	uint8_t windSpeed;
	uint8_t winDirection;
	uint8_t firWindSpeed;
	uint8_t firWindDirection;
	uint16_t sunLevel;
	int16_t sht21T;
	uint16_t sht21RH;
	uint16_t bmp280P;
	int16_t bmp280T;
} RS485networkdata;

typedef struct {
	bool lost_connecton_fault;
	bool lost_signal_fault;
	bool data_fault;
} RS485COM;

extern RS485COM RS485;

extern RS485networkdata RS485data;

#endif /* USART_H_ */