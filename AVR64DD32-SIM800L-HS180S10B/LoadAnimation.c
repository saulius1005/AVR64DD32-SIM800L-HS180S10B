/*
 * LoadAnimation.c
 *
 * Created: 2025-10-23 20:19:41
 *  Author: Saulius
 */ 
#include "Settings.h"
#include "LoadAnimation.h"

//-------------------------------------------
// GNSS paieðkos animacija (kol nëra signalo)
//-------------------------------------------
void gnss_search_animation() {
	const uint8_t centerX = ST7735_WIDTH / 2;
	const uint8_t centerY = (ST7735_HEIGHT / 2) - 10;
	const uint8_t radius = 30;
	const uint8_t dots = 30;        // taðkø kiekis ratu
	const uint8_t dotSize = 5;
	const uint8_t trail = 10;        // kometos ilgis taðkais

	const float visible_start_angle = 0;
	const float visible_end_angle = M_PI * 1.25f; // pvz. 270°
	const float angle_step = 2.0f * M_PI / dots;
	
	// head KINTAMASIS inicializuojamas ÈIA (funkcijos lygyje)
	// paliekame head = 0, bet reikðmæ nustatysime while cikle

	// Papildoma: Iðvalome ekranà
	lcd_fill_color(COLOR_BG);
	screen_write_formatted_text(16, ALIGN_CENTER, RED, COLOR_BG, "Searching GNSS...");
	uint8_t gnsslocked = 1;
	// Iðorinis ciklas: Sukasi, kol neuþfiksuota
	while (gnsslocked) {
		
		// PAKEITIMAS: head REINICIALIZUOJAMAS KIEKVIENO CIKLO PRADÞIOJE
		// Taip uþtikrinama, kad kiekviena nauja animacija prasideda nuo tos paèios vietos (nematomos zonos)
		uint8_t head = 18;
		
		// 1. GNSS DUOMENØ TIKRINIMAS
		BK280_Data_Read();
		
		if (ALLGNSSDATA.second != 0) {
			gnsslocked = 0;
			break;
		}

		// 2. ANIMACIJOS CIKLAS (i <= dots, kad iðsitrintø paskutinis taðkas)
		for (uint8_t i = 0; i <= dots; i++) {
			
			// PIEÐIMAS (kodas paliekamas be pakeitimø)
			for (uint8_t offset = 0; offset < trail; offset++) {
				uint8_t idx = (head + offset) % dots;
				float angle = idx * angle_step;

				if (angle < visible_start_angle || angle > visible_end_angle) {
					continue;
				}

				uint16_t color;
				if (offset == 0) {
					color = COLOR_BG;
					} else {
					uint8_t fade = 255 * offset / (trail - 1);
					uint8_t r = ((COLOR_FG >> 11) & 0x1F) * fade / 255;
					uint8_t g = ((COLOR_FG >> 5) & 0x3F) * fade / 255;
					uint8_t b = (COLOR_FG & 0x1F) * fade / 255;
					color = (r << 11) | (g << 5) | b;
				}

				int8_t x = centerX + radius * cos(angle);
				int8_t y = centerY + radius * sin(angle);
				st7735_fill_circle(x, y, dotSize, color);
			}
			// Paslenkame animacijos galvà
			head = (head + 1) % dots;
			_delay_ms(50);
		}
	}
		// GNSS uþfiksuotas
	lcd_fill_color(COLOR_BG);
	screen_write_formatted_text(16, ALIGN_CENTER, GREEN, COLOR_BG, "GNSS Locked!");
}

//typedef uint8_t (*AnimationCheckFunc)();

void generic_animation(AnimationCheckFunc check_func, const char* running_text, const char* finished_text) {
	const uint8_t centerX = ST7735_WIDTH / 2;
	const uint8_t centerY = (ST7735_HEIGHT / 2) - 10;
	const uint8_t radius = 30;
	const uint8_t dots = 30;
	const uint8_t dotSize = 5;
	const uint8_t trail = 10;

	const float visible_start_angle = 0;
	const float visible_end_angle = M_PI * 1.25f;
	const float angle_step = 2.0f * M_PI / dots;

	lcd_fill_color(COLOR_BG);
	screen_write_formatted_text(16, ALIGN_CENTER, RED, COLOR_BG, running_text);

	uint8_t head = 18;

	while (1) {
		if (!check_func()) {  // jei callback gràþina 0, baigiame animacijà
			break;
		}

		for (uint8_t i = 0; i <= dots; i++) {
			for (uint8_t offset = 0; offset < trail; offset++) {
				uint8_t idx = (head + offset) % dots;
				float angle = idx * angle_step;

				if (angle < visible_start_angle || angle > visible_end_angle) continue;

				uint16_t color;
				if (offset == 0) {
					color = COLOR_BG;
					} else {
					uint8_t fade = 255 * offset / (trail - 1);
					uint8_t r = ((COLOR_FG >> 11) & 0x1F) * fade / 255;
					uint8_t g = ((COLOR_FG >> 5) & 0x3F) * fade / 255;
					uint8_t b = (COLOR_FG & 0x1F) * fade / 255;
					color = (r << 11) | (g << 5) | b;
				}

				int8_t x = centerX + radius * cos(angle);
				int8_t y = centerY + radius * sin(angle);
				st7735_fill_circle(x, y, dotSize, color);
			}
			head = (head + 1) % dots;
			_delay_ms(50);
		}
	}

	lcd_fill_color(COLOR_BG);
	screen_write_formatted_text(16, ALIGN_CENTER, GREEN, COLOR_BG, finished_text);
	_delay_ms(2000);
}