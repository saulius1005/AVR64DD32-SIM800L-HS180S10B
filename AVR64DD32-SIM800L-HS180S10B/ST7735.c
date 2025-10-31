/*
 * ST7735.c
 *
 * Created: 2025-10-12 19:09:08
 * Author: Saulius
 *
 */

#include "Settings.h"
#include "font5x7.h"

// ---- LCD write helpers ----
// Write a command byte (DC low, assert CS around the transfer)
void lcd_write_cmd(uint8_t cmd) {
	DC_LOW();
	CS_LOW();
	SPI0_write(cmd);
	CS_HIGH();
}

// Write a data byte (DC high, assert CS around the transfer)
void lcd_write_data(uint8_t data) {
	DC_HIGH();
	CS_LOW();
	SPI0_write(data);
	CS_HIGH();
}

// Hardware reset sequence: pull reset low, wait, release and wait again
void reset_display() {
	RES_LOW();
	_delay_ms(40);
	RES_HIGH();
	_delay_ms(200);
}

/*
 * Initialize the ST7735 display using a recommended sequence.
 * This sequence configures sleep, frame rate, power, memory access (orientation),
 * pixel format and gamma correction. The values below are common defaults for
 * ST7735S modules; vendor modules may require different values (frame rate,
 * power, gamma, pixel format). If colors, contrast or orientation look wrong,
 * check the following areas: MADCTL (0x36) for rotation and RGB/BGR order,
 * pixel format (0x3A), gamma tables (0xE0/0xE1) and VCOM (0xC5).
 */
void lcd_init(void) {
	// Ensure the controller starts from a known state
	reset_display(); // toggles RES and waits

	// 1) Exit sleep mode
	// Command 0x11 wakes the controller. Wait afterwards for stabilization.
	lcd_write_cmd(0x11);
	_delay_ms(120); // typical wait after sleep OUT

	// 2) Frame rate / display timing setup (commands B1, B2, B3)
	lcd_write_cmd(0xB1); // Frame rate control (normal mode)
	lcd_write_data(0x05);
	lcd_write_data(0x3C);
	lcd_write_data(0x3C);

	lcd_write_cmd(0xB2); // Frame rate control (idle mode)
	lcd_write_data(0x05);
	lcd_write_data(0x3C);
	lcd_write_data(0x3C);

	lcd_write_cmd(0xB3); // Frame rate control (partial mode)
	lcd_write_data(0x05);
	lcd_write_data(0x3C);
	lcd_write_data(0x3C);
	lcd_write_data(0x05);
	lcd_write_data(0x3C);
	lcd_write_data(0x3C);

	// 3) Inversion control
	// 0xB4 sets display inversion scheme; 0x03 is commonly used.
	lcd_write_cmd(0xB4);
	lcd_write_data(0x03);

	// 4) Power control / voltage settings (C0..C5)
	// These parameters control internal voltages. Incorrect values can cause
	// poor contrast, instability or no image.
	lcd_write_cmd(0xC0); // Power control 1
	lcd_write_data(0x28);
	lcd_write_data(0x08);
	lcd_write_data(0x04);

	lcd_write_cmd(0xC1); // Power control 2
	lcd_write_data(0xC0);

	lcd_write_cmd(0xC2); // Power control 3
	lcd_write_data(0x0D);
	lcd_write_data(0x00);

	lcd_write_cmd(0xC3); // Power control 4
	lcd_write_data(0x8D);
	lcd_write_data(0x2A);

	lcd_write_cmd(0xC4); // Power control 5
	lcd_write_data(0x8D);
	lcd_write_data(0xEE);

	lcd_write_cmd(0xC5); // VCOM control (VCOM setting)
	lcd_write_data(0x1A); // adjust if contrast/VCOM artifacts appear

	// 5) Memory access control (orientation and RGB/BGR order)
	// MADCTL (0x36) bits control rotation and RGB/BGR ordering.
	// Common examples:
	//  - 0x00 => no rotation, RGB
	//  - 0x08 => BGR order
	//  - 0x60 => MX + MV (rotate 90)
	//  - 0xA0 => MY + MV (other rotation)
	lcd_write_cmd(0x36);
	lcd_write_data(0xC0); // Normal orientation, RGB ordering

	// 6) Pixel format (0x3A)
	// Typical options: 0x05 = 16-bit RGB565 (most common for ST7735S)
	lcd_write_cmd(0x3A);
	lcd_write_data(0x05);

	// 7) Gamma correction (positive and negative)
	lcd_write_cmd(0xE0); // Positive gamma correction
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
	// Some modules use non-zero memory offsets. If drawing appears shifted,
	// adjust CASET (0x2A) / RASET (0x2B) offsets here.

	// 9) Turn display ON
	lcd_write_cmd(0x29); // Display ON
	_delay_ms(20);

	// 10) Turn backlight on if controlled by MCU (module-specific)
	// If BL pin is driven by MCU, set it here. Otherwise the backlight
	// might be tied to VCC and already on.

	// Ready: controller accepts RAMWR (0x2C) pixel writes.
}


// ---- Helper to set the area for subsequent pixel writes ----
void lcd_set_address(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	lcd_write_cmd(0x2A); // CASET (column address set)
	lcd_write_data(x0 >> 8); lcd_write_data(x0 & 0xFF);
	lcd_write_data(x1 >> 8); lcd_write_data(x1 & 0xFF);

	lcd_write_cmd(0x2B); // RASET (row address set)
	lcd_write_data(y0 >> 8); lcd_write_data(y0 & 0xFF);
	lcd_write_data(y1 >> 8); lcd_write_data(y1 & 0xFF);

	lcd_write_cmd(0x2C); // RAMWR (start memory write)
}

// Fill the whole screen with a 16-bit color value
void lcd_fill_color(uint16_t color) {
	uint32_t pixels = ST7735_WIDTH * ST7735_HEIGHT; // total pixels
	lcd_set_address(0, 0, ST7735_WIDTH -1, ST7735_HEIGHT -1);
	DC_HIGH();
	CS_LOW();
	for (uint32_t i = 0; i < pixels; i++) {
		SPI0_write_16(color);
	}
	CS_HIGH();
}

// Draw a single pixel (bounds-checked)
void st7735_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
	if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;
	lcd_set_address(x, y, x, y);
	DC_HIGH();    // Set data mode
	CS_LOW();     // Assert chip select
	SPI0_write_16(color);
	CS_HIGH();    // Deassert chip select
}

// Filled circle implemented by brute-force pixel checks
void st7735_fill_circle(uint8_t x0, uint8_t y0, uint8_t radius, uint16_t color) {
	for (int16_t y = -radius; y <= radius; y++) {
		for (int16_t x = -radius; x <= radius; x++) {
			if (x * x + y * y <= radius * radius) {
				st7735_draw_pixel(x0 + x, y0 + y, color);
			}
		}
	}
}

// Draw a single character using 5x7 font. Background is drawn as well.
void st7735_draw_char(int x, int y, char c, uint16_t fg, uint16_t bg) {
	if (c < 32 || c > 127) c = '?';
	const uint8_t *chr_data = font5x7[c - 32];
	for (int i = 0; i < 5; i++) { // character width
		uint8_t line = chr_data[i];
		for (int j = 0; j < 7; j++) { // character height
			uint16_t color = (line & 0x01) ? fg : bg;
			st7735_draw_pixel(x + i, y + j, color);
			line >>= 1;
		}
	}
}

// Draw a null-terminated string at (x,y). Characters are spaced by 1 pixel.
void st7735_draw_text(int x, int y, const char *str, uint16_t fg, uint16_t bg) {
	while (*str) {
		st7735_draw_char(x, y, *str++, fg, bg);
		x += 6; // Character width + spacing
	}
}

/*
// Wrapped text function (commented out). Behavior summary:
// - Handles '\r' by returning to line start
// - Handles '\n' by moving to next line
// - Automatically wraps when reaching screen width
// - Stops if vertical space exceeded
*/

// Calculate starting pixel for a text string based on alignment
uint8_t calculate_start_pixel(char *text, alignment_t alignment) {
    uint8_t text_length = 0;
    while (text[text_length] != '\0') {
        text_length++;
    }

    uint8_t text_width = text_length * 6;  // text width in pixels
    switch (alignment) {
        case ALIGN_CENTER:
            return (128 - text_width) / 2;  // center the text
        case ALIGN_RIGHT:
            return (128 - text_width);      // right align
        case ALIGN_LEFT:
        default:
            return 0;  // left align
    }
}

// Write text starting at a pixel position on a specific line. Handles '\n' to move to next line and clears that line with bg color.
void screen_write_text(char *text, uint8_t line, uint8_t start_pixel, uint16_t fg, uint16_t bg) {
	int x = start_pixel;
	int y = line * 8;

	while (*text) {
		if (*text == '\n') {
			y += 8;       // new line
			x = start_pixel;

			// Clear the horizontal line for the new line area
			for (int i = 0; i < 128; i++) {
				for (int j = 0; j < 8; j++) {
					st7735_draw_pixel(i, y + j, bg);
				}
			}
		} else {
			// Pre-fill character area with background
			for (int j = 0; j < 7; j++) {
				for (int i = 0; i < 5; i++) {
					st7735_draw_pixel(x + i, y + j, bg);
				}
			}

			st7735_draw_char(x, y, *text, fg, bg);
			x += 6;
		}
		text++;
	}
}

// Convenience: calculate start pixel from alignment and write text
void screen_write_text_aligned(char *text, uint8_t line, alignment_t alignment, uint16_t fg, uint16_t bg) {
	uint8_t start_pixel = calculate_start_pixel(text, alignment);
	screen_write_text(text, line, start_pixel, fg, bg);
}

// Write wrapped text starting from a given line using alignment. Splits on '\n' and wraps at MAX_TEXT_LENGTH_HORIZONTAL.
void screen_write_text_wrapped(uint8_t start_line, alignment_t alignment, uint16_t fg, uint16_t bg, const char *text) {
	uint8_t line = start_line;
	size_t pos = 0;
	size_t len = strlen(text);

	while (pos < len && line < MAX_TEXT_LENGTH_VERTICAL) {
		// Remaining characters and line length to use
		size_t remaining = len - pos;
		size_t line_len = (remaining > MAX_TEXT_LENGTH_HORIZONTAL) ? MAX_TEXT_LENGTH_HORIZONTAL : remaining;

		// Search for '\n' inside the current line_len
		size_t newline_pos = line_len;
		for (size_t i = 0; i < line_len; i++) {
			if (text[pos + i] == '\n') {
				newline_pos = i;
				break;
			}
		}

		char buffer[MAX_TEXT_LENGTH_HORIZONTAL + 1];
		strncpy(buffer, &text[pos], newline_pos);
		buffer[newline_pos] = '\0';

		// Compute start_pixel based on alignment
		uint8_t start_pixel = calculate_start_pixel(buffer, alignment);

		// Draw characters for this line
		int x = start_pixel;
		int y = line * 8;
		for (size_t i = 0; i < strlen(buffer); i++) {
			st7735_draw_char(x, y, buffer[i], fg, bg);
			x += 6; // 5 px char width + 1 px space
		}

		pos += newline_pos;

		// Skip the '\n' if present
		if (pos < len && text[pos] == '\n') pos++;

		line++;
	}
}

// Formatted print into screen with wrapping. Uses a temporary buffer for vsnprintf.
void screen_write_formatted_text(uint8_t start_line, alignment_t alignment, uint16_t fg, uint16_t bg, const char *format, ...) {
	char textStorage[MAX_TEXT_LENGTH_HORIZONTAL * MAX_TEXT_LENGTH_VERTICAL];  // buffer for formatted text
	va_list args;

	va_start(args, format);
	vsnprintf(textStorage, sizeof(textStorage), format, args);
	va_end(args);

	screen_write_text_wrapped(start_line, alignment, fg, bg, textStorage);
}

// Text buffer for lines from TEXT_BUFFER_START_LINE to MAX_TEXT_LENGTH_VERTICAL
ColouredChar screenBuffer[MAX_TEXT_LENGTH_VERTICAL - TEXT_BUFFER_START_LINE][MAX_TEXT_LENGTH_HORIZONTAL + 1] = {{{0}}};
uint8_t bufferLinesUsed = 0;  // number of lines currently used in buffer

// Redraw the current screen buffer starting at start_line using bg color and alignment
void redraw_screen_buffer(uint8_t start_line, uint16_t bg, alignment_t alignment) {
	for (uint8_t i = 0; i < bufferLinesUsed; i++) {
		uint8_t y = (start_line + i) * 8;
	
		ColouredChar *current_line = screenBuffer[i];
	
		// Convert ColouredChar line to a plain C string to compute alignment
		char temp_text_buffer[MAX_TEXT_LENGTH_HORIZONTAL + 1];
		size_t len = 0;
		for (size_t k = 0; k < MAX_TEXT_LENGTH_HORIZONTAL; k++) {
			if (current_line[k].character == '\0') break;
			temp_text_buffer[k] = current_line[k].character;
			len++;
		}
		temp_text_buffer[len] = '\0';
	
		uint8_t start_pixel = calculate_start_pixel(temp_text_buffer, alignment);
		int x = start_pixel;

		// 1) Clear the entire line area (8 px high) with the background color
		for (int py = 0; py < 8; py++) {
			for (int px = 0; px < ST7735_WIDTH; px++) {
				st7735_draw_pixel(px, y + py, bg);
			}
		}
	
		// 2) Draw the text using the stored foreground colors
		for (size_t j = 0; j < len; j++) {
			uint16_t current_fg = current_line[j].fg_color;
			st7735_draw_char(x, y, current_line[j].character, current_fg, bg);
			x += 6; // 5 px char width + 1 px spacing
		}
	}

	// Clear the remaining portion of the buffer area (if any)
	for (uint8_t i = bufferLinesUsed; i < (MAX_TEXT_LENGTH_VERTICAL - start_line); i++) {
		uint8_t y = (start_line + i) * 8;
		for (int py = 0; py < 8; py++) {
			for (int px = 0; px < ST7735_WIDTH; px++) {
				st7735_draw_pixel(px, y + py, bg);
			}
		}
	}
}

// Append coloured text parts and autoscroll the internal buffer if needed.
void screen_write_coloured_text_autoscroll(uint8_t start_line, alignment_t alignment, uint16_t bg, const char * const *str_parts, const uint16_t *fg_colors, size_t num_parts) {
	
	uint8_t max_buffer_lines = MAX_TEXT_LENGTH_VERTICAL - start_line;
	
	// Temporary line buffer for building a new line
	ColouredChar tempLine[MAX_TEXT_LENGTH_HORIZONTAL + 1] = {0};
	size_t temp_len = 0;

	for (size_t part_idx = 0; part_idx < num_parts; part_idx++) {
		const char *part = str_parts[part_idx];
		uint16_t color = fg_colors[part_idx];
		
		for (size_t char_idx = 0; part[char_idx] != '\0'; char_idx++) {
			char c = part[char_idx];
			
			// 1) Handle newlines or line overflow
			if (c == '\n' || temp_len >= MAX_TEXT_LENGTH_HORIZONTAL) {
				// Terminate temporary line (character-wise)
				tempLine[temp_len].character = '\0';
				
				// 1a) Scrolling: if buffer full, move lines up
				if (bufferLinesUsed >= max_buffer_lines) {
					for (uint8_t i = 1; i < bufferLinesUsed; i++) {
						memcpy(screenBuffer[i - 1], screenBuffer[i], sizeof(screenBuffer[0]));
					}
					// Place new line at the last position
					memcpy(screenBuffer[bufferLinesUsed - 1], tempLine, sizeof(tempLine));
				} else {
					// Insert at next free slot
					memcpy(screenBuffer[bufferLinesUsed], tempLine, sizeof(tempLine));
					bufferLinesUsed++;
				}

				// Reset temporary line
				memset(tempLine, 0, sizeof(tempLine));
				temp_len = 0;
				
				// If newline triggered the flush, skip adding this char
				if (c == '\n') {
					continue;
				}
			}
			
			// 2) Append character to temporary line
			tempLine[temp_len].character = c;
			tempLine[temp_len].fg_color = color;
			temp_len++;
		}
	}

	// 3) If there's a remaining line after all parts, append it
	if (temp_len > 0) {
		tempLine[temp_len].character = '\0';

		if (bufferLinesUsed >= max_buffer_lines) {
			for (uint8_t i = 1; i < bufferLinesUsed; i++) {
				memcpy(screenBuffer[i - 1], screenBuffer[i], sizeof(screenBuffer[0]));
			}
			memcpy(screenBuffer[bufferLinesUsed - 1], tempLine, sizeof(tempLine));
		} else {
			memcpy(screenBuffer[bufferLinesUsed], tempLine, sizeof(tempLine));
			bufferLinesUsed++;
		}
	}
	
	// 4) Redraw the entire buffer to the display
	redraw_screen_buffer(start_line, bg, alignment);
}
