/*
 * BK_280.c
 *
 * Created: 2025-07-12 09:47:59
 * Author: Saulius
 */ 

#include "Settings.h"
#include "BK_280Var.h"

/*
 * Splits a comma-separated string into individual fields.
 * Returns the number of fields found.
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

/*
 * Parses the $GNRMC NMEA sentence and extracts
 * UTC time (hhmmss) and date (ddmmyy) fields.
 */
void parse_GNRMC() {
	char *fields[15] = {0};
	int count = split_fields(buffer, fields, 15);
	if (count < 10) return;

	// Parse UTC time (hhmmss)
	if (fields[1] && strlen(fields[1]) >= 6) {
		ALLGNSSDATA.hour   = (fields[1][0] - '0') * 10 + (fields[1][1] - '0');
		ALLGNSSDATA.minute = (fields[1][2] - '0') * 10 + (fields[1][3] - '0');
		ALLGNSSDATA.second = (fields[1][4] - '0') * 10 + (fields[1][5] - '0');
	} else {
		ALLGNSSDATA.hour = ALLGNSSDATA.minute = ALLGNSSDATA.second = 0;
	}

	// Parse date (ddmmyy)
	if (fields[9] && strlen(fields[9]) == 6) {
		ALLGNSSDATA.day   = (fields[9][0] - '0') * 10 + (fields[9][1] - '0');
		ALLGNSSDATA.month = (fields[9][2] - '0') * 10 + (fields[9][3] - '0');
		ALLGNSSDATA.year  = (fields[9][4] - '0') * 10 + (fields[9][5] - '0');
	} else {
		ALLGNSSDATA.day = ALLGNSSDATA.month = ALLGNSSDATA.year = 0;
	}
}

/*
 * Displays the current GPS date on the screen in the format YYYY-MM-DD.
 */
void display_gps_date_and_time() {
	screen_write_formatted_text(
		0,
		ALIGN_CENTER,
		WHITE,
		BLACK,
		"%04u-%02u-%02u",
		((uint16_t)ALLGNSSDATA.year) + 2000,
		ALLGNSSDATA.month,
		ALLGNSSDATA.day
	);
}

/*
 * Reads the $GNRMC NMEA sentence from the GNSS module via UART.
 * Extracts the full sentence into a buffer, then parses and applies timezone correction.
 */
void BK280_Data_Read() {
	uint16_t index = 0;
	bool capturing = false;
	static const char target[] = "$GNRMC";
	uint8_t match_index = 0;

	while (1) {
		char c = USART0_readChar();

		if (!capturing) {
			// Wait for the start of the $GNRMC sentence
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

		// Store incoming data into the buffer
		if (index < NMEA_BUFFER_SIZE - 1)
			buffer[index++] = c;

		// End of NMEA line detected (\r\n)
		if (c == '\n' && index >= 2 && buffer[index - 2] == '\r') {
			buffer[index] = '\0';
			parse_GNRMC();
			apply_timezone(); // Apply GMT offset
			break;
		}

		// Prevent buffer overflow
		if (index >= NMEA_BUFFER_SIZE - 1) {
			buffer[NMEA_BUFFER_SIZE - 1] = '\0';
			break;
		}
	}
}

/*
 * Checks if GNSS data was successfully received.
 * Returns 0 when valid time is available, otherwise returns 1.
 */
uint8_t gnss_check() {
	BK280_Data_Read(); // Read GNSS data
	if (ALLGNSSDATA.second != 0) {
		return 0; // GNSS data ready
	}
	return 1; // GNSS data not ready
}

/*
 * Applies timezone and daylight saving time (DST) correction
 * according to EU rules, adjusting date and time if needed.
 */
void apply_timezone(void) {
	int16_t year = 2000 + ALLGNSSDATA.year;
	int16_t hour = ALLGNSSDATA.hour;

	bool isDST = false;

	// Determine if DST applies (EU rules)
	if (ALLGNSSDATA.month > 3 && ALLGNSSDATA.month < 10)
		isDST = true;
	else if (ALLGNSSDATA.month == 3 || ALLGNSSDATA.month == 10) {
		uint8_t h = (31 + (13 * (ALLGNSSDATA.month + 1)) / 5 + year + (year / 4) - (year / 100) + (year / 400)) % 7;
		uint8_t lastSunday = 31 - ((h + 6) % 7);
		isDST = (ALLGNSSDATA.month == 3) ? (ALLGNSSDATA.day >= lastSunday) : (ALLGNSSDATA.day < lastSunday);
	}

	int8_t offset = GMT - (isDST ? 0 : 1); // Winter: -1 hour
	hour += offset;

	// Check for leap year
	bool leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));

	// Get days in current month
	uint8_t dim = (ALLGNSSDATA.month==4||ALLGNSSDATA.month==6||ALLGNSSDATA.month==9||ALLGNSSDATA.month==11) ? 30 :
				  (ALLGNSSDATA.month==2 ? (leap ? 29 : 28) : 31);

	// Handle day/month/year rollover
	if (hour >= 24) {
		hour -= 24;
		if (++ALLGNSSDATA.day > dim) {
			ALLGNSSDATA.day = 1;
			if (++ALLGNSSDATA.month > 12) {
				ALLGNSSDATA.month = 1;
				year++;
			}
		}
	} else if (hour < 0) {
		hour += 24;
		if (--ALLGNSSDATA.day == 0) {
			if (--ALLGNSSDATA.month == 0) {
				ALLGNSSDATA.month = 12;
				year--;
			}
			leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
			dim = (ALLGNSSDATA.month==4||ALLGNSSDATA.month==6||ALLGNSSDATA.month==9||ALLGNSSDATA.month==11) ? 30 :
				  (ALLGNSSDATA.month==2 ? (leap ? 29 : 28) : 31);
			ALLGNSSDATA.day = dim;
		}
	}

	ALLGNSSDATA.hour = (uint8_t)hour;
	ALLGNSSDATA.year = (uint8_t)(year - 2000);
}
