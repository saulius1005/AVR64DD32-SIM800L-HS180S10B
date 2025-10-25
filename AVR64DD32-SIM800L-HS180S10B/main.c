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

	intData_to_hexChar(); //convert data which would be sended to google sheets
	generic_animation(sim800l_check, "Checking SIM...", "SIM Prepared!");
	//generic_animation(gnss_check, "Searching GNSS...", "GNSS Locked!");//comment if you don't want to synchronize time. It can took prom couple seconds to half hour. It depends from GNSS signal, sattelites in view and so on
	BK280_Data_Read();
	display_gps_date_and_time();
	SIM800LUARTReceiver();//reading of signal strength
	HTTP_alive_init();
	while (1)
	{	
		BK280_Data_Read();
		display_gps_date_and_time();
		SIM800LUARTReceiver();//reading of signal strength
		SIM800LUARTReceiver2(600, "AT+HTTPPARA=\"URL\",\"%s%s]\"\n", RS485data.url, RS485data.dataBuffer);//send data
		SIM800LUARTReceiver2(10000, "AT+HTTPACTION=0\n"); //receive data (and waiting answer for 10s from server)
		_delay_ms(20000); //30sek delay in total more less to limit data transfers
	}
}
