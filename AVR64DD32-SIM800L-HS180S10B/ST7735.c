/*
 * ST7735.c
 *
 * Created: 2025-10-12 19:09:08
 *  Author: Saulius
 */ 
#include "Settings.h"
#include "ST7735Var.h"
#include "font5x7.h"

// ---- LCD write helpers ----
void lcd_write_cmd(uint8_t cmd) {
	DC_LOW();
	CS_LOW();
	SPI0_write(cmd);
	CS_HIGH();
}

void lcd_write_data(uint8_t data) {
	DC_HIGH();
	CS_LOW();
	SPI0_write(data);
	CS_HIGH();
}

void reset_display() {
	RES_LOW();
	_delay_ms(40);
	RES_HIGH();
	_delay_ms(200);
}

/**
 * @brief Initialize the ST7735 display with recommended sequence and detailed comments.
 *
 * This init is based on the ST7735S sequences (frame rate / power / pixel format / gamma).
 * Keep the lcd_write_cmd() / lcd_write_data() helpers (they must set DC/CS correctly).
 *
 * Notes:
 *  - Many parameters (frame rate, power sequence, gamma) are vendor/variant dependent.
 *    If image looks strange (contrast, colors, orientation) tweak the related bytes below.
 *  - Pixel format: 0x03 = 12-bit, 0x04 = 16-bit? (vendor variants), 0x05 = 16-bit (65k), 0x06 = 18-bit.
 *    For ST7735S modules 0x05 is standard for RGB565.
 *  - MADCTL (0x36) controls rotation/flipping and RGB/BGR order. See comment near MADCTL usage.
 */
void lcd_init(void) {
	// Hardware reset: ensure controller starts from known state
	reset_display(); // toggles RES and waits (your reset_display already does delays)

	// 1) Exit sleep mode
	// Command 0x11 (Sleep OUT) wakes the display from sleep.
	// After this command the controller needs time to stabilize before further commands.
	lcd_write_cmd(0x11);
	_delay_ms(120); // required wait after sleep out (typical 120ms)

	// 2) Frame rate / display timing setup (B1, B2, B3)
	// These affect the PLL/frame frequency and image flicker/contrast.
	// Values here are common defaults from ST7735S appendix — vendor may tweak.
	lcd_write_cmd(0xB1); // Frame rate control (in normal mode)
	// B1: [osc], [rt], [f] (vendor semantics vary) — typical sequence uses {0x05,0x3C,0x3C}
	lcd_write_data(0x05);
	lcd_write_data(0x3C);
	lcd_write_data(0x3C);

	lcd_write_cmd(0xB2); // Frame rate control (in idle mode)
	lcd_write_data(0x05);
	lcd_write_data(0x3C);
	lcd_write_data(0x3C);

	lcd_write_cmd(0xB3); // Frame rate control (partial mode, different sequence)
	// For some controllers B3 has 6 parameters (two triplets for different clocks)
	lcd_write_data(0x05);
	lcd_write_data(0x3C);
	lcd_write_data(0x3C);
	lcd_write_data(0x05);
	lcd_write_data(0x3C);
	lcd_write_data(0x3C);

	// 3) Inversion control
	// 0xB4 configures display inversion (dot inversion, line inversion, etc.)
	// 0x03 is a commonly used value; other values change inversion scheme.
	lcd_write_cmd(0xB4);
	lcd_write_data(0x03);

	// 4) Power control / voltage settings (C0..C5)
	// These commands set internal voltages (VCI, VGH, VGL, VCOM) used by the panel.
	// Wrong values => poor contrast, unstable image, or no image.
	lcd_write_cmd(0xC0); // Power control 1
	lcd_write_data(0x28);
	lcd_write_data(0x08);
	lcd_write_data(0x04);

	lcd_write_cmd(0xC1); // Power control 2
	lcd_write_data(0xC0); // often a single parameter

	lcd_write_cmd(0xC2); // Power control 3 (sometimes used for injection)
	lcd_write_data(0x0D);
	lcd_write_data(0x00);

	lcd_write_cmd(0xC3); // Power control 4 (VGH, VGL adjustments)
	lcd_write_data(0x8D);
	lcd_write_data(0x2A);

	lcd_write_cmd(0xC4); // Power control 5
	lcd_write_data(0x8D);
	lcd_write_data(0xEE);

	lcd_write_cmd(0xC5); // VCOM control (VCOM setting)
	lcd_write_data(0x1A); // tweak this if contrast/VCOM artifacts appear

	// 5) Memory access control (orientation + RGB/BGR)
	// Command 0x36 (MADCTL) bits control orientation:
	//  - MY (bit7): Row address order (vertical flip)
	//  - MX (bit6): Column address order (horizontal flip)
	//  - MV (bit5): Row/column exchange (swap X/Y)
	//  - ML (bit4): Vertical refresh order (usually 0)
	//  - RGB (bit3): 0 = RGB order, 1 = BGR order
	//  - MH (bit2): Horizontal refresh order (rarely used)
	//
	// Common values:
	//  - 0x00 => no rotation, RGB
	//  - 0x08 => BGR order (if your colors are swapped, set this)
	//  - 0x60 => MX+MV (rotate 90)
	//  - 0xA0 => MY+MV (rotate other directions)
	//
	// Choose based on how your module is mounted. If colors look swapped R<->B, flip the RGB bit.
	lcd_write_cmd(0x36);
	lcd_write_data(0xC0); // Normal orientation, RGB ordering
	// If you prefer the same mapping as some libraries use, try 0xC0 or 0xA0 etc.
	// Example: lcd_write_data(0xC0); // flip X and use BGR

	// 6) Pixel format
	// 0x3A selects pixel format (bits-per-pixel). Typical choices:
	//  - 0x03 = 12-bit/pixel (R4G4B4)    (rare)
	//  - 0x04 = 16-bit/pixel (sometimes vendor-specific)
	//  - 0x05 = 16-bit/pixel (RGB565)   <- most common for ST7735S
	//  - 0x06 = 18-bit/pixel (6-6-6)
	//
	// Use 0x05 for 16-bit RGB565 (your write_data16 expects this).
	lcd_write_cmd(0x3A);
	lcd_write_data(0x05);

	// 7) Gamma correction (E0/E1)
	// Gamma curves affect color balance and contrast. The sequences are long arrays.
	// If colors/contrast look off, try alternate gamma tables from datasheet.
	// Here we use a common pair (you can paste the full arrays from the datasheet / appendix).
	lcd_write_cmd(0xE0); // Positive gamma correction
	// Example small set (datasheet usually gives 16 bytes). Replace with full table if needed:
	lcd_write_data(0x04);
	lcd_write_data(0x22);
	lcd_write_data(0x07);
	lcd_write_data(0x0A);
	lcd_write_data(0x2E);
	lcd_write_data(0x30);
	lcd_write_data(0x25);
	lcd_write_data(0x2A);
	lcd_write_data(0x28);
	lcd_write_data(0x26);
	lcd_write_data(0x2E);
	lcd_write_data(0x3A);
	lcd_write_data(0x00);
	lcd_write_data(0x01);
	lcd_write_data(0x03);
	lcd_write_data(0x13);

	lcd_write_cmd(0xE1); // Negative gamma correction
	lcd_write_data(0x04);
	lcd_write_data(0x16);
	lcd_write_data(0x06);
	lcd_write_data(0x0D);
	lcd_write_data(0x2D);
	lcd_write_data(0x26);
	lcd_write_data(0x23);
	lcd_write_data(0x27);
	lcd_write_data(0x27);
	lcd_write_data(0x25);
	lcd_write_data(0x2D);
	lcd_write_data(0x3B);
	lcd_write_data(0x00);
	lcd_write_data(0x01);
	lcd_write_data(0x04);
	lcd_write_data(0x13);

	// 8) Column / row address set defaults (optional)
	// Some modules need COLUMN/ROW offset correction because module's visible area
	// doesn't start at 0,0 in panel memory (common offsets: 0 or a few pixels).
	// If your drawing is shifted, adjust offsets here (0x2A and 0x2B).
	// Typical for 128x160 there's no offset, so we skip a forced CASET/RASET here.

	// 9) Finally, turn display ON
	lcd_write_cmd(0x29); // Display ON
	_delay_ms(20);

	// 10) Turn backlight on (module-specific)
	// If BL pin is controlled by MCU, set it HIGH here. If backlight is tied to VCC,
	// it is already on. Many modules expose BL as an LED anode that must be driven through a resistor.
	// Example (if BL_PIN is defined and handled in Settings.h):
	// PORTA.OUTSET = TFT_BL_PIN;
	// or call a helper: tft_backlight_on();

	// READY: The display should now accept RAMWR (0x2C) pixel data writes.
	// If image looks wrong: check MADCTL (0x36) for orientation/BGR bit, pixel format (0x3A),
	// gamma tables (E0/E1) and VCOM (C5).
}



// ---- Helper spalvai uþpildyti visà ekranà ----
void lcd_set_address(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	lcd_write_cmd(0x2A); // CASET
	lcd_write_data(x0 >> 8); lcd_write_data(x0 & 0xFF);
	lcd_write_data(x1 >> 8); lcd_write_data(x1 & 0xFF);

	lcd_write_cmd(0x2B); // RASET
	lcd_write_data(y0 >> 8); lcd_write_data(y0 & 0xFF);
	lcd_write_data(y1 >> 8); lcd_write_data(y1 & 0xFF);

	lcd_write_cmd(0x2C); // RAMWR
}


void lcd_fill_color(uint16_t color) {
	uint32_t pixels = ST7735_WIDTH * ST7735_HEIGHT; // viso pikseliø
	lcd_set_address(0, 0, ST7735_WIDTH -1, ST7735_HEIGHT -1);
	DC_HIGH();
	CS_LOW();
	for (uint32_t i = 0; i < pixels; i++) {
		SPI0_write_16(color);
	}
	CS_HIGH();
}

/**
 * @brief Draw a single pixel on the screen.
 * 
 * @param x X coordinate (0..ST7735_WIDTH-1).
 * @param y Y coordinate (0..ST7735_HEIGHT-1).
 * @param color 16-bit RGB565 color.
 */
void st7735_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
	if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;
	lcd_set_address(x, y, x, y);
	DC_HIGH();    // Ájungiam „data“ reþimà
	CS_LOW();     // Aktyvuojam LCD
	SPI0_write_16(color);
	CS_HIGH();    // Iðjungiam LCD
}

/**
 * @brief Draw a single character from 5x7 font.
 * 
 * @param x Top-left X coordinate.
 * @param y Top-left Y coordinate.
 * @param c Character to draw.
 * @param fg Foreground color (RGB565).
 * @param bg Background color (RGB565).
 */

void st7735_draw_char(int x, int y, char c, uint16_t fg, uint16_t bg) {
	if (c < 32 || c > 127) c = '?';
	const uint8_t *chr_data = font5x7[c - 32];
	for (int i = 0; i < 5; i++) { // plotis
		uint8_t line = chr_data[i];
		for (int j = 0; j < 7; j++) { // aukðtis
			uint16_t color = (line & 0x01) ? fg : bg;
			st7735_draw_pixel(x + i, y + j, color);
			line >>= 1;
		}
	}
}

/**
 * @brief Draw a text string using 5x7 font.
 * 
 * @param x Starting X coordinate.
 * @param y Starting Y coordinate.
 * @param str Null-terminated string to draw.
 * @param fg Foreground color (RGB565).
 * @param bg Background color (RGB565).
 */
void st7735_draw_text(int x, int y, const char *str, uint16_t fg, uint16_t bg) {
	while (*str) {
		st7735_draw_char(x, y, *str++, fg, bg);
		x += 6; // Character width + spacing
	}
}

/**
 * @brief Calculates the starting pixel based on text alignment.
 * 
 * This function calculates the starting pixel for the text based on the chosen 
 * alignment (left, center, right).
 * 
 * @param text A pointer to the text string.
 * @param max_length The maximum number of characters.
 * @param alignment The desired text alignment (left, center, right).
 * 
 * @return The starting pixel for the text.
 */
uint8_t calculate_start_pixel(char *text, /*uint8_t max_length,*/ alignment_t alignment) {
    uint8_t text_length = 0;
    while (text[text_length] != '\0' /*&& text_length < max_length*/) {
        text_length++;
    }

    uint8_t text_width = text_length * 6;  ///< Calculate the width of the text in pixels
    switch (alignment) {
        case ALIGN_CENTER:
            return (128 - text_width) / 2;  ///< Center the text
        case ALIGN_RIGHT:
            return (128 - text_width);  ///< Right-align the text
        case ALIGN_LEFT:
        default:
            return 0;  ///< Left-align the text
    }
}

/**
 * @brief Writes a string of text with alignment on the ST7567S display.
 * 
 * This function writes the text to a specific line, adjusting for alignment (left, 
 * center, or right).
 * 
 * @param text A pointer to the text string to write.
 * @param line The line (page) where the text will be written.
 * @param alignment The desired text alignment (left, center, right).
 */
void screen_write_text_aligned(char *text, uint8_t line, alignment_t alignment) {
    uint8_t start_pixel = calculate_start_pixel(text, alignment);  ///< Calculate start pixel
   //screen_write_text(text, line, start_pixel);
}

/**
 * @brief Writes formatted text with alignment to the ST7567S display.
 * 
 * This function formats the text using the specified format string and arguments 
 * and then writes the formatted text to the display with the chosen alignment.
 * 
 * @param format The format string for the text.
 * @param line The line (page) where the text will be written.
 * @param alignment The desired text alignment (left, center, right).
 */
void screen_write_formatted_text(const char *format, uint8_t line, alignment_t alignment, ...) {
    char textStorage[MAX_TEXT_LENGTH];  ///< Buffer for storing formatted text
    va_list args;  ///< Variable argument list

    va_start(args, alignment);  ///< Start reading variable arguments
    vsnprintf(textStorage, MAX_TEXT_LENGTH, format, args);  ///< Format the text
    va_end(args);  ///< End reading variable arguments

    screen_write_text_aligned(textStorage, line, alignment);  ///< Write formatted text to display
}