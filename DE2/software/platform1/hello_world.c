
#include <stdio.h>
#include "altera_up_avalon_rs232.h"
#include <string.h>

int main()
{
		int i;
		unsigned char data;
		unsigned char parity;

        printf("UART Initialization\n");
        alt_up_rs232_dev* uart = alt_up_rs232_open_dev(RS232_0_NAME);

        printf("Clearing read buffer to start\n");
		while (alt_up_rs232_get_used_space_in_read_FIFO(uart)) {
			alt_up_rs232_read_data(uart, &data, &parity);
		}

		while(1){
			printf("waiting for message from android device\n");
			//need to wait for at least 2 bytes
			//first is the ID, second is the amount of bytes
			while (alt_up_rs232_get_used_space_in_read_FIFO(uart) < 2);

			// First byte is the number of characters in our message
			alt_up_rs232_read_data(uart, &data, &parity);
			printf("receive from id: %i\n", (unsigned int)data);
			alt_up_rs232_read_data(uart, &data, &parity);
			int num_to_receive = (int)data;

			//wait for all the data to arrive
			while (alt_up_rs232_get_used_space_in_read_FIFO(uart) < num_to_receive);

			//receiveing data
			unsigned char buffer[128];
			printf("About to receive %d characters:\n", num_to_receive);

			for (i = 0; i < num_to_receive; i++) {
				while (alt_up_rs232_get_used_space_in_read_FIFO(uart) == 0)
					;
				alt_up_rs232_read_data(uart, &data, &parity);

				buffer[i] = data;
				printf("%c",buffer[i]);
			}
			printf("\n");

			//sends an ack
			alt_up_rs232_write_data(uart, 0xFF); //echo to all
			alt_up_rs232_write_data(uart, 1);//middleman discards this
			alt_up_rs232_write_data(uart, 0);

			// Start with the number of bytes in our message
			//sending data
			alt_up_rs232_write_data(uart, 0xFF); //echo to all
			alt_up_rs232_write_data(uart, (unsigned char) num_to_receive+1);//middleman discards this
			alt_up_rs232_write_data(uart, (unsigned char) num_to_receive);

			// Now send the actual message to the Middleman
			for (i = 0; i < num_to_receive; i++) {
				alt_up_rs232_write_data(uart, buffer[i]);
				printf("%c",buffer[i]);
			}


			printf("\n");
			printf("Message Echo Complete\n");
		}
        return 0;
}
