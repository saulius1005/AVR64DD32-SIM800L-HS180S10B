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
	
	//USART_printf(1, "ATE0&W\r\n");

	SIM800LUARTReceiver2(1, "", 10000); //15sekundþiu laukia ijungimo þinuèiø
//HTTP alive
SIM800LUARTReceiver2(0, "AT\n",100);
SIM800LUARTReceiver2(0, "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\n", 100);
SIM800LUARTReceiver2(0, "AT+SAPBR=3,1,\"APN\",\"internet.tele2.lt\"\n", 300);
SIM800LUARTReceiver2(0, "AT+SAPBR=1,1\n", 1500); // aktyvuojam GPRS
SIM800LUARTReceiver2(0, "AT+HTTPINIT\n", 180); // inicializuojam HTTP moduli
SIM800LUARTReceiver2(0, "AT+HTTPPARA=\"CID\",1\n", 100); // nustatom GPRS kanalà

	while (1)
	{	
		//lcd_fill_color(BLACK);
		BK280_Data_Read();
		display_gps_date_and_time();
		SIM800LUARTReceiver2(0, "AT+HTTPPARA=\"URL\",\"http://yoururl.com/index.php?data=[59880E9510A01240A41B108ED29040108F2]\"\n", 500);
		SIM800LUARTReceiver2(0, "AT+HTTPACTION=0\n", 10000);

		_delay_ms(24000); //30sek delay in total more less
	}
}
