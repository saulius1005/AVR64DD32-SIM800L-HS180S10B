/*
 * RS485USARTVar.h
 *
 * Created: 2025-09-09 13:20:19
 *  Author: Saulius
 */ 


#ifndef RS485USARTVAR_H_
#define RS485USARTVAR_H_

RS485COM RS485 = {
	.lost_connecton_fault = false,
	.lost_signal_fault = false,
	.data_fault = false
};

#endif /* RS485USARTVAR_H_ */