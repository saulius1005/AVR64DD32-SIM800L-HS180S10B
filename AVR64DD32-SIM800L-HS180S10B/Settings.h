/*
 * Settings.h
 *
 * Created: 2024-12-04 17:00:49
 * Author: Saulius
 * 
 * @brief This header file contains function prototypes, macros, and includes necessary for configuring hardware,
 *        managing communication protocols (I2C, USART), and interacting with various sensors and peripherals.
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

/**
 * @brief CPU clock frequency (F_CPU).
 * 
 * This macro defines the system clock frequency used for various calculations, including baud rate configuration.
 */
#define F_CPU 24000000

/**
 * @brief Macro for calculating the USART0 baud rate.
 * 
 * This macro calculates the baud rate for USART0 based on the desired baud rate and system clock (F_CPU).
 * 
 * @param BAUD_RATE Desired baud rate.
 * @return Calculated baud rate setting for USART0.
 */
#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (8 *(float)BAUD_RATE)) + 0.5)

/**
 * @brief Macro for calculating the USART1 baud rate.
 * 
 * This macro calculates the baud rate for USART1 based on the desired baud rate and system clock (F_CPU).
 * 
 * @param BAUD_RATE Desired baud rate.
 * @return Calculated baud rate setting for USART1.
 */
#define USART1_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (8 *(float)BAUD_RATE)) + 0.5)

#include <avr/io.h>      /**< Include AVR I/O library for register definitions and hardware control */
#include <avr/cpufunc.h>
#include <util/delay.h>
#include <avr/interrupt.h> /**< Include AVR interrupt library for ISR (Interrupt Service Routine) support */
#include <stdio.h>       /**< Include standard I/O library for functions like printf */
#include <string.h>      /**< Include string library for handling string functions like strlen */
#include <stdlib.h>
#include <math.h>        /**< Include math library for mathematical functions (e.g., sin, cos) */
#include <float.h>       /**< Include float.h for floating point constants like FLT_MAX */
#include <stdbool.h>     /**< Include stdbool.h for boolean type support (true/false) */
#include <avr/pgmspace.h>
#include "RS485LED.h"
#include "RS485USART.h"
#include "ST7735.h"
#include "SIM800L.h"
#include "USART.h"
#include "BK_280.h"

////////////////////////////////////////////////////////////////////////////////
// Function Prototypes
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Initializes the General Purpose I/O (GPIO) pins for necessary tasks.
 * 
 * This function configures the necessary GPIO pins for communication, control, 
 * or other I/O purposes. The exact configuration will depend on the application.
 */
void GPIO_init();

/**
 * @brief Initializes the external oscillator clock.
 * 
 * Configures the external oscillator for system clock usage.
 */
void CLOCK_XOSCHF_clock_init();

/**
 * @brief Initializes the external crystal oscillator.
 * 
 * Configures the external crystal for system clock usage.
 */
void CLOCK_XOSCHF_crystal_init();

void CLOCK_INHF_clock_init();


/**
 * @brief Initializes USART0 for serial communication.
 * 
 * Configures the USART0 for transmitting and receiving data.
 */
void USART0_init(RS485orGNSS module);

/**
 * @brief Sends a character over USART0.
 * 
 * @param c Character to send.
 */
void USART0_sendChar(char c);

/**
 * @brief Sends a string over USART0.
 * 
 * @param str String to send.
 */
void USART0_sendString(char *str);

/**
 * @brief Reads a character from USART0.
 * 
 * @return Character received.
 */
char USART0_readChar();

/**
 * @brief Sends formatted data over USART.
 * 
 * @param usart_number USART port (0 or 1).
 * @param format Format string for the data.
 */

void USART1_init();

char USART1_readChar();

void USART1_sendString(char *str);

void USART_printf(uint8_t usart_number, const char *format, ...);

void RS485_Led(RS485_LED_t LED);

void FOReceiver();

void RS485Receiver();

void SPI0_init();

void SPI0_write(uint8_t data);

void SPI0_write_16(uint16_t data);

void lcd_init();

void lcd_fill_color(uint16_t color);

void st7735_draw_text(int x, int y, const char *str, uint16_t fg, uint16_t bg);

void SIM800LUARTReceiver();

void display_gps_date_and_time(AllGPSData *data);

void BK280_Data_Read();

#endif /* SETTINGS_H_ */