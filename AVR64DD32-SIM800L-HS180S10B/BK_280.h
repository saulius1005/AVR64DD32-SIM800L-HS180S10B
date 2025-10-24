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

#define GMT 3  //(pvz., Lietuva þiemà GMT+2, vasarà GMT+3)

typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t day;
    uint8_t month;
    uint8_t year; // tik paskutiniai du skaitmenys, pvz. 23 -> 2023
} AllGPSData;

extern AllGPSData ALLGNSSDATA;       // Global instance holding all GNSS data
extern char buffer[NMEA_BUFFER_SIZE]; // Buffer for incoming NMEA sentences

#endif /* BK_280_H_ */
