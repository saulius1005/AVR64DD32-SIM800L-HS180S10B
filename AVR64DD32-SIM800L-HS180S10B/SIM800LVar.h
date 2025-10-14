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
	.FreshDataPack = {"\0"}
};


#endif /* SIM800LVAR_H_ */