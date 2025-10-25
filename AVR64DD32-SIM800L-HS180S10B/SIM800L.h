/*
 * SIM800L.h
 *
 * Created: 2025-10-12 21:57:17
 *  Author: Saulius
 */ 


#ifndef SIM800L_H_
#define SIM800L_H_


#define SIM800L_TIMEOUT_COUNTER 270000 ///< Timeout counter value for operations if baud k
#define CountForError_FO 3
#define Angle_Precizion 100 // 100 meaning exp. 360 degree 10 3600 (360,0), 1 36000 (360,00)
#define U_I_Precizion 1 // 10 meaning 24, 1 240 (24.0)
#define MESSAGE_LENGTH_SIM800L 128


typedef struct {
	bool bad_signal_fault;
	bool lost_signal_fault;
	bool data_fault;
	bool lost_connecton_fault;
	uint8_t faultcount;
	char FreshDataPack[MESSAGE_LENGTH_SIM800L];
	bool ready;
} SIM800Ldata;



// --- Kompaktiðkas „Command-to-text“ lentelinis keitimas ---
typedef struct {
	const char *match;
	const char *replace;
} CommandReplace_t;

typedef struct {
	uint16_t code;
	const char *text;
} HttpStatus_t;


extern SIM800Ldata SIM800L;
extern HttpStatus_t http_status_table[45];
extern CommandReplace_t cmd_replace_table[7];

#endif /* SIM800L_H_ */