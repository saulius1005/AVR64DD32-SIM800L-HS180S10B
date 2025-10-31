/*
 * AVR64DD32-SIM800L-HS180S10B.c
 *
 * Main program for AVR64DD32 microcontroller with SIM800L, HS180S10B screen and BK280 GNSS module.
 *
 * Created: 2025-10-12 18:58:30
 * Author : Saulius
 */

#include "Settings.h"

int main()
{
    // --- Initialize system peripherals ---
    CLOCK_INHF_clock_init(); // Configure internal 24 MHz oscillator
    GPIO_init();             // Initialize GPIO pins for sensors, USART, SPI, etc.
    SPI0_init();             // Initialize SPI0 interface
    USART0_init(GNSS_module); // Initialize USART0 for GNSS module
    USART1_init();           // Initialize USART1 for RS485/other
    lcd_init();              // Initialize LCD display
    lcd_fill_color(BLACK);   // Clear screen

    // --- Prepare data for sending to Google Sheets ---
    intData_to_hexChar();    // Convert integer data to hex characters

    // --- Run loading animations ---
    generic_animation(sim800l_check, "Checking SIM...", "SIM Prepared!");
    generic_animation(gnss_check, "Searching GNSS...", "GNSS Locked!");
    // Note: GNSS synchronization may take from a few seconds to ~30 minutes depending on signal

    // --- Read initial GNSS data and display it ---
    BK280_Data_Read();
    display_gps_date_and_time();

    // --- Read SIM signal strength ---
    SIM800LUARTReceiver();

    // --- Initialize HTTP communication ---
    HTTP_alive_init();

    while (1)
    {
        // --- Read GNSS data and update display ---
        BK280_Data_Read();
        display_gps_date_and_time();

        // --- Monitor SIM signal strength ---
        SIM800LUARTReceiver();

        // --- Send data via HTTP using SIM800L ---
        SIM800LUARTReceiver2(100, "AT+HTTPPARA=\"URL\",\"%s%s]\"\n", RS485data.url, RS485data.dataBuffer);

        // Uncomment these lines if using SIM800C without a relay server
        // SIM800LUARTReceiver2(100, "AT+HTTPPARA=REDIR,1\n");
        // SIM800LUARTReceiver2(100, "AT+HTTPSSL=1\n");

        // --- Trigger HTTP GET/POST action and wait for server response (timeout 15s) ---
        SIM800LUARTReceiver2(15000, "AT+HTTPACTION=0\n");
    }
}
