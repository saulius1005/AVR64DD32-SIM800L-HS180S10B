/*
 * SIM800L.c
 *
 * Created: 2025-10-12 21:57:03
 *  Author: Saulius
 */ 
#include "Settings.h"
#include "SIM800LVar.h"

uint64_t hexToUint64(const char *str) {
	uint64_t result = 0;
	char c;

	while ((c = *str++)) {
		result <<= 4; // kiekvienas hex simbolis = 4 bitai

		if (c >= '0' && c <= '9') {
			result |= (uint64_t)(c - '0');
			} else if (c >= 'A' && c <= 'F') {
			result |= (uint64_t)(c - 'A' + 10);
			} else if (c >= 'a' && c <= 'f') {
			result |= (uint64_t)(c - 'a' + 10);
			} else {
			// netinkamas simbolis
			return 0;
		}
	}
	return result;
}

void HTTP_alive_init(){
	//HTTP alive setting up
	//SIM800LUARTReceiver2("AT\n",100);
	SIM800LUARTReceiver2(100, "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\n");
	SIM800LUARTReceiver2(300, "AT+SAPBR=3,1,\"APN\",\"internet.tele2.lt\"\n"); //nustatom apn pagal savo SIM kortelës operatoriø
	SIM800LUARTReceiver2(1500, "AT+SAPBR=1,1\n"); // aktyvuojam GPRS
	SIM800LUARTReceiver2(180, "AT+HTTPINIT\n"); // inicializuojam HTTP moduli
	SIM800LUARTReceiver2(100, "AT+HTTPPARA=\"CID\",1\n"); // nustatom GPRS kanalà
}
void HTTP_alive_restart(){
	SIM800LUARTReceiver2(100, "AT+HTTPTERM\n");     // uþdaro HTTP modulá
	SIM800LUARTReceiver2(200, "AT+SAPBR=0,1\n");    // uþdaro GPRS sesijà
	SIM800LUARTReceiver2(100, "AT+SAPBR=1,1\n");    // vël aktyvuoja GPRS
	SIM800LUARTReceiver2(150, "AT+HTTPINIT\n");     // vël inicializuoja HTTP
	SIM800LUARTReceiver2(100, "AT+HTTPPARA=\"CID\",1\n");
}

void SIM800LDataSplitter(char *answer) {
	const char *ptr = strstr(answer, "+CSQ:");
	ptr += 5; // praeinam "+CSQ:"
	while (*ptr == ' ') ptr++; // praleidþiam tarpà
	int8_t rssi = (int8_t)atoi(ptr);
	int16_t dbm = 0;
	if (rssi == 99) 
		dbm= -999; // neþinoma
	else
		dbm= -113 + (rssi * 2);
	screen_write_formatted_text(1, ALIGN_LEFT, BLUE, BLACK,"RSSI: %d dBm: %d", rssi, dbm);
}

void SIM800LUARTReceiver() { //signalo tikrinimo funkcija
	USART_printf(1, "AT+CSQ\n");
	RTC_ON(100); //start answer waiting timer
	uint8_t index = 0;
	char answer[MESSAGE_LENGTH_SIM800L] = {0}; // Empty answer array

	while (!(RTC.INTFLAGS & RTC_OVF_bm)) {
		char c = USART1_readCharRTC(); // Read a character

		// Skip only carriage return
		if (c != '\r') {
			if (index < MESSAGE_LENGTH_SIM800L - 1) { // Prevent buffer overflow
				answer[index++] = c;
			}
		}
		if(c == '\n' && answer[index-2] == '\n')
		index--;
	}
	RTC.INTFLAGS = RTC_OVF_bm; // Clear RTC overflow flag
	RTC_OFF();
	SIM800LDataSplitter(answer);
}

void SIM800LDataSplitter3(char *answer) { //terminalas kuomet nesiunèiama uþklausa

	const char *parts[] = {
		answer
	};

	const uint16_t colors[] = {
		DARK_GREEN,   // "\nANSW: "
		GREEN   // answer
	};

	// 2. Kvieèiame (reikia ásitikinti, kad turite naujà funkcijos pavadinimà)
	screen_write_coloured_text_autoscroll(
	6,              // line (start_line)
	ALIGN_CENTER,     // alignment
	BLACK,          // bg (bendras fonas)
	parts,          // Teksto daliø masyvas
	colors,         // Spalvø masyvas
	1               // Daliø skaièius (4)
	);
	_delay_ms(2000);
}

void SIM800LUARTReceiver3(uint16_t answer_time_ms) {
	RTC_ON(answer_time_ms); //start answer waiting timer
	uint8_t index = 0;
	char answer[MESSAGE_LENGTH_SIM800L] = {0}; // Empty answer array

	while (!(RTC.INTFLAGS & RTC_OVF_bm)) {
		char c = USART1_readCharRTC(); // Read a character

		// Skip only carriage return
		if (c != '\r') {
			if (index < MESSAGE_LENGTH_SIM800L - 1) { // Prevent buffer overflow
				answer[index++] = c;
			}			
		}
		if(c == '\n' && answer[index-2] == '\n')
		index--;
	}
	RTC.INTFLAGS = RTC_OVF_bm; // Clear RTC overflow flag
	RTC_OFF();

	SIM800LDataSplitter3(answer); // Process received answer
	SIM800L.ready = true;
}

void SIM800LDataSplitter2(char *command, char *answer) { //pagrindinis veikimo metu veikiantis terminalas
	// 1. Paruoðiame teksto dalis ir spalvas
	char time_str[10]; // "HH:MM:SS" + '\0'

	time_str[0] = (ALLGNSSDATA.hour / 10) + '0';
	time_str[1] = (ALLGNSSDATA.hour % 10) + '0';
	time_str[2] = ':';
	time_str[3] = (ALLGNSSDATA.minute / 10) + '0';
	time_str[4] = (ALLGNSSDATA.minute % 10) + '0';
	time_str[5] = ':';
	time_str[6] = (ALLGNSSDATA.second / 10) + '0';
	time_str[7] = (ALLGNSSDATA.second % 10) + '0';
	time_str[8] = ' ';
	time_str[9] = '\0';

	const char *parts[] = {
		time_str,
/*		"CMD: ",*/
		command,
/*		"ANSW: ",*/
		answer
	};

	const uint16_t colors[] = {
		DARK_GRAY,
	//	DARK_GREEN,   // "CMD: "
		GREEN,  // command
	//	DARK_RED,   // "\nANSW: "
		RED   // answer
	};

	// 2. Kvieèiame (reikia ásitikinti, kad turite naujà funkcijos pavadinimà)
	screen_write_coloured_text_autoscroll(
	3,              // line (start_line)
	ALIGN_LEFT,     // alignment
	BLACK,          // bg (bendras fonas)
	parts,          // Teksto daliø masyvas
	colors,         // Spalvø masyvas
	3//5               // Daliø skaièius (4)
	);
	//screen_write_formatted_text_autoscroll(3, ALIGN_LEFT, GREEN, BLACK,"CMD: %s\nANSW: %s\n", command, answer);
}

char* HTTPStatus(char *response)
{
	// Surandam vietà, kur prasideda frazë "+HTTPACTION:"
	char *p = strstr(response, "+HTTPACTION:");
	if (p == NULL) return "NO RESPONSE";
	// Paslenkam rodyklæ po dviem kableliais iki status kodo
	p = strchr(p, ','); // pirmas kablelis
	if (p == NULL) return "PARSE ERROR";
	p++; // pereinam po kablelio
	const char *status_ptr = strchr(p, ','); // antras kablelis
	if (status_ptr == NULL) return "PARSE ERROR";

	
	uint16_t status_code = atoi(p);// Iðtraukiam statuso kodà

	// Gràþinam tekstà pagal status_code reikðmæ
	switch (status_code) { //according to 1.09 datasheet version
		case 100: return "Continue";
		case 101: return "Switching Protocols";
		case 200: return "OK"; //OK
		case 201: return "Created";
		case 202: return "Accepted";
		case 203: return "Non-Authoritative Information";
		case 204: return "No Content";
		case 205: return "Reset Content";
		case 206: return "Partial Content";
		case 300: return "Multiple Choices";
		case 301: return "Moved Permanently";
		case 302: return "Found";
		case 303: return "See Other";
		case 304: return "Not Modified";
		case 305: return "Use Proxy";
		case 307: return "Temporary Redirect";
		case 400: return "Bad Request";
		case 401: return "Unauthorized";
		case 402: return "Payment Required";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 406: return "Not Acceptable";
		case 407: return "Proxy Authentication Required";
		case 408: return "Request Time-out";
		case 409: return "Conflict";
		case 410: return "Gone";
		case 411: return "Length Required";
		case 412: return "Precondition Failed";
		case 413: return "Request Entity Too Large";
		case 414: return "Request-URI Too Large";
		case 415: return "Unsupported Media Type";
		case 416: return "Requested range not satisfiable";
		case 417: return "Expectation Failed";
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 502: return "Bad Gateway";
		case 503: return "Service Unavailable";
		case 504: return "Gateway Time-out";
		case 505: return "HTTP Version not supported";
		case 600: return "Not HTTP PDU";
		case 601: return "Network Error";
		case 602: return "No memory";
		case 603: return "DNS Error";
		case 604: return "Stack Busy";
		default:  return "UNKNOWN";
	}
}

void ChangeAnswers(char *answer, size_t answer_len) {
    if (strstr(answer, "+HTTPACTION:")) {
        const char *status_text = HTTPStatus(answer);  
        strncpy(answer, status_text, answer_len - 1);
        answer[answer_len - 1] = '\0';
    }
}

void ChangeCommand(char *command, size_t answer_len) {
	if (strstr(command, "AT+HTTPPARA=\"URL\"")) {		
		strncpy(command, "Send: ", answer_len - 1);
		strcat(command, RS485data.dataBuffer);
		strcat(command, "\n");
		command[answer_len - 1] = '\0';
	}
	else if (strstr(command, "AT+HTTPACTION")) {
		strncpy(command, "Data receive status\n", answer_len - 1);
		command[answer_len - 1] = '\0';
	}
	else if (strstr(command, "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\n")) {
		strncpy(command, "Set GPRS Connection Type\n", answer_len - 1);
		command[answer_len - 1] = '\0';
	}
	else if (strstr(command, "AT+SAPBR=3,1,\"APN\",\"internet.tele2.lt\"\n")) {
		strncpy(command, "Set APN (Access Point Name)\n", answer_len - 1);
		command[answer_len - 1] = '\0';
	}
	else if (strstr(command, "AT+SAPBR=1,1\n")) {
		strncpy(command, "Activate GPRS Bearer\n", answer_len - 1);
		command[answer_len - 1] = '\0';
	}
	else if (strstr(command, "AT+HTTPINIT\n")) {
		strncpy(command, "Initialize HTTP Module\n", answer_len - 1);
		command[answer_len - 1] = '\0';
	}
	else if (strstr(command, "AT+HTTPPARA=\"CID\",1\n")) {
		strncpy(command, "Set HTTP GPRS Channel\n", answer_len - 1);
		command[answer_len - 1] = '\0';
	}
}

void SIM800LUARTReceiver2(uint16_t answer_time_ms, const char *format, ...) {
	char command[128];
	va_list args;
	va_start(args, format); 
	vsnprintf(command, sizeof(command), format, args);
	va_end(args);

	BK280_Data_Read(); // read GNSS
	display_gps_date_and_time();

	USART_printf(1, "%s", command); // send formatted command
	RTC_ON(answer_time_ms); // start answer waiting timer

	uint8_t index = 0;
	char answer[MESSAGE_LENGTH_SIM800L] = {0};

	while (!(RTC.INTFLAGS & RTC_OVF_bm)) {
		char c = USART1_readCharRTC();
		if (c != '\r' && c != '\n') {
			if (index < MESSAGE_LENGTH_SIM800L - 1)
			answer[index++] = c;
		}
	}
	RTC.INTFLAGS = RTC_OVF_bm;
	RTC_OFF();
	ChangeAnswers(answer, MESSAGE_LENGTH_SIM800L); //comment this if you want see true answers
	ChangeCommand(command,MESSAGE_LENGTH_SIM800L); //comment this if you want see true commands
	SIM800LDataSplitter2(command, answer); // process received answer
}

uint8_t sim800l_check() {
	SIM800LUARTReceiver3(10000);
	if (SIM800L.ready) {
		return 0; // animacija baigta
	}
	return 1; // animacija tæsiama
}