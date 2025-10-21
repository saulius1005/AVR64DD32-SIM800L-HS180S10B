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
	lcd_fill_color(ORANGE);
	_delay_ms(5000);
	lcd_fill_color(YELLOW);
	_delay_ms(5000);
	lcd_fill_color(GREEN);
	_delay_ms(5000);
	lcd_fill_color(BLACK);

//USART_printf(1, "ATE1&W\r\n"); // atsako ne tik ok bet ir parodo i ka atsakyta
//SIM800LUARTReceiver();
//USART_printf(1, "ATE0&W\r\n"); //rodo tik ok
// SIM800LUARTReceiver();

//HTTP

USART_printf(1, "AT\r\n");
_delay_ms(50);
USART_printf(1, "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n");
_delay_ms(50);
USART_printf(1, "AT+CSTT=\"internet.tele2.lt\",\"wap\",\"wap\"\r\n");
_delay_ms(150);
USART_printf(1, "AT+SAPBR=1,1\r\n");
_delay_ms(750);
USART_printf(1, "AT+HTTPINIT\r\n");
_delay_ms(88);
USART_printf(1, "AT+HTTPPARA=\"CID\",1\r\n");
_delay_ms(50);
USART_printf(1, "AT+HTTPPARA=\"URL\",\"http://yoururl.com/index.php?data=[59880E9510A01240A41B108ED29040108F2]\"\r\n");
_delay_ms(50);
USART_printf(1, "AT+HTTPACTION=0\r\n");
_delay_ms(1333);

//TCP

USART_printf(1, "AT\r\n");
_delay_ms(50);
USART_printf(1, "AT+CIPSHUT\r\n"); // uþdarom visus ankstesnius ryðius
_delay_ms(300);
USART_printf(1, "AT+CIPMUX=0\r\n"); // vienas TCP kanalas
_delay_ms(50);
USART_printf(1, "AT+CSTT=\"internet.tele2.lt\",\"wap\",\"wap\"\r\n"); // APN
_delay_ms(200);
USART_printf(1, "AT+CIICR\r\n"); // paleidþiam GPRS
_delay_ms(3000);
USART_printf(1, "AT+CIFSR\r\n"); // pasitikrinam IP adresà
_delay_ms(200);
USART_printf(1, "AT+CIPSTART=\"TCP\",\"yoururl.com\",80\r\n"); // jungiamës prie serverio
_delay_ms(2000);
USART_printf(1, "AT+CIPSEND\r\n"); // pradësim siøsti duomenis
_delay_ms(100);
// Siunèiam HTTP GET uþklausà (tavo data dalá galima generuoti dinaminiu bûdu)
USART_printf(1,
"GET /index.php?data=[59880E9510A01240A41B108ED29040108F2] HTTP/1.1\r\n"
"Host: yoururl.com\r\n"
"Connection: close\r\n\r\n");
// Baigiam siuntimà su CTRL+Z (0x1A)
USART1_sendChar(0x1A);
_delay_ms(1500); // palaukiam SEND OK / +IPD atsakymo
USART_printf(1, "AT+CIPCLOSE\r\n"); // uþdarom TCP jungtá
_delay_ms(200);
USART_printf(1, "AT+CIPSHUT\r\n"); // iðjungiam GPRS kontekstà
_delay_ms(200);

	while (1)
	{
/*
		USART_printf(1, "ATI\r\n");
		SIM800LUARTReceiver();*/
		
		BK280_Data_Read();
		display_gps_date_and_time(&ALLGNSSDATA);
	}
}
