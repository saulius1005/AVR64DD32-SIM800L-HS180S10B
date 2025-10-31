/*
 * Settings.h
 *
 * Created: 2024-12-04 17:00:49
 * Author: Saulius
 *
 * This header file defines system-wide settings, constants, and function prototypes
 * used across different modules of the program.
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

// System clock frequency in Hz (24 MHz)
#define F_CPU 24000000

// Calculate USART0 baud rate based on CPU frequency and desired baud rate
#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (8 * (float)BAUD_RATE)) + 0.5)

// Calculate USART1 baud rate based on CPU frequency and desired baud rate
#define USART1_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (8 * (float)BAUD_RATE)) + 0.5)

// Include standard AVR and C libraries
#include <avr/io.h>
#include <avr/cpufunc.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>
#include <avr/pgmspace.h>

// Include custom project headers
#include "ST7735.h"
#include "SIM800L.h"
#include "USART.h"
#include "BK_280.h"

////////////////////////////////////////////////////////////////////////////////
// Function Prototypes
////////////////////////////////////////////////////////////////////////////////

// Initialize all necessary GPIO pins
void GPIO_init();

// Initialize the external oscillator clock
void CLOCK_XOSCHF_clock_init();

// Initialize the external crystal oscillator
void CLOCK_XOSCHF_crystal_init();

// Initialize the internal high-frequency clock
void CLOCK_INHF_clock_init();

// Initialize USART0 for serial communication
void USART0_init(RS485orGNSS module);

// Send a single character through USART0
void USART0_sendChar(char c);

// Send a full string through USART0
void USART0_sendString(char *str);

// Read a single character from USART0
char USART0_readChar();

// Initialize USART1 for serial communication
void USART1_init();

// Read a single character from USART1
char USART1_readChar();

// Read a character from USART1 using RTC as timeout control
char USART1_readCharRTC();

// Send a full string through USART1
void USART1_sendString(char *str);

// Send formatted text through the selected USART (0 or 1)
void USART_printf(uint8_t usart_number, const char *format, ...);

// Initialize SPI0 interface
void SPI0_init();

// Send one byte through SPI0
void SPI0_write(uint8_t data);

// Send 16 bits through SPI0
void SPI0_write_16(uint16_t data);

// Initialize LCD
void lcd_init();

// Fill LCD screen with a single color
void lcd_fill_color(uint16_t color);

// Draw one pixel at given coordinates
void st7735_draw_pixel(uint16_t x, uint16_t y, uint16_t color);

// Draw a filled circle at given coordinates
void st7735_fill_circle(uint8_t x0, uint8_t y0, uint8_t radius, uint16_t color);

// Draw text string on LCD
void st7735_draw_text(int x, int y, const char *str, uint16_t fg, uint16_t bg);

// Draw text that automatically wraps to the next line
void st7735_draw_text_wrap(int x, int y, const char *str, uint16_t fg, uint16_t bg);

// Write text to a specific line and position on screen
void screen_write_text(char *text, uint8_t line, uint8_t start_pixel, uint16_t fg, uint16_t bg);

// Write long text with automatic line wrapping and alignment
void screen_write_text_wrapped(uint8_t start_line, alignment_t alignment, uint16_t fg, uint16_t bg, const char *text);

// Write text aligned (left, center, right) on a given line
void screen_write_text_aligned(char *text, uint8_t line, alignment_t alignment, uint16_t fg, uint16_t bg);

// Write formatted text (like printf) with alignment and color
void screen_write_formatted_text(uint8_t line, alignment_t alignment, uint16_t fg, uint16_t bg, const char *format, ...);

// Display multi-colored text parts with auto-scrolling
void screen_write_coloured_text_autoscroll(uint8_t line, alignment_t alignment, uint16_t bg, const char * const *str_parts, const uint16_t *fg_colors, size_t num_parts);

// Enable Real-Time Counter (RTC) with given period in milliseconds
void RTC_ON(uint16_t period_ms);

// Disable RTC
void RTC_OFF();

// Handle data received from SIM800L UART
void SIM800LUARTReceiver();

// Handle SIM800L UART data with timeout and formatted output
void SIM800LUARTReceiver2(uint16_t answer_time_ms, const char *format, ...);

// Handle SIM800L UART data with timeout only
void SIM800LUARTReceiver3(uint16_t answer_time_ms);

// Start SIM800L HTTP keep-alive connection
void HTTP_alive_init();

// Display GPS date and time on LCD
void display_gps_date_and_time();

// Read pressure and temperature data from BK280 sensor
void BK280_Data_Read();

// Calculate the starting X pixel position based on text alignment
uint8_t calculate_start_pixel(char *text, alignment_t alignment);

// Function pointer type for animation status checking
typedef uint8_t (*AnimationCheckFunc)();

// Generic animation function used for progress or transition effects
void generic_animation(AnimationCheckFunc check_func, const char* running_text, const char* finished_text);

// Check if GNSS module is connected and responding
uint8_t gnss_check();

// Check if SIM800L module is connected and responding
uint8_t sim800l_check();

// Apply current timezone settings to system time
void apply_timezone();

// Convert integer sensor data to hexadecimal characters for transmission
void intData_to_hexChar();

#endif /* SETTINGS_H_ */
