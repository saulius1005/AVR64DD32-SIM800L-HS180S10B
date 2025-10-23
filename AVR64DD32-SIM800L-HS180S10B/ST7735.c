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

void st7735_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
	if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;
	lcd_set_address(x, y, x, y);
	DC_HIGH();    // Ájungiam „data“ reþimà
	CS_LOW();     // Aktyvuojam LCD
	SPI0_write_16(color);
	CS_HIGH();    // Iðjungiam LCD
}

void st7735_fill_circle(uint8_t x0, uint8_t y0, uint8_t radius, uint16_t color) {
	for (int16_t y = -radius; y <= radius; y++) {
		for (int16_t x = -radius; x <= radius; x++) {
			if (x * x + y * y <= radius * radius) {
				st7735_draw_pixel(x0 + x, y0 + y, color);
			}
		}
	}
}


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


void st7735_draw_text(int x, int y, const char *str, uint16_t fg, uint16_t bg) {
	while (*str) {
		st7735_draw_char(x, y, *str++, fg, bg);
		x += 6; // Character width + spacing
	}
}

/*
void st7735_draw_text_wrap(int x, int y, const char *str, uint16_t fg, uint16_t bg) {
	int start_x = x;                        // Pradinë eilutës x reikðmë
	const int char_width = 6;               // 5 px simbolis + 1 px tarpas
	const int line_height = 8;              // 7 px aukðtis + 1 px tarpas

	while (*str) {
		if (*str == '\r') {
			// Carriage return – gráþtam á eilutës pradþià
			x = start_x;
			} else if (*str == '\n') {
			// Newline – pereinam á naujà eilutæ
			x = start_x;
			y += line_height;
			} else {
			// Automatinis lauþymas, jei pasiekiam ekrano kraðtà
			if (x + char_width > ST7735_WIDTH) {
				x = start_x;
				y += line_height;
			}

			// Patikrinam, ar dar telpam á ekranà vertikaliai
			if (y + line_height > ST7735_HEIGHT) {
				// Nebetelpam – baigiam spausdinimà
				break;
			}

			st7735_draw_char(x, y, *str, fg, bg);
			x += char_width;
		}

		str++;
	}
}*/

uint8_t calculate_start_pixel(char *text, alignment_t alignment) {
    uint8_t text_length = 0;
    while (text[text_length] != '\0') {
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

void screen_write_text(char *text, uint8_t line, uint8_t start_pixel, uint16_t fg, uint16_t bg) {
	int x = start_pixel;
	int y = line * 8;

	while (*text) {
		if (*text == '\n') {
			y += 8;       // nauja eilutë
			x = start_pixel;

			// Nuvalome horizontalià linijà naujai eilutei
			for (int i = 0; i < 128; i++) {
				for (int j = 0; j < 8; j++) {
					st7735_draw_pixel(i, y + j, bg);
				}
			}
			} else {
			// Pirmiausia uþpildome simbolio plotà fono spalva
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

void screen_write_text_aligned(char *text, uint8_t line, alignment_t alignment, uint16_t fg, uint16_t bg) {
	uint8_t start_pixel = calculate_start_pixel(text, alignment);  ///< Calculate start pixel
	screen_write_text(text, line, start_pixel, fg, bg);             ///< Draw text with colors
}

void screen_write_text_wrapped(uint8_t start_line, alignment_t alignment, uint16_t fg, uint16_t bg, const char *text) {
	uint8_t line = start_line;
	size_t pos = 0;
	size_t len = strlen(text);

	while (pos < len && line < MAX_TEXT_LENGTH_VERTICAL) {
		// Suskaièiuojame eilutës ilgá
		size_t remaining = len - pos;
		size_t line_len = (remaining > MAX_TEXT_LENGTH_HORIZONTAL) ? MAX_TEXT_LENGTH_HORIZONTAL : remaining;

		// Ieðkome \n
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

		// Apskaièiuojame start_pixel pagal lygiavimà
		uint8_t start_pixel = calculate_start_pixel(buffer, alignment);

		// Pieðiame simbolius eilutëje, st7735_draw_char jau pieðia fono spalvà
		int x = start_pixel;
		int y = line * 8;
		for (size_t i = 0; i < strlen(buffer); i++) {
			st7735_draw_char(x, y, buffer[i], fg, bg);
			x += 6; // 5 px simbolio plotis + 1 px tarpas
		}

		pos += newline_pos;

		// Praleidþiame \n, jei buvo
		if (pos < len && text[pos] == '\n') pos++;

		line++;
	}
}


void screen_write_formatted_text(uint8_t start_line, alignment_t alignment, uint16_t fg, uint16_t bg, const char *format, ...) {
	char textStorage[MAX_TEXT_LENGTH_HORIZONTAL * MAX_TEXT_LENGTH_VERTICAL];  ///< Buffer for storing formatted text
	va_list args;

	va_start(args, format);                                        ///< Start reading variable arguments
	vsnprintf(textStorage, sizeof(textStorage), format, args);    ///< Format the text
	va_end(args);                                                  ///< End reading variable arguments

	screen_write_text_wrapped(start_line, alignment, fg, bg, textStorage);  ///< Write wrapped/cut text
}

/// Teksto bufferis ekrano linijoms nuo TEXT_BUFFER_START_LINE iki MAX_TEXT_LENGTH_VERTICAL
ColouredChar screenBuffer[MAX_TEXT_LENGTH_VERTICAL - TEXT_BUFFER_START_LINE][MAX_TEXT_LENGTH_HORIZONTAL + 1] = {{{0}}};
uint8_t bufferLinesUsed = 0;  // Kiek eiluèiø ðiuo metu uþimta bufferio

void redraw_screen_buffer(uint8_t start_line, uint16_t bg, alignment_t alignment) {
	for (uint8_t i = 0; i < bufferLinesUsed; i++) {
		uint8_t y = (start_line + i) * 8;
	
		ColouredChar *current_line = screenBuffer[i];
	
		// Konvertuojame ColouredChar eilutæ á char* tekstà, kad apskaièiuotume lygiavimà
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

		// 1. **Visos linijos valymas (8 px aukðèio)** su bendra BG spalva
		for (int py = 0; py < 8; py++) {
			for (int px = 0; px < ST7735_WIDTH; px++) {
				st7735_draw_pixel(px, y + py, bg);
			}
		}
	
		// 2. TEKSTO PIEÐIMAS (NAUDOJANT JÛSØ LOGIKÀ IÐ screen_write_text_wrapped)
		for (size_t j = 0; j < len; j++) {
			uint16_t current_fg = current_line[j].fg_color;
		
			// Jûsø patikrinta pieðimo funkcija:
			st7735_draw_char(x, y, current_line[j].character, current_fg, bg);
			x += 6; // 5 px simbolio plotis + 1 px tarpas
		}
	}
	// Valymas likusiai bufferio daliai
	for (uint8_t i = bufferLinesUsed; i < (MAX_TEXT_LENGTH_VERTICAL - start_line); i++) {
		uint8_t y = (start_line + i) * 8;
		for (int py = 0; py < 8; py++) {
			for (int px = 0; px < ST7735_WIDTH; px++) {
				st7735_draw_pixel(px, y + py, bg);
			}
		}
	}
}


void screen_write_coloured_text_autoscroll(uint8_t start_line, alignment_t alignment, uint16_t bg, const char * const *str_parts, const uint16_t *fg_colors, size_t num_parts) {
	
	uint8_t max_buffer_lines = MAX_TEXT_LENGTH_VERTICAL - start_line;
	
	// Laikinas buferio masyvas naujai informacijai
	ColouredChar tempLine[MAX_TEXT_LENGTH_HORIZONTAL + 1] = {0};
	size_t temp_len = 0;

	for (size_t part_idx = 0; part_idx < num_parts; part_idx++) {
		const char *part = str_parts[part_idx];
		uint16_t color = fg_colors[part_idx];
		
		for (size_t char_idx = 0; part[char_idx] != '\0'; char_idx++) {
			char c = part[char_idx];
			
			// 1. Naujos eilutës apdorojimas (PAGRINDINIS PATAISYMAS)
			if (c == '\n' || temp_len >= MAX_TEXT_LENGTH_HORIZONTAL) {
				// Eilutë baigësi (dël \n arba per didelio ilgio)
				
				// Prieð ádedant á buferá, uþtikriname, kad tempLine bûtø null-terminuotas (simbolio atþvilgiu)
				tempLine[temp_len].character = '\0';
				
				// 1a. Scrolinimo logika
				if (bufferLinesUsed >= max_buffer_lines) {
					// Stumiame senas eilutes aukðtyn
					for (uint8_t i = 1; i < bufferLinesUsed; i++) {
						// Naudojame atminties kopijavimà, nes elementas yra struktûra
						memcpy(screenBuffer[i - 1], screenBuffer[i], sizeof(screenBuffer[0]));
					}
					// Ádedam naujà eilutæ á paskutinæ vietà
					memcpy(screenBuffer[bufferLinesUsed - 1], tempLine, sizeof(tempLine));
					} else {
					// Ádedam á laisvà vietà
					memcpy(screenBuffer[bufferLinesUsed], tempLine, sizeof(tempLine));
					bufferLinesUsed++;
				}

				// 1b. Atstatome laikinà eilutæ naujai pradþiai
				memset(tempLine, 0, sizeof(tempLine));
				temp_len = 0;
				
				// Jei nauja eilutë prasidëjo dël \n, praleidþiame kità apdorojimà ðiam simboliui
				if (c == '\n') {
					continue;
				}
			}
			
			// 2. Simbolio pridëjimas prie laikinosios eilutës
			tempLine[temp_len].character = c;
			tempLine[temp_len].fg_color = color;
			temp_len++;
		}
	}

	// 3. Apdorojame paskutinæ likusià eilutæ (jei ji nebuvo baigta su \n)
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
	
	// 4. Perpieðiame visà bufferá
	redraw_screen_buffer(start_line, bg, alignment);
}


