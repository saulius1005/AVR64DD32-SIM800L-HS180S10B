/*
 * BK_280Var.h
 *
 * Created: 2025-07-12 11:44:51
 *  Author: Saulius
 */ 


#ifndef BK_280VAR_H_
#define BK_280VAR_H_

AllGPSData ALLGNSSDATA = {
    .hour = 0,
    .minute = 0,
    .second = 0,
    .day = 0,
    .month = 0,
    .year = 0

};
char buffer[NMEA_BUFFER_SIZE]={};

#endif /* BK-280VAR_H_ */