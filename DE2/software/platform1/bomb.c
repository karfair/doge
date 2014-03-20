/*
 * bomb.c
 *
 *  Created on: 2014-03-19
 *      Author: Phil
 */

#include "bomb.h"
#include "rs232com.h"

players p;
volatile int set_name_i = 0;

void setName(unsigned char id, unsigned char size, unsigned char *name){
	int i;
	switch(set_name_i){
	case 0:
		for(i = 0; i < size; i++){
			p.p0name[i] = name[i];
		}
		p.size0 = size;
		p.pid0 = id;
		break;
	case 1:
		for(i = 0; i < size; i++){
			p.p1name[i] = name[i];
		}
		p.size1 = size;
		p.pid1 = id;
		break;
	case 2:
		for(i = 0; i < size; i++){
			p.p2name[i] = name[i];
		}
		p.size2 = size;
		p.pid2 = id;
		break;
	case 3:
		for(i = 0; i < size; i++){
			p.p3name[i] = name[i];
		}
		p.size3 = size;
		p.pid3 = id;
		break;
	}
	set_name_i++;
	printf("i: %i\n", set_name_i);
}

void sendNameAll(){
	printf("sending names\n");
	send_data(SEND_TO_ALL,12,p.size0,p.p0name);
	//send_data(SEND_TO_ALL,12,p.size1,p.p1name);
	//send_data(SEND_TO_ALL,12,p.size2,p.p2name);
	//send_data(SEND_TO_ALL,12,p.size3,p.p3name);
}
