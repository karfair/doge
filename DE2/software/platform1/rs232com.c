#include <stdio.h>
#include <string.h>
#include "altera_up_avalon_rs232.h"
#include "sys/alt_irq.h"
#include "io.h"

#include "rs232com.h"

#define BUFFER_SIZE 1024 //sending buffer
#define TIMER_PERIOD 200000 //200k ticks -> 4mS

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
	IOWR_16DIRECT(TIMER_0_BASE,0,0); //needed to show that interrupt finished executing

	unsigned char i, parity;

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
			}else {
				break;
			}
		}
		printf("1");
		//wait for all the data to arrive
		if(alt_up_rs232_get_used_space_in_read_FIFO(uart) < size ) {
			break;
		}
		printf("2");
		//receiving data into buffer
		for (i = 0; i < size; i++) {
			alt_up_rs232_read_data(uart, buffer+i, &parity);
			printf("%c",buffer[i]);
		}

		//turn off hasReadHeader
		hasReadHeader = false;

		//send ack
		if(!send_ack(id)){
			printf("ack not sent to id:%i! prepared to die...\n", (int)id);
		}

		//TODO: Modify stuff here to do different things for different data type
		switch(dataType){
			case 0:
				//speed test mode, do nothing
				break;
			case 1:
				send_data(id, dataType, size, buffer);
				break;
		}
	}

	//sending data

	//create a copy of the global variables
	int start = Start;
	int end = End;

	int queue_size = start < end ? end - start : end + BUFFER_SIZE - start; //how much is there to send
	queue_size--; //start != end
	int FIFO_avail = alt_up_rs232_get_available_space_in_write_FIFO(uart); //how much can we send?
	int amount_to_send = FIFO_avail > queue_size ? queue_size : FIFO_avail; //send the lower amount

	//printf("transmit\n");
	for(i = 0; i < amount_to_send; i++) {
		start++;
		if(start == BUFFER_SIZE) start = 0;
		alt_up_rs232_write_data(uart, send[start]); //sends the actual data
		printf("\n%c,%X,%i",send[start],send[start],send[start]);
	}
	//printf("\n");

	Start = start; //sets the global

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
	int i = 0;
	while(1){
		i++;
		if(i % 5000000 == 0){
			printf("content:");
			int j;
			for(j=0;j<50;j++)
				printf("%c",buffer[j]);
			printf(" id: %i size: %i datatype: %i start: %i end: %i\n", id,size,dataType,Start,End);
		}
	}
	return 0;
}
