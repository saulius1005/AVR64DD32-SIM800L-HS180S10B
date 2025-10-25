/*
 * BK_280.c
 *
 * Created: 2025-07-12 09:47:59
 *  Author: Saulius
 */ 
#include "Settings.h"
#include "BK_280Var.h"

/**
 * @brief Splits a comma-separated line into individual fields.
 *
 * This function modifies the input string by replacing commas with null terminators.
 *
 * @param line       Input string containing comma-separated values (modified in place).
 * @param fields     Array of pointers to store extracted fields.
 * @param max_fields Maximum number of fields to extract.
 *
 * @return Number of fields successfully extracted.
 */
int split_fields(char *line, char *fields[], int max_fields) {
	 int count = 0;
	 char *ptr = line;
	 char *start = ptr;
	 while (*ptr && count < max_fields) {
		 if (*ptr == ',') {
			 *ptr = '\0';
			 fields[count++] = start;
			 start = ptr + 1;
		 }
		 ptr++;
	 }
	 if (count < max_fields && start < ptr) {
		 fields[count++] = start;
	 }
	 return count;
 }


void parse_GNRMC() {

	char *fields[15] = {0};
	int count = split_fields(buffer, fields, 15);
	if (count < 10) return;

	// Parse time hhmmss
	if (fields[1] && strlen(fields[1]) >= 6) {
		ALLGNSSDATA.hour   = (fields[1][0] - '0') * 10 + (fields[1][1] - '0');
		ALLGNSSDATA.minute = (fields[1][2] - '0') * 10 + (fields[1][3] - '0');
		ALLGNSSDATA.second = (fields[1][4] - '0') * 10 + (fields[1][5] - '0');
		} else {
		ALLGNSSDATA.hour = ALLGNSSDATA.minute = ALLGNSSDATA.second = 0;
	}

	// Parse date ddmmyy
	if (fields[9] && strlen(fields[9]) == 6) {
		ALLGNSSDATA.day   = (fields[9][0] - '0') * 10 + (fields[9][1] - '0');
		ALLGNSSDATA.month = (fields[9][2] - '0') * 10 + (fields[9][3] - '0');
		ALLGNSSDATA.year  = (fields[9][4] - '0') * 10 + (fields[9][5] - '0');
		} else {
		ALLGNSSDATA.day = ALLGNSSDATA.month = ALLGNSSDATA.year = 0;
	}
}


void display_gps_date_and_time() {
	screen_write_formatted_text(0, ALIGN_LEFT, WHITE, BLACK,
	"Data: %04u-%02u-%02u",
	((uint16_t)ALLGNSSDATA.year) + 2000,
	ALLGNSSDATA.month,
	ALLGNSSDATA.day);

}

void BK280_Data_Read() {
	uint16_t index = 0;
	bool capturing = false;
	static const char target[] = "$GNRMC";
	uint8_t match_index = 0;

	while (1) {
		char c = USART0_readChar();

		if (!capturing) {
			if (c == target[match_index]) {
				match_index++;
				if (match_index == sizeof(target) - 1) {
					capturing = true;
					index = 0;
					memcpy(buffer, target, sizeof(target) - 1);
					index = sizeof(target) - 1;
					match_index = 0;
				}
				} else {
				match_index = 0;
			}
			continue;
		}

		// Jeigu jau fiksuojam eilutæ — dedam á buferá
		if (index < NMEA_BUFFER_SIZE - 1)
		buffer[index++] = c;

		// NMEA eilutë baigiasi \r\n
		if (c == '\n' && index >= 2 && buffer[index - 2] == '\r') {
			buffer[index] = '\0';
			parse_GNRMC();
			apply_timezone();//apply gmt
			break;
		}

		// Apsauga, jei buferis perpildomas
		if (index >= NMEA_BUFFER_SIZE - 1) {
			buffer[NMEA_BUFFER_SIZE - 1] = '\0';
			break;
		}
	}
}

uint8_t gnss_check() {
	BK280_Data_Read(); //read gnss
	if (ALLGNSSDATA.second != 0) {
		return 0; // animacija baigta
	}
	return 1; // animacija tæsiama
}

// ar metai keliamieji
static inline bool is_leap(uint8_t year) {
	uint16_t full_year = 2000 + year;
	return (full_year % 4 == 0 && (full_year % 100 != 0 || full_year % 400 == 0));
}

// kiek dienø turi mënuo
static uint8_t days_in_month(uint8_t month, uint8_t year) {
	switch (month) {
		case 1: case 3: case 5: case 7: case 8: case 10: case 12: return 31;
		case 4: case 6: case 9: case 11: return 30;
		case 2: return is_leap(year) ? 29 : 28;
		default: return 30;
	}
}

// ar ðiuo metu taikomas vasaros laikas (ES: paskutinis kovo – paskutinis spalio sekmadienis)
static bool is_summer_time(uint8_t day, uint8_t month, uint8_t year) {
	if (month < 3 || month > 10) return false;
	if (month > 3 && month < 10) return true;

	// Zeller algoritmas (supaprastintas, be papildomø tipø)
	uint8_t q = day;
	uint8_t m = (month < 3) ? month + 12 : month;
	uint16_t Y = (month < 3) ? (2000 + year - 1) : (2000 + year);
	uint8_t h = (q + (13 * (m + 1)) / 5 + Y + (Y / 4) - (Y / 100) + (Y / 400)) % 7;
	uint8_t weekday = (h + 6) % 7; // 0=sekmadienis, 1=pirmadienis...

	uint8_t lastSunday = 31 - ((weekday + (31 - day)) % 7);

	if (month == 3)
	return (day > lastSunday);  // po paskutinio kovo sekmadienio – vasaros laikas
	else
	return !(day > lastSunday); // iki paskutinio spalio sekmadienio – vasaros laikas
}

// pagrindinë funkcija
void apply_timezone() {
	int8_t offset = GMT;

	// jei vasaros laikas – pridëti papildomà 1 valandà
	if (is_summer_time(ALLGNSSDATA.day, ALLGNSSDATA.month, ALLGNSSDATA.year))
	offset += 1;

	int16_t hour = ALLGNSSDATA.hour + offset;

	// valandø korekcija su dienos/mënesio/metø perëjimais
	if (hour >= 24) {
		hour -= 24;
		ALLGNSSDATA.day++;
		if (ALLGNSSDATA.day > days_in_month(ALLGNSSDATA.month, ALLGNSSDATA.year)) {
			ALLGNSSDATA.day = 1;
			ALLGNSSDATA.month++;
			if (ALLGNSSDATA.month > 12) {
				ALLGNSSDATA.month = 1;
				ALLGNSSDATA.year++;
			}
		}
		} else if (hour < 0) {
		hour += 24;
		if (ALLGNSSDATA.day == 1) {
			if (ALLGNSSDATA.month == 1) {
				ALLGNSSDATA.month = 12;
				ALLGNSSDATA.year--;
				} else {
				ALLGNSSDATA.month--;
			}
			ALLGNSSDATA.day = days_in_month(ALLGNSSDATA.month, ALLGNSSDATA.year);
			} else {
			ALLGNSSDATA.day--;
		}
	}

	ALLGNSSDATA.hour = (uint8_t)hour;
}
