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

//gnss_search_animation();
//SIM800LUARTReceiver3(10000); //15sekundþiu laukia ijungimo þinuèiø
generic_animation(sim800l_check, "Checking SIM...", "SIM Prepared!");
generic_animation(gnss_check, "Searching GNSS...", "GNSS Locked!");


//HTTP alive
SIM800LUARTReceiver2("AT\n",100);
SIM800LUARTReceiver2("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\n", 100);
SIM800LUARTReceiver2("AT+SAPBR=3,1,\"APN\",\"internet.tele2.lt\"\n", 300);
SIM800LUARTReceiver2("AT+SAPBR=1,1\n", 1500); // aktyvuojam GPRS
SIM800LUARTReceiver2("AT+HTTPINIT\n", 180); // inicializuojam HTTP moduli
SIM800LUARTReceiver2( "AT+HTTPPARA=\"CID\",1\n", 100); // nustatom GPRS kanalà

	while (1)
	{	
		SIM800LUARTReceiver2("AT+HTTPPARA=\"URL\",\"http://yoururl.com/index.php?data=[59880E9510A01240A41B108ED29040108F2]\"\n", 500);
		SIM800LUARTReceiver2("AT+HTTPACTION=0\n", 10000);

		_delay_ms(24000); //30sek delay in total more less
	}
}
