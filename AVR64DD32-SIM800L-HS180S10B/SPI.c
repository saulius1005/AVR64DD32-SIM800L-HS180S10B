/*
 * SPI.c
 *
 * SPI initialization and control functions for AVR64DD32
 *
 * Created: 2025-01-10
 * Author: Saulius
 */

#include "Settings.h"

/* Initialize SPI0 module as Master with 6 MHz clock */
void SPI0_init() {
    SPI0.CTRLA = SPI_ENABLE_bm       // Enable SPI
               | SPI_MASTER_bm       // Master mode
               | SPI_PRESC_DIV4_gc   // Clock prescaler
               | SPI_CLK2X_bm        // Double speed
               | SPI_MODE_0_gc;      // SPI Mode 0
    SPI0.CTRLB = SPI_SSD_bm;         // Slave select disabled (manual control)
}

/* Pull SS (PA7) low to start SPI communication */
void SPI0_Start() {
    PORTA.OUTCLR = PIN7_bm;
}

/* Pull SS (PA7) high to stop SPI communication */
void SPI0_Stop() {
    PORTA.OUTSET = PIN7_bm;
}

/* Send a byte via SPI0 and receive a byte from the slave */
uint8_t SPI0_Exchange_Data(uint8_t data_storage) {
    SPI0_Start();               // Start communication
    SPI0.DATA = data_storage;   // Send data
    while (!(SPI0.INTFLAGS & SPI_IF_bm)) {} // Wait for completion
    SPI0_Stop();                // End communication
    return SPI0.DATA;           // Return received byte
}

/* Send a single byte via SPI0 without reading */
void SPI0_write(uint8_t data) {
    SPI0.DATA = data;
    while (!(SPI0.INTFLAGS & SPI_IF_bm));
}

/* Send 16-bit word via SPI0 (high byte first) */
void SPI0_write_16(uint16_t data) {
    SPI0_write(data >> 8);       // High byte
    SPI0_write(data & 0xFF);     // Low byte
}
