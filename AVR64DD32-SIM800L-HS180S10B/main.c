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
	lcd_fill_color(RED);
	_delay_ms(5000);
	lcd_fill_color(YELLOW);
	_delay_ms(5000);
	lcd_fill_color(GREEN);
	_delay_ms(5000);
	lcd_fill_color(BLACK);

//USART_printf(1, "ATE1&W\r\n"); // atsako ne tik ok bet ir parodo i ka atsakyta
//SIM800LUARTReceiver();
// USART_printf(1, "ATE0&W\r\n"); //rodo tik ok
// SIM800LUARTReceiver();



USART_printf(1, "AT\r\n");

_delay_ms(2000);

// USART_printf(1, "AT+CSQ\r\n");
// 
// _delay_ms(2000);
// 
// USART_printf(1, "AT+CREG?\r\n");
// 
// _delay_ms(2000);
//
//USART_printf(1, "AT+SAPBR=0,1\r\n");   // iðjungiam, jei aktyvus
//
//_delay_ms(2000);

USART_printf(1, "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n");

_delay_ms(50);

//USART_printf(1, "AT+SAPBR=3,1,\"APN\",\"internet.tele2.lt\"\r\n");
USART_printf(1, "AT+CSTT=\"internet.tele2.lt\",\"wap\",\"wap\"\r\n");

_delay_ms(50);

USART_printf(1, "AT+SAPBR=1,1\r\n");

_delay_ms(50);

/*
USART_printf(1, "AT+SAPBR=2,1\r\n");

_delay_ms(2000);*/

/*
USART_printf(1, "AT+HTTPTERM\r\n"); // iðvalyti ankstesnius seansus
_delay_ms(2000);*/

USART_printf(1, "AT+HTTPINIT\r\n");

_delay_ms(50);

USART_printf(1, "AT+HTTPPARA=\"CID\",1\r\n");

_delay_ms(50);

USART_printf(1, "AT+HTTPPARA=\"URL\",\"http://yoururl.com//index.php?data=[59880E9510A01240A41B108ED29040108F2]\"\r\n");

_delay_ms(50);

USART_printf(1, "AT+HTTPACTION=0\r\n");
_delay_ms(50);

/*
_delay_ms(5000);
_delay_ms(5000);
// èia turi laukti, kol per UART ateis +HTTPACTION: 0,200,xx ið moduli?
// SIM800LUARTReceiver() turëtø laukti bent 5–10 sek. arba kol aptinka "HTTPACTION"

USART_printf(1, "AT+HTTPREAD\r\n");
SIM800LUARTReceiver();

USART_printf(1, "AT+HTTPTERM\r\n");
SIM800LUARTReceiver();*/

/*
USART_printf(1, "AT+SAPBR=0,1\r\n");   // iðjungiam, jei aktyvus
SIM800LUARTReceiver();
USART_printf(1, "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n");
SIM800LUARTReceiver();
USART_printf(1, "AT+SAPBR=3,1,\"APN\",\"internet.tele2.lt\"\r\n");
SIM800LUARTReceiver();
USART_printf(1, "AT+SAPBR=1,1\r\n");   // vël ájungiam
SIM800LUARTReceiver();
USART_printf(1, "AT+SAPBR=2,1\r\n");
SIM800LUARTReceiver();*/
	while (1)
	{
/*
		USART_printf(1, "ATI\r\n");
		SIM800LUARTReceiver();*/
		
		BK280_Data_Read();
		display_gps_date_and_time(&ALLGNSSDATA);
	}
}
