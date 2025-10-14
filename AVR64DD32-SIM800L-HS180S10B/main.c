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
	//_delay_ms(2000);
	//USART_printf(1, "AT+IPR=460800\r\n");
	//USART_printf(1, "AT\r\n");

	//USART1_sendString("AT+IPR?\r\n");
//	SIM800LUARTReceiver();
/*
 	lcd_fill_color(GREEN);
 	_delay_ms(2000);*/

// 	st7735_draw_text(10, 33,"TESTAS", YELLOW, BLACK);
	while (1)
	{	
		USART_printf(1, "AT\r\n");
		SIM800LUARTReceiver();
		_delay_ms(500);
		BK280_Data_Read();
		display_gps_date_and_time(&ALLGNSSDATA);
	}
}
