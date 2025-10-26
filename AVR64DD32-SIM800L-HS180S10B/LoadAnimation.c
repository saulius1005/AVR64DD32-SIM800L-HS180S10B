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


void generic_animation(AnimationCheckFunc check_func, const char* running_text, const char* finished_text) {
	const uint8_t centerX = ST7735_WIDTH / 2;
	const uint8_t centerY = (ST7735_HEIGHT / 2) - 10;
	const uint8_t radius = 25;
	const uint8_t dots = 60;
	const uint8_t dotSize = 4;
	const uint8_t trail = 20;
	const uint8_t delay_ms = 45;

	const float visible_start_angle = 0.0f;
	const float visible_end_angle = M_PI * 1.25f;
	const float angle_step = 2.0f * M_PI / dots;

	// Sin/Cos iðankstinis skaièiavimas
	float sin_table[dots], cos_table[dots];
	for (uint8_t i = 0; i < dots; i++) {
		float angle = i * angle_step;
		sin_table[i] = sin(angle);
		cos_table[i] = cos(angle);
	}

	lcd_fill_color(COLOR_BG);
	screen_write_formatted_text(16, ALIGN_CENTER, RED, COLOR_BG, running_text);

	uint8_t head = 0;
	bool check_done = false; // flag, kad check_func kvieèiamas tik vienà kartà per ratà

	while (1) {
		// --- Iðtriname paskutiná taðkà (uodegos galà)
		uint8_t erase_idx = (head + trail) % dots;
		float erase_angle = erase_idx * angle_step;
		if (erase_angle >= visible_start_angle && erase_angle <= visible_end_angle) {
			int8_t ex = centerX + radius * cos_table[erase_idx];
			int8_t ey = centerY + radius * sin_table[erase_idx];
			st7735_fill_circle(ex, ey, dotSize, COLOR_BG);
		}

		// --- Nupieðiame trail nuo galvos link uodegos
		for (uint8_t t = 0; t < trail; t++) {
			uint8_t idx = (head + t) % dots;
			float angle = idx * angle_step;
			if (angle < visible_start_angle || angle > visible_end_angle) continue;

			uint16_t color;
			if (t == 0) {
				color = COLOR_FG; // galva ryðkiausia
			} else {
				uint8_t fade = 255 * (trail - t) / trail; // uodega silpnëja
				uint8_t r = ((COLOR_FG >> 11) & 0x1F) * fade / 255;
				uint8_t g = ((COLOR_FG >> 5) & 0x3F) * fade / 255;
				uint8_t b = (COLOR_FG & 0x1F) * fade / 255;
				color = (r << 11) | (g << 5) | b;
			}

			int8_t x = centerX + radius * cos_table[idx];
			int8_t y = centerY + radius * sin_table[idx];
			st7735_fill_circle(x, y, dotSize, color);
		}

		// --- Tikriname check_func tik tada, kai visi trail taðkai uþ matomo lanko
		if (!check_done) {
			bool all_hidden = true;
			for (uint8_t t = 0; t < trail; t++) {
				uint8_t idx = (head + t) % dots;
				float angle = idx * angle_step;
				if (angle >= visible_start_angle && angle <= visible_end_angle) {
					all_hidden = false;
					break;
				}
			}
			if (all_hidden) {
				if (!check_func()) break;
				check_done = true;
			}
		}

		// --- Sukame prieðinga kryptimi
		if (head == 0)
			head = dots - 1;
		else
			head--;

		_delay_ms(delay_ms);

		// --- Reset flag naujam ratui
		if (head == 0) check_done = false;
	}

	lcd_fill_color(COLOR_BG);
	screen_write_formatted_text(16, ALIGN_CENTER, GREEN, COLOR_BG, finished_text);
	_delay_ms(2000);
}

