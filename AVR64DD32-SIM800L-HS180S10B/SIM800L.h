/*
 * SIM800L.h
 *
 * Definitions for SIM800L GSM/GPRS module communication and status tracking.
 *
 * Created: 2025-10-12 21:57:17
 * Author: Saulius
 */

#ifndef SIM800L_H_
#define SIM800L_H_

#define SIM800L_TIMEOUT_COUNTER 270000 ///< Timeout counter for SIM800L operations
#define MESSAGE_LENGTH_SIM800L 128     ///< Max length of SIM800L message buffer

/*
 * Structure to hold SIM800L state and data
 */
typedef struct {
    bool lost_signal_fault;      ///< Set if signal is lost
    bool ready;                  ///< Flag indicating module ready status
} SIM800Ldata;

/*
 * Structure for command replacement table
 */
typedef struct {
    const char *match;   ///< Command pattern to search
    const char *replace; ///< Replacement string
} CommandReplace_t;

/*
 * Structure for HTTP status code mapping
 */
typedef struct {
    uint16_t code;       ///< HTTP status code
    const char *text;    ///< Corresponding text description
} HttpStatus_t;

/* External variables */
extern SIM800Ldata SIM800L;              ///< Global SIM800L state
extern HttpStatus_t http_status_table[45]; ///< Table of HTTP status codes and texts
extern CommandReplace_t cmd_replace_table[8]; ///< Table for command-to-text replacements

#endif /* SIM800L_H_ */
