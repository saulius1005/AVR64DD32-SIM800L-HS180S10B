/*
 * SIM800LVar.h
 *
 * Variables and tables for SIM800L module communication
 *
 * Created: 2025-10-12 21:57:33
 * Author: Saulius
 */

#ifndef SIM800LVAR_H_
#define SIM800LVAR_H_

/* Global instance to track SIM800L state */
SIM800Ldata SIM800L = {
    .lost_signal_fault = false, ///< True if module shows no life (no power or MCU dead)
    .ready = false              ///< True when module is ready
};

/* Table mapping HTTP status codes to human-readable text */
HttpStatus_t http_status_table[] = {
    {100, "Continue"}, 
    {101, "Switching Protocols"},
    {200, "OK"}, 
    {201, "Created"}, 
    {202, "Accepted"},
    {203, "Non-Authoritative Information"}, 
    {204, "No Content"},
    {205, "Reset Content"}, 
    {206, "Partial Content"},
    {300, "Multiple Choices"}, 
    {301, "Moved Permanently"},
    {302, "Found"}, 
    {303, "See Other"}, 
    {304, "Not Modified"},
    {305, "Use Proxy"}, 
    {307, "Temporary Redirect"},
    {400, "Bad Request"}, 
    {401, "Unauthorized"},
    {402, "Payment Required"}, 
    {403, "Forbidden"},
    {404, "Not Found"}, 
    {405, "Method Not Allowed"},
    {406, "Not Acceptable"}, 
    {407, "Proxy Authentication Required"},
    {408, "Request Time-out"}, 
    {409, "Conflict"}, 
    {410, "Gone"},
    {411, "Length Required"}, 
    {412, "Precondition Failed"},
    {413, "Request Entity Too Large"}, 
    {414, "Request-URI Too Large"},
    {415, "Unsupported Media Type"}, 
    {416, "Requested Range Not Satisfiable"},
    {417, "Expectation Failed"}, 
    {500, "Internal Server Error"},
    {501, "Not Implemented"}, 
    {502, "Bad Gateway"},
    {503, "Service Unavailable"}, 
    {504, "Gateway Time-out"},
    {505, "HTTP Version not supported"}, 
    {600, "Not HTTP PDU"},
    {601, "Network Error"}, 
    {602, "No memory"},
    {603, "DNS Error"}, 
    {604, "Stack Busy"}
};

/* Table to map specific AT commands to descriptive text */
CommandReplace_t cmd_replace_table[] = {
    { "AT+HTTPPARA=\"URL\"", "Send: " },                       ///< HTTP URL command
    { "AT+HTTPACTION", "Data receive status\n" },             ///< HTTP action command
    { "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\n", "Set GPRS Connection Type\n" }, ///< Set GPRS type
    { "AT+SAPBR=3,1,\"APN\",\"internet.tele2.lt\"\n", "Set APN (Access Point Name)\n" }, ///< Set APN
    { "AT+SAPBR=1,1\n", "Activate GPRS Bearer\n" },           ///< Activate GPRS
    { "AT+SAPBR=2,1\n", "Getting IP address\n"},              ///< Get IP
    { "AT+HTTPINIT\n", "Initialize HTTP Module\n" },         ///< Initialize HTTP
    { "AT+HTTPPARA=\"CID\",1\n", "Set HTTP GPRS Channel\n" } ///< Set HTTP GPRS channel
};

#endif /* SIM800LVAR_H_ */
