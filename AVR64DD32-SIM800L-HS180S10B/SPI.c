/*
 * @file SPI.c
 * @brief SPI initialization and control functions for AVR64DD32 microcontroller.
 *
 * @details This file contains functions for initializing the SPI0 module, 
 *          starting and stopping communication with the SPI slave device.
 * 
 * @author Saulius
 * @date 2025-01-10
 */

#include "Settings.h"

/**
 * @brief Initializes the SPI0 module for communication.
 * 
 * @details 
 * - Configures SPI0 as a Master with a clock speed of 6 MHz (F_CPU/4).
 * - Enables the SPI0 module.
 */
void SPI0_init() {
	SPI0.CTRLA = SPI_ENABLE_bm        // Enable SPI
	| SPI_MASTER_bm        // Master mode
	| SPI_PRESC_DIV4_gc    // Clock prescaler
	| SPI_CLK2X_bm			// Double speed
	| SPI_MODE_0_gc;       // SPI Mode 0
	SPI0.CTRLB = SPI_SSD_bm;          // Slave select disabled (manual control)
}

/**
 * @brief Starts SPI communication by pulling the SS pin low.
 * 
 * @details 
 * - Pulls the Slave Select (SS) line low to indicate the start of communication 
 *   with the SPI slave device.
 */
void SPI0_Start() {
    PORTA.OUTCLR = PIN7_bm; // Set SS (PA7) low
}

/**
 * @brief Stops SPI communication by pulling the SS pin high.
 * 
 * @details 
 * - Pulls the Slave Select (SS) line high to indicate the end of communication 
 *   with the SPI slave device.
 */
void SPI0_Stop() {
    PORTA.OUTSET = PIN7_bm; // Set SS (PA7) high
}

/**
 * @brief Exchanges a byte of data via SPI0.
 *
 * This function transmits a single byte of data to an SPI slave device and 
 * simultaneously receives a byte of data from the slave device.
 * It ensures the SPI communication is correctly initiated and terminated by 
 * managing the slave select (SS) line.
 *
 * @param data_storage The byte of data to send to the SPI slave.
 * @return The byte of data received from the SPI slave.
 */
uint8_t SPI0_Exchange_Data(uint8_t data_storage) {
    SPI0_Start(); // Pull SS low to initiate communication
    SPI0.DATA = data_storage; // Send the data
    while (!(SPI0.INTFLAGS & SPI_IF_bm)) {} // Wait until data is exchanged
    SPI0_Stop(); // Pull SS high to terminate communication
    return SPI0.DATA; // Return the received data
}

void SPI0_write(uint8_t data) {
	SPI0.DATA = data;
	while (!(SPI0.INTFLAGS & SPI_IF_bm));
}

 /**
 * @brief Sends a 16-bit word via SPI to the TFT display.
 *
 * Sends the high byte first, followed by the low byte, using `spi_tft_write()`.
 *
 * @param data 16-bit data to send.
 */
void SPI0_write_16(uint16_t data) {
	 SPI0_write(data >> 8);
	 SPI0_write(data & 0xFF);
 }
