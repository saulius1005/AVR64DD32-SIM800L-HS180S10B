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

/**
 * @brief Parses an NMEA $GNRMC sentence and fills the RMC_Data structure.
 *
 * @param buffer Input NMEA data buffer (string containing multiple sentences).
 * @param r      Pointer to the RMC_Data structure to fill with parsed values.
 *
 * @return true if parsing was successful, false otherwise.
 */
bool parse_GNRMC(const char *buffer, RMC_Data *r) {
	const char *line = strstr(buffer, "$GNRMC");
	if (!line) return false;

	char copy[NMEA_BUFFER_SIZE];
	strncpy(copy, line, sizeof(copy) - 1);
	copy[sizeof(copy) - 1] = '\0';

	char *fields[15] = {0};
	int count = split_fields(copy, fields, 15);
	if (count < 10) return false;

	// Parse time hhmmss
	if (fields[1] && strlen(fields[1]) >= 6) {
		r->hour   = (fields[1][0] - '0') * 10 + (fields[1][1] - '0');
		r->minute = (fields[1][2] - '0') * 10 + (fields[1][3] - '0');
		r->second = (fields[1][4] - '0') * 10 + (fields[1][5] - '0');
		} else {
		r->hour = r->minute = r->second = 0;
	}

	// Parse date ddmmyy
	if (fields[9] && strlen(fields[9]) == 6) {
		r->day   = (fields[9][0] - '0') * 10 + (fields[9][1] - '0');
		r->month = (fields[9][2] - '0') * 10 + (fields[9][3] - '0');
		r->year  = (fields[9][4] - '0') * 10 + (fields[9][5] - '0');
		} else {
		r->day = r->month = r->year = 0;
	}

	return true;
}


void display_gps_date_and_time(void) {
	memset(&ALLGNSSDATA.rmc, 0, sizeof(ALLGNSSDATA.rmc));
	if (strstr(buffer, "$GNRMC") && parse_GNRMC(buffer, &ALLGNSSDATA.rmc)) {
		ALLGNSSDATA.has_rmc = true;
	}
	screen_write_formatted_text(0, ALIGN_LEFT, WHITE, BLACK,
	"Laikas: %02u:%02u:%02u\n"
	"Data: %04u-%02u-%02u",
	ALLGNSSDATA.rmc.hour,
	ALLGNSSDATA.rmc.minute,
	ALLGNSSDATA.rmc.second,
	((uint16_t)ALLGNSSDATA.rmc.year) + 2000,
	ALLGNSSDATA.rmc.month,
	ALLGNSSDATA.rmc.day);

}

/**
 * @brief Reads GPS data from USART and captures NMEA sentences from $GNRMC to $GNGLL.
 *
 * This function fills the global `buffer` with all NMEA sentences starting from
 * the first detected $GNRMC sentence and ending after a $GNGLL sentence (terminated by CRLF).
 *
 * @note Uses a blocking read approach with `USART0_readChar()`.
 */
void BK280_Data_Read(void) {
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
			break;
		}

		// Apsauga, jei buferis perpildomas
		if (index >= NMEA_BUFFER_SIZE - 1) {
			buffer[NMEA_BUFFER_SIZE - 1] = '\0';
			break;
		}
	}
}

