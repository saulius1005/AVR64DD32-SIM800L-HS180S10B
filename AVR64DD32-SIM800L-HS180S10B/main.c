/*
 * AVR64DD32-SIM800L-HS180S10B.c
 *
 * Created: 2025-10-12 18:58:30
 * Author : Saulius
 */ 
#include "Settings.h"

int main()
{
	CLOCK_INHF_clock_init();
	GPIO_init();
	SPI0_init();
	USART0_init(GNSS_module);
	USART1_init();
	lcd_init();
	lcd_fill_color(BLACK);
	
	char url[]="http://yoururl.com/index.php?data=[";

	uint16_t azimuth = 23750; //237,50
	uint16_t elevation = 1230; //12,30
	uint16_t dayTopElevation = 5625; //56,25
	uint8_t windSpeed = 13; //13m/s
	uint8_t winDirection = 6; //West
	uint8_t firWindSpeed = 10; //wind speed after filtration
	uint8_t firWindDirection = 10; //wind diretion after filtration
	uint16_t sunLevel = 495; //Average light level in mV
	int16_t sht21T = 2100; //temperature sensor +21,00C
	uint16_t sht21RH = 632; //humidity 63,2%
	uint16_t bmp280P = 1001; //pressure in hPa
	int16_t bmp280T = 2133; //21,33C



	//USART_printf(1, "ATE0&W\r\n");

	generic_animation(sim800l_check, "Checking SIM...", "SIM Prepared!");
	generic_animation(gnss_check, "Searching GNSS...", "GNSS Locked!");

	//HTTP alive setting up
	//SIM800LUARTReceiver2("AT\n",100);
	SIM800LUARTReceiver2(100, "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\n");
	SIM800LUARTReceiver2(300, "AT+SAPBR=3,1,\"APN\",\"internet.tele2.lt\"\n");
	SIM800LUARTReceiver2(1500, "AT+SAPBR=1,1\n"); // aktyvuojam GPRS
	SIM800LUARTReceiver2(180, "AT+HTTPINIT\n"); // inicializuojam HTTP moduli
	SIM800LUARTReceiver2(100, "AT+HTTPPARA=\"CID\",1\n"); // nustatom GPRS kanalà

	while (1)
	{	
		SIM800LUARTReceiver();//reading of signal strength
		SIM800LUARTReceiver2(500, "AT+HTTPPARA=\"URL\",\"%s%04x%04x%04x%02x%x%02x%x%03x%04x%03x%03x%04x]\"\n",
		url,
		azimuth, //237,50
		elevation, //12,30
		dayTopElevation, //56,25
		windSpeed, //13m/s
		winDirection, //West
		firWindSpeed, //wind speed after filtration
		firWindDirection, //wind diretion after filtration
		sunLevel, //Average light level in mV
		sht21T, //temperature sensor +21,00C
		sht21RH, //humidity 63,2%
		bmp280P, //pressure in hPa
		bmp280T //21,33C
		);
		SIM800LUARTReceiver2(10000, "AT+HTTPACTION=0\n");
		_delay_ms(20000); //30sek delay in total more less
	}
}
