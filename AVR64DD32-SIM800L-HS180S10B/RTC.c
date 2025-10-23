/*
 * RTC.c
 *
 * Created: 2025-10-23 13:33:56
 *  Author: Saulius
 */ 
#include "Settings.h"

void RTC_ON(uint16_t period_ms)
{
	while (RTC.STATUS > 0); // sinchronizacijos laukimas
	RTC.CLKSEL = RTC_CLKSEL_OSC1K_gc; //32768/32=1024khz

    // RTC COUNT eina nuo 0 iki PER (kai COUNT == PER -> interrupt)
    RTC.PER = (uint16_t)(period_ms*1.024)+0.5;  

    // Nustatom pradinæ reikðmæ
    RTC.CNT = 0;

    RTC.CTRLA = RTC_RTCEN_bm | RTC_PRESCALER_DIV1_gc;
	//RTC.DBGCTRL |= RTC_DBGRUN_bm;

}

void RTC_OFF()
{
	RTC.CTRLA &= ~RTC_RTCEN_bm;
	// 2?? Laukti, kol modulis sinchronizuosis
	while (RTC.STATUS > 0);

}
