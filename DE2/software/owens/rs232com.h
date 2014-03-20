/*
 * rs232com.h
 *
 *  Created on: 2014-03-15
 *      Author: Phil
 */

#ifndef RS232COM_H_
#define RS232COM_H_

#include <stdbool.h>

#define SEND_TO_ALL 0xFF

//public
void uart_init();
bool send_data(unsigned char id, unsigned char dataType, unsigned char size, unsigned char *data);
unsigned int get_buffer_space();

//semi-public
//don't use unless you absolutely have to
bool send_ack(unsigned char id);

#endif /* RS232COM_H_ */
