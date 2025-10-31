/*
 * SIM800L.c
 *
 * SIM800L GSM/GPRS module control and UART handling.
 *
 * Created: 2025-10-12 21:57:03
 * Author: Saulius
 */

#include "Settings.h"
#include "SIM800LVar.h"

/*
 * Convert a hexadecimal string to uint64_t.
 */
uint64_t hexToUint64(const char *str) {
    uint64_t result = 0;
    char c;

    while ((c = *str++)) {
        result <<= 4; // each hex digit = 4 bits
        if (c >= '0' && c <= '9') {
            result |= (uint64_t)(c - '0');
        } else if (c >= 'A' && c <= 'F') {
            result |= (uint64_t)(c - 'A' + 10);
        } else if (c >= 'a' && c <= 'f') {
            result |= (uint64_t)(c - 'a' + 10);
        } else {
            return 0; // invalid character
        }
    }
    return result;
}

/*
 * Initialize HTTP module and GPRS settings.
 */
void HTTP_alive_init() {
    // Configure GPRS and HTTP module
    SIM800LUARTReceiver2(1850, "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\n");
    SIM800LUARTReceiver2(1850, "AT+SAPBR=3,1,\"APN\",\"internet.tele2.lt\"\n"); // Set APN
    SIM800LUARTReceiver2(1850, "AT+SAPBR=1,1\n"); // Activate GPRS
    SIM800LUARTReceiver2(1850, "AT+SAPBR=2,1\n");
    SIM800LUARTReceiver2(100, "AT+HTTPINIT\n"); // Initialize HTTP module
    SIM800LUARTReceiver2(100, "AT+HTTPPARA=\"CID\",1\n"); // Set GPRS channel
}

/*
 * Parse HTTP status code from module response.
 */
char* HTTPStatus(char *response) {
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

/*
 * Clean up or adjust SIM800L responses for display or processing.
 */
void ChangeAnswers(char *answer, size_t len) {
    char *p = strstr(answer, "+HTTPACTION:");
    if (p) {
        const char *status = HTTPStatus(answer);
        strncpy(answer, status, len - 1);
        answer[len - 1] = '\0';
    }

    char *b = strstr(answer, "+SAPBR: 1,1,");
    if (b != NULL) {
        b += strlen("+SAPBR: 1,1,");
        if (*b == '"') b++;
        char *end = strchr(b, '"');
        if (end != NULL) {
            *end = '\0';
            memmove(answer, b, strlen(b) + 1);
        }
    }
}

/*
 * Replace commands based on predefined table and optionally append RS485 data.
 */
void ChangeCommand(char *command, size_t len) {
    for (uint8_t i = 0; i < sizeof(cmd_replace_table)/sizeof(cmd_replace_table[0]); i++) {
        if (strstr(command, cmd_replace_table[i].match)) {
            strncpy(command, cmd_replace_table[i].replace, len - 1);
            command[len - 1] = '\0';
            if (cmd_replace_table[i].match[0] == 'A' && strstr(cmd_replace_table[i].match, "URL")) {
                strncat(command, RS485data.dataBuffer, len - strlen(command) - 1);
                strncat(command, "\n", len - strlen(command) - 1);
            }
            return;
        }
    }
}

/*
 * Read SIM800L UART response with optional newline handling.
 */
void SIM800L_ReadResponse(char *answer, uint16_t timeout_ms, bool keep_newlines) {
    RTC_ON(timeout_ms);
    uint8_t index = 0;
    char prev_char = 0;

    while (!(RTC.INTFLAGS & RTC_OVF_bm)) {
        char c = USART1_readCharRTC();
        if (c == '\r') continue;
        if (!keep_newlines && c == '\n') continue;
        if (keep_newlines && c == '\n' && prev_char == '\n') continue;
        if (index < MESSAGE_LENGTH_SIM800L - 1)
            answer[index++] = c;
        prev_char = c;
    }

    RTC.INTFLAGS = RTC_OVF_bm;
    RTC_OFF();
    answer[index] = '\0';
}

/*
 * Parse RSSI from +CSQ response and display on screen.
 */
void SIM800LDataSplitter(char *answer) {
    const char *ptr = strstr(answer, "+CSQ:");
    if (!ptr) return;

    int8_t rssi = (int8_t)atoi(ptr + 5);
    int16_t dbm = (rssi == 99) ? -999 : (-113 + (rssi * 2));

    screen_write_formatted_text(1, ALIGN_LEFT, BLUE, BLACK, "RSSI: %d dBm: %d", rssi, dbm);
}

/*
 * Send AT+CSQ command and display result.
 */
void SIM800LUARTReceiver() {
    USART_printf(1, "AT+CSQ\n");
    char answer[MESSAGE_LENGTH_SIM800L] = {0};
    SIM800L_ReadResponse(answer, 100, false);
    SIM800LDataSplitter(answer);
}

/*
 * Display command, response, and time on terminal.
 */
void SIM800LDataSplitter2(char *command, char *answer) {
    char time_str[10] = {
        (ALLGNSSDATA.hour / 10) + '0', (ALLGNSSDATA.hour % 10) + '0', ':',
        (ALLGNSSDATA.minute / 10) + '0', (ALLGNSSDATA.minute % 10) + '0', ':',
        (ALLGNSSDATA.second / 10) + '0', (ALLGNSSDATA.second % 10) + '0', ' '
    };
    const char *parts[] = { time_str, command, answer };
    const uint16_t colors[] = { DARK_GRAY, GREEN, RED };

    screen_write_coloured_text_autoscroll(3, ALIGN_LEFT, BLACK, parts, colors, 3);
}

/*
 * Send formatted command, read response, and update terminal display.
 */
void SIM800LUARTReceiver2(uint16_t timeout_ms, const char *format, ...) {
    char command[128];
    va_list args;
    va_start(args, format);
    vsnprintf(command, sizeof(command), format, args);
    va_end(args);

    BK280_Data_Read();          // Update GNSS time
    display_gps_date_and_time();

    USART_printf(1, "%s", command);

    char answer[MESSAGE_LENGTH_SIM800L] = {0};
    SIM800L_ReadResponse(answer, timeout_ms, false);

    ChangeAnswers(answer, MESSAGE_LENGTH_SIM800L);
    ChangeCommand(command, MESSAGE_LENGTH_SIM800L);

    SIM800LDataSplitter2(command, answer);
}

/*
 * Display a response on terminal without sending a command.
 */
void SIM800LDataSplitter3(char *answer) {
    const char *parts[] = { answer };
    const uint16_t colors[] = { DARK_GREEN };

    screen_write_coloured_text_autoscroll(6, ALIGN_CENTER, BLACK, parts, colors, 1);
    _delay_ms(2000);
}

/*
 * Receive SIM800L response without sending a command.
 */
void SIM800LUARTReceiver3(uint16_t timeout_ms) {
    char answer[MESSAGE_LENGTH_SIM800L] = {0};
    SIM800L_ReadResponse(answer, timeout_ms, true); // Keep newlines

    SIM800LDataSplitter3(answer);
    SIM800L.ready = true;
}

/*
 * Check if SIM800L is ready for animation.
 */
uint8_t sim800l_check() {
    SIM800LUARTReceiver3(10000);
    if (SIM800L.ready) return 0; // animation finished
    return 1;                     // animation continues
}
