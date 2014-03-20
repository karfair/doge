#include <stdio.h>
#include <string.h>
#include "altera_up_avalon_rs232.h"
#include "sys/alt_irq.h"
#include "io.h"

#include "rs232com.h"
#include "bomb.h"

#define BUFFER_SIZE 1024 //sending buffer
#define TIMER_PERIOD 200000 //200k ticks -> 4mS

//USER DEFINED






//DO NOT EXTERN OR MODIFY
////////////////////////////
alt_up_rs232_dev* uart;
bool hasReadHeader = false;
unsigned char id, dataType, size;
unsigned char buffer[128]; //input buffer

unsigned char send[BUFFER_SIZE]; //output buffer
int Start, End;

void uart_isr() {
	//receiving data


	unsigned char parity;
	int start, end, queue_size, amount_to_send,FIFO_avail;

	while(alt_up_rs232_get_used_space_in_read_FIFO(uart) > 0) {
		if(!hasReadHeader) {
			if(alt_up_rs232_get_used_space_in_read_FIFO(uart) >= 3) {
				// First byte is ID
				alt_up_rs232_read_data(uart, &id, &parity);
				//2nd byte is the amount of data (including the dataType)
				alt_up_rs232_read_data(uart, &size, &parity);
				//3rd byte is the dataType
				alt_up_rs232_read_data(uart, &dataType, &parity);

				hasReadHeader = true;
				size--; //remaining data
				//printf("0:id: %i size: %i datatype: %i \n", id,size,dataType);
			}else {
				goto Send;
			}
		}
		if(size > 0) {
			//printf("1: space_avail: %i, size: %i\n",alt_up_rs232_get_used_space_in_read_FIFO(uart),size);
			//wait for all the data to arrive

			if(alt_up_rs232_get_used_space_in_read_FIFO(uart) < size ) {
				printf("1");
				goto Send;
			}
			//printf("2:id: %i size: %i datatype: %i \n", id,size,dataType);
			//receiving data into buffer
			unsigned i;
			for (i = 0; i < size; i++) {
				alt_up_rs232_read_data(uart, buffer+i, &parity);
				//printf("%c",buffer[i]);
			}
			//printf("\n");
		}

		//turn off hasReadHeader
		hasReadHeader = false;

		//send ack
		if(!send_ack(id)){
			//printf("ack not sent to id:%i! prepared to die...\n", (int)id);
		}else{
			//printf("ack to ID: %i\n",id);
		}

		//TODO: Modify stuff here to do different things for different data type
		switch(dataType){
		case 0:
			//speed test mode, do nothing
			break;
		case 1:
			send_data(id, dataType, size, buffer);
			break;
		case 10:
			setName(id, size, buffer);
			break;
		}
	}

	Send:

	//sending data

	//create a copy of the global variables
	start = Start;
	end = End;

	queue_size = start < end ? end - start : end + BUFFER_SIZE - start; //how much is there to send
	queue_size--; //start != end
	FIFO_avail = alt_up_rs232_get_available_space_in_write_FIFO(uart); //how much can we send?
	amount_to_send = FIFO_avail > queue_size ? queue_size : FIFO_avail; //send the lower amount

	const unsigned int BASE_addr = uart->base;
	const unsigned int RAVAIL = uart->base + 2;

	if(amount_to_send > 0){
		//printf("transmit:  %i bytes\n\n\n",amount_to_send);
		int i;
		for(i = 0; i < amount_to_send; i++) {
			int temp_start = start;
			start++;
			if(start == BUFFER_SIZE) start = 0;
			unsigned char temp_data = send[start];

			if(IORD_8DIRECT(RAVAIL,0)==0){
				IOWR_8DIRECT(BASE_addr,0,temp_data); //sends the actual data
			}else{ //revert changes if send not successful
				start = temp_start;
				break;
			}
			//printf("\n%c,%X,%i",send[start],send[start],send[start]);

		}
	}

	Start = start; //sets the global
	IOWR_16DIRECT(TIMER_0_BASE,0,0); //needed to show that interrupt finished executing
	IOWR_16DIRECT(TIMER_0_BASE,4,0x5); //restarts timer
	return;
}

//sends ack to the android phone
bool send_ack(unsigned char id) {
	//calculate available space
	int end = End;
	int start = Start;
	int avail_to_fill = start > end ? start - end : start + BUFFER_SIZE - end;
	avail_to_fill--; //start != end

	if(avail_to_fill < 3){
		printf("error: send(): not enough space in buffer");
		return false;
	}else{
		send[end++] = id;
		if( end == BUFFER_SIZE ) end = 0; //reached end of buffer
		send[end++] = 1;
		if( end == BUFFER_SIZE ) end = 0; //reached end of buffer
		send[end++] = 0;
		if( end == BUFFER_SIZE ) end = 0; //reached end of buffer
	}
	End = end;
	return true;
}

//initialize uart functions
void uart_init() {

	//crap data holder
	unsigned char parity, data;

    //UART Initialization
    uart = alt_up_rs232_open_dev(RS232_0_NAME);

    //Clearing read buffer to start
	while (alt_up_rs232_get_used_space_in_read_FIFO(uart)) {
		alt_up_rs232_read_data(uart, &data, &parity);
	}

	//clear the sending buffer
	Start = 0;
	End = 1;

	//start the receiving interrupt
	IOWR_16DIRECT(TIMER_0_BASE, 8, TIMER_PERIOD & 0xFFFF); //writes the period to the hardware timer
	IOWR_16DIRECT(TIMER_0_BASE, 12, TIMER_PERIOD >> 16);
	IOWR_16DIRECT(TIMER_0_BASE, 4, 1 << 3); //stop timer

	alt_irq_register(TIMER_0_IRQ,NULL,(void*)uart_isr);//registers function to a specific IRQ

	IOWR_16DIRECT(TIMER_0_BASE, 4, 0x5);	//starts timer with interrupt
}

//input:
//id: to which device id?
//dataType
//size: size of the data array, excluding the dataType byte (cannot exceed 124)
//*data: pointer to the character array
//output:
//true if send, false if not send
bool send_data(unsigned char id, unsigned char dataType, unsigned char size, unsigned char *data) {
	//in addition to the data, we're also sending
	//[0] id
	//[1] data_size + 2
	//[2] data_size + 1
	//[3] data_type
	//so max_size is 124 bytes

	//calculate available space
	int end = End;
	int start = Start;
	int avail_to_fill = start > end ? start - end : start + BUFFER_SIZE - end;
	avail_to_fill--; //start != end

	if(size>124){
		printf("error: send(): sending too much data! size must be <= 124\n");
		return false;
	}else if((avail_to_fill - 60) < (size + 4)){ //leaving atleast 60 bytes exclusively for ack messages
		printf("error: send(): not enough space in buffer");
		return false;
	}else{
		send[end++] = id;
		if( end == BUFFER_SIZE ) end = 0; //reached end of buffer
		send[end++] = size + 2;
		if( end == BUFFER_SIZE ) end = 0; //reached end of buffer
		send[end++] = size + 1;
		if( end == BUFFER_SIZE ) end = 0; //reached end of buffer
		send[end++] = dataType;
		if( end == BUFFER_SIZE ) end = 0; //reached end of buffer

		int i;
		for(i = 0; i < size; i++) {
			send[end] = data[i];

			end++;
			if( end == BUFFER_SIZE ) end = 0; //reached end of buffer
		}

		End = end;
	}
	return true;
}

unsigned int get_buffer_space() {
	int start = Start;
	int end = End;
	return start > end ? start - end - 1 : start + BUFFER_SIZE - end - 1;
}

int main()
{
	uart_init();
	printf("uart init\n");

	while(set_name_i != 4);

	sendNameAll();

	return 0;
}
