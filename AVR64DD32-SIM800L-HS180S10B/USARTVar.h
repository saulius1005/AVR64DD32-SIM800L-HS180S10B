/*
 * USARTVar.h
 *
 * Created: 2025-10-25 16:36:04
 *  Author: Saulius
 */ 


#ifndef USARTVAR_H_
#define USARTVAR_H_


RS485networkdata RS485data = { //dummy data initial values
	.url ="http://http://yoururl.com/index.php?data=[",
	.dataBuffer = {0},
	.azimuth = 23750, //237,50
	.elevation = 1230, //12,30
	.dayTopElevation = 5625, //56,25
	.windSpeed = 13, //13m/s
	.winDirection = 6, //West
	.firWindSpeed = 10, //wind speed after filtration
	.firWindDirection = 10, //wind diretion after filtration
	.sunLevel = 495, //Average light level in mV
	.sht21T = 2100, //temperature sensor +21,00C
	.sht21RH = 632, //humidity 63,2%
	.bmp280P = 1001, //pressure in hPa
	.bmp280T = 2133 //21,33C
};

RS485COM RS485 = {
	.lost_connecton_fault = false,
	.lost_signal_fault = false,
	.data_fault = false
};

#endif /* USARTVAR_H_ */