/*
 * LoadAnimation.c
 *
 * Loading animation for display.
 *
 * Created: 2025-10-23 20:19:41
 * Author: Saulius
 */

#include "Settings.h"
#include "LoadAnimation.h"

/*
 * Draws a circular loading animation with a fading trail.
 * The animation continues until check_func() returns true.
 *
 * check_func: function pointer to check if the process is done
 * running_text: text displayed while animation runs
 * finished_text: text displayed after animation completes
 */
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

	// Precompute sine and cosine for all dots
	float sin_table[dots], cos_table[dots];
	for (uint8_t i = 0; i < dots; i++) {
		float angle = i * angle_step;
		sin_table[i] = sin(angle);
		cos_table[i] = cos(angle);
	}

	// Clear screen and display running text
	lcd_fill_color(COLOR_BG);
	screen_write_formatted_text(16, ALIGN_CENTER, RED, COLOR_BG, running_text);

	uint8_t head = 0;
	bool check_done = false; // Flag to call check_func only once per rotation

	while (1) {
		// --- Erase the tail of the previous rotation
		uint8_t erase_idx = (head + trail) % dots;
		float erase_angle = erase_idx * angle_step;
		if (erase_angle >= visible_start_angle && erase_angle <= visible_end_angle) {
			int8_t ex = centerX + radius * cos_table[erase_idx];
			int8_t ey = centerY + radius * sin_table[erase_idx];
			st7735_fill_circle(ex, ey, dotSize, COLOR_BG);
		}

		// --- Draw the trail from head to tail
		for (uint8_t t = 0; t < trail; t++) {
			uint8_t idx = (head + t) % dots;
			float angle = idx * angle_step;
			if (angle < visible_start_angle || angle > visible_end_angle) continue;

			uint16_t color;
			if (t == 0) {
				color = COLOR_FG; // Head is brightest
			} else {
				uint8_t fade = 255 * (trail - t) / trail; // Tail fades
				uint8_t r = ((COLOR_FG >> 11) & 0x1F) * fade / 255;
				uint8_t g = ((COLOR_FG >> 5) & 0x3F) * fade / 255;
				uint8_t b = (COLOR_FG & 0x1F) * fade / 255;
				color = (r << 11) | (g << 5) | b;
			}

			int8_t x = centerX + radius * cos_table[idx];
			int8_t y = centerY + radius * sin_table[idx];
			st7735_fill_circle(x, y, dotSize, color);
		}

		// --- Call check_func only when the trail is completely outside the visible area
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
				if (!check_func()) break; // Stop animation if check_func returns true
				check_done = true;
			}
		}

		// --- Rotate counterclockwise
		if (head == 0)
			head = dots - 1;
		else
			head--;

		_delay_ms(delay_ms);

		// --- Reset flag for the next rotation
		if (head == 0) check_done = false;
	}

	// Clear screen and display finished text
	lcd_fill_color(COLOR_BG);
	screen_write_formatted_text(16, ALIGN_CENTER, GREEN, COLOR_BG, finished_text);
	_delay_ms(2000);
}
