/*
 * SIM800LVar.h
 *
 * Created: 2025-10-12 21:57:33
 *  Author: Saulius
 */ 


#ifndef SIM800LVAR_H_
#define SIM800LVAR_H_


SIM800Ldata SIM800L = {
	.bad_signal_fault = false, // bad FO signal couses data to show  00000.... declaration at true if at MCU starts FO will not work its prevents from motors spinning (tries reach 0)
	.lost_signal_fault = false, //if module not showing life (no power or MCU dead)
	.data_fault = false, // bad crc
	.lost_connecton_fault = false,// true if usart1 while loop spins without receiving data, after 3 times repeting same error in a row seting up FO_no_power_fault in FO data receiving function (both resets after data receiving goes back (autoreset))
	.faultcount = 0,
	.FreshDataPack = {"\0"},
	.ready = false
};

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

	CommandReplace_t cmd_replace_table[] = {
		{ "AT+HTTPPARA=\"URL\"", "Send: " },
		{ "AT+HTTPACTION", "Data receive status\n" },
		{ "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\n", "Set GPRS Connection Type\n" },
		{ "AT+SAPBR=3,1,\"APN\",\"internet.tele2.lt\"\n", "Set APN (Access Point Name)\n" },
		{ "AT+SAPBR=1,1\n", "Activate GPRS Bearer\n" },
		{ "AT+HTTPINIT\n", "Initialize HTTP Module\n" },
		{ "AT+HTTPPARA=\"CID\",1\n", "Set HTTP GPRS Channel\n" }
	};


#endif /* SIM800LVAR_H_ */