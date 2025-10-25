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

char* HTTPStatus(char *response)
{
	char *p = strstr(response, "+HTTPACTION:");
	if (!p) return "NO RESPONSE";
		p = strchr(p, ',');
	if (!p) return "PARSE ERROR";
		p++;
	uint16_t status_code = (uint16_t)atoi(p);
	for (uint8_t i = 0; i < sizeof(http_status_table)/sizeof(http_status_table[0]); i++) {
		if (http_status_table[i].code == status_code)
		return (char*)http_status_table[i].text;
	}
	return "UNKNOWN";
}


void ChangeAnswers(char *answer, size_t len)
{
	char *p = strstr(answer, "+HTTPACTION:");
	if (p) {
		const char *status = HTTPStatus(answer);
		strncpy(answer, status, len - 1);
		answer[len - 1] = '\0';
	}
}

void ChangeCommand(char *command, size_t len)
{
	for (uint8_t i = 0; i < sizeof(cmd_replace_table)/sizeof(cmd_replace_table[0]); i++) {
		if (strstr(command, cmd_replace_table[i].match)) {
			strncpy(command, cmd_replace_table[i].replace, len - 1);
			command[len - 1] = '\0';		
			if (cmd_replace_table[i].match[0] == 'A' && strstr(cmd_replace_table[i].match, "URL"))// Specialus atvejis "Send: " — reikia pridëti RS485 buferá
			{
				strncat(command, RS485data.dataBuffer, len - strlen(command) - 1);
				strncat(command, "\n", len - strlen(command) - 1);
			}
			return;
		}
	}
}

void SIM800L_ReadResponse(char *answer, uint16_t timeout_ms, bool keep_newlines)// Bendras UART atsakymo skaitymo modulis
{
	RTC_ON(timeout_ms);
	uint8_t index = 0;
	char prev_char = 0;
	while (!(RTC.INTFLAGS & RTC_OVF_bm))
	{
		char c = USART1_readCharRTC();
		if (c == '\r')
			continue; // CR visada praleidþiam
		if (!keep_newlines && c == '\n')
			continue; // jei nereikia newline — praleidþiam
		if (keep_newlines && c == '\n' && prev_char == '\n')
			continue; // dvigubø newline neleidþiam
		if (index < MESSAGE_LENGTH_SIM800L - 1)
		answer[index++] = c;
		prev_char = c;
	}
	RTC.INTFLAGS = RTC_OVF_bm;
	RTC_OFF();
	answer[index] = '\0';
}

void SIM800LDataSplitter(char *answer)//CSQ analizës funkcija 
{
	const char *ptr = strstr(answer, "+CSQ:");
	if (!ptr) return; // jei nerasta

	int8_t rssi = (int8_t)atoi(ptr + 5); // praeinam "+CSQ:"
	int16_t dbm = (rssi == 99) ? -999 : (-113 + (rssi * 2));

	screen_write_formatted_text(1, ALIGN_LEFT, BLUE, BLACK, "RSSI: %d dBm: %d", rssi, dbm);
}

void SIM800LUARTReceiver()//Receiver su CSQ
{
	USART_printf(1, "AT+CSQ\n");

	char answer[MESSAGE_LENGTH_SIM800L] = {0};
	SIM800L_ReadResponse(answer, 100, false);

	SIM800LDataSplitter(answer);
}

void SIM800LDataSplitter2(char *command, char *answer)//Pagrindinis terminalas veikimo metu
{
	char time_str[10] = {
		(ALLGNSSDATA.hour / 10) + '0', (ALLGNSSDATA.hour % 10) + '0', ':',
		(ALLGNSSDATA.minute / 10) + '0', (ALLGNSSDATA.minute % 10) + '0', ':',
		(ALLGNSSDATA.second / 10) + '0', (ALLGNSSDATA.second % 10) + '0', ' '
	};

	const char *parts[] = { time_str, command, answer };
	const uint16_t colors[] = { DARK_GRAY, GREEN, RED };

	screen_write_coloured_text_autoscroll(
	3, ALIGN_LEFT, BLACK, parts, colors, 3);
}

void SIM800LUARTReceiver2(uint16_t timeout_ms, const char *format, ...)//Receiver pagrindiniam veikimui
{
	char command[128];
	va_list args;
	va_start(args, format);
	vsnprintf(command, sizeof(command), format, args);
	va_end(args);

	BK280_Data_Read(); // update time
	display_gps_date_and_time(); //show date
	USART_printf(1, "%s", command);

	char answer[MESSAGE_LENGTH_SIM800L] = {0};
	SIM800L_ReadResponse(answer, timeout_ms, false);

	ChangeAnswers(answer, MESSAGE_LENGTH_SIM800L);
	ChangeCommand(command, MESSAGE_LENGTH_SIM800L);

	SIM800LDataSplitter2(command, answer);
}

void SIM800LDataSplitter3(char *answer)//Terminalas kai nesiunèiama uþklausa
{
	const char *parts[] = { answer };
	const uint16_t colors[] = { DARK_GREEN };

	screen_write_coloured_text_autoscroll(
	6, ALIGN_CENTER, BLACK, parts, colors, 1);

	_delay_ms(2000);
}

void SIM800LUARTReceiver3(uint16_t timeout_ms)//Receiver terminalui be uþklausos
{
	char answer[MESSAGE_LENGTH_SIM800L] = {0};
	SIM800L_ReadResponse(answer, timeout_ms, true); // iðsaugom \n

	SIM800LDataSplitter3(answer);
	SIM800L.ready = true;
}

uint8_t sim800l_check() {

	SIM800LUARTReceiver3(10000);
	if (SIM800L.ready) {
		return 0; // animacija baigta
	}
	return 1; // animacija tæsiama
}