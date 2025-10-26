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
	screen_write_formatted_text(0, ALIGN_CENTER, WHITE, BLACK,
	"%04u-%02u-%02u",
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

void apply_timezone(void) {
	int16_t year = 2000 + ALLGNSSDATA.year;
	int16_t hour = ALLGNSSDATA.hour;

	bool isDST = false;
	if (ALLGNSSDATA.month > 3 && ALLGNSSDATA.month < 10) isDST = true;//taikomas vasaros laikas (ES taisyklës)
	else if (ALLGNSSDATA.month == 3 || ALLGNSSDATA.month == 10) {
		uint8_t h = (31 + (13 * (ALLGNSSDATA.month + 1)) / 5 + year + (year / 4) - (year / 100) + (year / 400)) % 7;
		uint8_t lastSunday = 31 - ((h + 6) % 7);
		isDST = (ALLGNSSDATA.month == 3) ? (ALLGNSSDATA.day >= lastSunday) : (ALLGNSSDATA.day < lastSunday);
	}
	int8_t offset = GMT - (isDST ? 0 : 1); // þiemà -1 valanda
	hour += offset;	
	bool leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));//keliamieji metai

	uint8_t dim = (ALLGNSSDATA.month==4||ALLGNSSDATA.month==6||ALLGNSSDATA.month==9||ALLGNSSDATA.month==11)?30:(ALLGNSSDATA.month==2?(leap?29:28):31);//dienø mënesyje

	if (hour >= 24) {//perëjimai tarp dienø/mënesiø/metø
		hour -= 24;
		if (++ALLGNSSDATA.day > dim) { ALLGNSSDATA.day = 1; if (++ALLGNSSDATA.month > 12) { ALLGNSSDATA.month = 1; year++; } }
		} else if (hour < 0) {
		hour += 24;
		if (--ALLGNSSDATA.day == 0) {
			if (--ALLGNSSDATA.month == 0) { ALLGNSSDATA.month = 12; year--; }
			leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
			dim = (ALLGNSSDATA.month==4||ALLGNSSDATA.month==6||ALLGNSSDATA.month==9||ALLGNSSDATA.month==11)?30:(ALLGNSSDATA.month==2?(leap?29:28):31);
			ALLGNSSDATA.day = dim;
		}
	}

	ALLGNSSDATA.hour = (uint8_t)hour;
	ALLGNSSDATA.day = ALLGNSSDATA.day;
	ALLGNSSDATA.month = ALLGNSSDATA.month;
	ALLGNSSDATA.year = (uint8_t)(year - 2000);
}



