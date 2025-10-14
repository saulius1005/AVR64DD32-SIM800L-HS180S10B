/*
 * RS485USART.h
 *
 * Created: 2025-09-05 11:15:15
 *  Author: Saulius
 */ 


#ifndef RS485USART_H_
#define RS485USART_H_

/**
 * @brief Maximum count for consecutive errors before marking the system as faulty.
 */
#define RS485_TIMEOUT_COUNTER 200000 ///< Timeout counter value for operations slower speed meaning more this
#define CountForError_RS485 3 //times to receive signal error and after this number will be set up connection fault
#define MESSAGE_LENGTH_RS485 20


typedef struct {
	bool lost_connecton_fault;
	bool lost_signal_fault;
	bool data_fault;
} RS485COM;

extern RS485COM RS485;

#endif /* RS485USART_H_ */