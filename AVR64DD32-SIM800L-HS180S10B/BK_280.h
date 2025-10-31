/*
 * BK_280.h
 *
 * Created: 2025-07-12 11:43:04
 * Author: Saulius
 */

#ifndef BK_280_H_
#define BK_280_H_

#define NMEA_BUFFER_SIZE 60   // Maximum length of the NMEA data buffer
#define MAX_SATELLITES 64     // Maximum number of satellites to handle

// Timezone offset (e.g., Lithuania: GMT+2 in winter, GMT+3 in summer)
#define GMT 3

/*
 * Structure holding parsed GNSS date and time data.
 * Year field stores only the last two digits (e.g., 23 ? 2023).
 */
typedef struct {
    uint8_t hour;    // UTC hour
    uint8_t minute;  // UTC minute
    uint8_t second;  // UTC second
    uint8_t day;     // Day of month
    uint8_t month;   // Month (1–12)
    uint8_t year;    // Last two digits of the year
} AllGPSData;

extern AllGPSData ALLGNSSDATA;        // Global instance containing all GNSS data
extern char buffer[NMEA_BUFFER_SIZE]; // Buffer for incoming NMEA sentences

#endif /* BK_280_H_ */
