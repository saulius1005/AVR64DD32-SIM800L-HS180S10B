/*
 * BK_280.h
 *
 * Created: 2025-07-12 11:43:04
 *  Author: Saulius
 */ 


#ifndef BK_280_H_
#define BK_280_H_

#define NMEA_BUFFER_SIZE 60
#define MAX_SATELLITES 64

typedef struct {
	char utc[11];          // UTC time in HH:MM:SS format

	char date[9];          // Date in DDMMYY format
} RMC_Data;



typedef struct {
	RMC_Data rmc;        // Recommended Minimum Navigation Information
	bool has_rmc;        // Flag indicating if RMC data is available
} AllGPSData;

extern AllGPSData ALLGNSSDATA;       // Global instance holding all GNSS data
extern char buffer[NMEA_BUFFER_SIZE]; // Buffer for incoming NMEA sentences

#endif /* BK_280_H_ */
