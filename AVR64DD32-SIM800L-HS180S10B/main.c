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
USART_printf(1, "ATE0&W\r\n"); //rodo tik ok
// SIM800LUARTReceiver();

//HTTP

/*
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
USART_printf(1, "AT+HTTPPARA=\"URL\",\"http://sunflower.w5.lt/index.php?data=[59880E9510A01240A41B108ED29040108F2]\"\r\n");
_delay_ms(50);
USART_printf(1, "AT+HTTPACTION=0\r\n");
_delay_ms(1333);*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//HTTP alive
USART_printf(1, "AT\r\n");
_delay_ms(50);
USART_printf(1, "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n");
_delay_ms(50);
USART_printf(1, "AT+SAPBR=3,1,\"APN\",\"internet.tele2.lt\"\r\n");
_delay_ms(150);
USART_printf(1, "AT+SAPBR=1,1\r\n"); // aktyvuojam GPRS
_delay_ms(750); // pirmas uþsikûrimas uþtrunka
USART_printf(1, "AT+HTTPINIT\r\n"); // inicializuojam HTTP moduli
_delay_ms(88);
USART_printf(1, "AT+HTTPPARA=\"CID\",1\r\n"); // nustatom GPRS kanalà
_delay_ms(50);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TCP
/*
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
USART_printf(1, "AT+CIPSTART=\"TCP\",\"sunflower.w5.lt\",80\r\n"); // jungiamës prie serverio
_delay_ms(2000);
USART_printf(1, "AT+CIPSEND\r\n"); // pradësim siøsti duomenis
_delay_ms(100);
// Siunèiam HTTP GET uþklausà 
USART_printf(1,
"GET /index.php?data=[59880E9510A01240A41B108ED29040108F2] HTTP/1.1\r\n"
"Host: sunflower.w5.lt\r\n"
"Connection: close\r\n\r\n");
USART_printf(1, "\x1A");
_delay_ms(1500); // palaukiam SEND OK / +IPD atsakymo
USART_printf(1, "AT+CIPCLOSE\r\n"); // uþdarom TCP jungtá
_delay_ms(200);
USART_printf(1, "AT+CIPSHUT\r\n"); // iðjungiam GPRS kontekstà
_delay_ms(200);*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TCP alive
//USART_printf(1, "AT\r\n");
//_delay_ms(100);
//USART_printf(1, "ATE0\r\n"); // iðjungiame echo
//_delay_ms(100);
/*
USART_printf(1, "AT+CIPSHUT\r\n"); // iðjungiam ankstesnius ryðius
_delay_ms(500);
USART_printf(1, "AT+CIPMUX=0\r\n"); // vienas TCP kanalas
_delay_ms(100);
USART_printf(1, "AT+CSTT=\"internet.tele2.lt\",\"wap\",\"wap\"\r\n"); // APN nustatymas
_delay_ms(300);
USART_printf(1, "AT+CIICR\r\n"); // paleidþiam GPRS
_delay_ms(3000);
USART_printf(1, "AT+CIFSR\r\n"); // pasitikrinam IP
_delay_ms(300);
USART_printf(1, "AT+CIPSTART=\"TCP\",\"sunflower.w5.lt\",80\r\n"); // jungiamës prie serverio
_delay_ms(3000); // laukiam CONNECT OK*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	while (1)
	{
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//TCP alive every second part
/*
		USART_printf(1, "AT+CIPSEND\r\n"); // pasiruoðiam siøsti
		_delay_ms(200);

		// Siunèiam HTTP GET uþklausà (kol kas nekintanèià)
		USART_printf(1,
		"GET /index.php?data=[59880E9510A01240A41B108ED29040108F2] HTTP/1.1\r\n"
		"Host: sunflower.w5.lt\r\n"
		"Connection: keep-alive\r\n\r\n");

		// siunèiam Ctrl+Z (pabaiga)
		USART_printf(1, "\x1A");
		_delay_ms(1500); // laukiame SEND OK / +IPD atsakymo*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		////HTTP alive every second part
		// HTTP uþklausa su kintamais duomenimis (èia dar fiksuota)
		USART_printf(1, "AT+HTTPPARA=\"URL\",\"http://sunflower.w5.lt/index.php?data=[59880E9510A01240A41B108ED29040108F2]\"\r\n");
		_delay_ms(50);
		USART_printf(1, "AT+HTTPACTION=0\r\n"); // GET
		_delay_ms(2000); // laukiame atsakymo (HTTPACTION + HTTPREAD)
/*		USART_printf(1, "AT+HTTPREAD\r\n"); // jei nori perskaityti atsakymà
		_delay_ms(300);
		//iðjungimas
		USART_printf(1, "AT+HTTPTERM\r\n"); // uþdarom HTTP servisà
		_delay_ms(200);
		USART_printf(1, "AT+SAPBR=0,1\r\n"); // uþdarom GPRS
		_delay_ms(200);*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
		USART_printf(1, "ATI\r\n");
		SIM800LUARTReceiver();*/
		
		BK280_Data_Read();
		display_gps_date_and_time(&ALLGNSSDATA);
		_delay_ms(25000); //30sek delay
	}
}
