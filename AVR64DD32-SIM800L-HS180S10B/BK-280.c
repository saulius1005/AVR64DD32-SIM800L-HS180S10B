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
bool parse_GNRMC(const char *buffer, RMC_Data *r) {// only time and date
	const char *line = strstr(buffer, "$GNRMC");
	if (!line) return false;
	char copy[NMEA_BUFFER_SIZE];
	strncpy(copy, line, sizeof(copy) - 1);
	copy[sizeof(copy) - 1] = '\0';
	char *fields[15] = {0};
	int count = split_fields(copy, fields, 15);
	if (count < 10) return false;
	if (fields[1] && strlen(fields[1]) >= 6) {
		snprintf(r->utc, sizeof(r->utc), "%c%c:%c%c:%c%c",
		fields[1][0], fields[1][1],
		fields[1][2], fields[1][3],
		fields[1][4], fields[1][5]);
		} else {
		strcpy(r->utc, "00:00:00");
	}
	if (fields[9]) strncpy(r->date, fields[9], sizeof(r->date));
	else r->date[0] = '\0';
	return true;
}


void display_gps_date_and_time(AllGPSData *data) {

	memset(data, 0, sizeof(AllGPSData));
	if (strstr(buffer, "$GNRMC") && parse_GNRMC(buffer, &data->rmc)) {
		data->has_rmc = true;
	}
	st7735_draw_text(0, 16 ,"laikas:", GREEN, BLACK);
	st7735_draw_text(48, 16 ,data->rmc.date, GREEN, BLACK);
	st7735_draw_text(0, 24 ,"data:", GREEN, BLACK);
	st7735_draw_text(48, 24 ,data->rmc.utc, GREEN, BLACK);

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

