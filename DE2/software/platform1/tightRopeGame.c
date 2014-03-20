/*
 * tightRopeGame.c
 *
 *  Created on: Mar 19, 2014
 *      Author: Owen
 */

//to do list
// background
// winning/losing screen(sagent)
// timer for game
// image for game
// interrupt????
// android comm
// drawing stuff code finalization?
// menu connection



#include <stdio.h>
#include <stdlib.h>
#include "draw.h"
#include "bomb.h"
#include "rs232com.h"
#include "tightRopeGame.h"

int RSL = 45;
int LSL = 135;
int mid = 90;
int right = 45;
int left = 315;
int axis[4];
int player_fall[4];
float ANDRIOD_INPUT[4]; ///????
int wind;


object object;


void tight_rope_game()
{
	//buffer init stuff
	pixel_buffer = alt_up_pixel_buffer_dma_open_dev("/dev/pixel_buffer_dma");

	master* m = master_init();
	//master* m;


	// Set the 1st buffer address
	alt_up_pixel_buffer_dma_change_back_buffer_address(pixel_buffer,pixel_buffer_addr1);
	// Swap buffers – we have to swap because there is only an API function
	// to set the address of the background buffer.
	alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
	while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
	// Set the 2nd buffer address
	alt_up_pixel_buffer_dma_change_back_buffer_address(pixel_buffer,pixel_buffer_addr2);
	// Clear both buffers (this makes all pixels black)
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 1);
	alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
	while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 1);

	//interrupt init stuff (for object writing)
	//TIMERPERIOD
	int timer_period = 1 * 500000;
	IOWR_16DIRECT(TIMER_0_BASE, 8, timer_period & 0xFFFF); //writes the period to the hardware timer
	IOWR_16DIRECT(TIMER_0_BASE, 12, timer_period >> 16);
	IOWR_16DIRECT(TIMER_0_BASE, 4, 1 << 3); //stop timer
	alt_irq_register(TIMER_0_IRQ,NULL,(void*)handle_timer_interrupts);//registers function to a specific IRQ
	IOWR_16DIRECT(TIMER_0_BASE, 4, 0x5); //start timer
	printf("past interrupt");

	//load bitmap files

	bmp* 45 = read_bmp("45.bmp");
	bmp* 55 = read_bmp("55.bmp");
	bmp* 60 = read_bmp("60.bmp");
	bmp* 65 = read_bmp("65.bmp");
	bmp* 70 = read_bmp("70.bmp");
	bmp* 75 = read_bmp("75.bmp");
	bmp* 80 = read_bmp("80.bmp");
	bmp* 85 = read_bmp("85.bmp");
	bmp* 90 = read_bmp("90.bmp");
	bmp* 95 = read_bmp("95.bmp");
	bmp* 100 = read_bmp("100.bmp");
	bmp* 105 = read_bmp("105.bmp");
	bmp* 110 = read_bmp("110.bmp");
	bmp* 115 = read_bmp("115.bmp");
	bmp* 120 = read_bmp("120.bmp");
	bmp* 125 = read_bmp("125.bmp");
	bmp* 130 = read_bmp("130.bmp");
	bmp* 135 = read_bmp("135.bmp");

	object.x[0] = 10;
	object.x[1] = 90;
	object.x[2] = 170;
	object.x[3] = 250;

	object.y[0] = 120;
	object.y[1] = 120;
	object.y[2] = 120;
	object.y[3] = 120;

	object.length[0]= 80;
	object.length[1]= 80;
	object.length[2]= 80;
	object.length[3]= 80;

	//waits until everybosy is connected
	while(set_name_i != 1);
	sendNameAll();

	while(1) //superloop
	{
		//menu should point to here to start the game
		//game start grahics?
		// need to find player and background
		// position is updated by interrupt
		//need to fine bmp for game
		// need a timer for the game
		//draw_object(axis);
		//clear_object(axis);
		int num = 0;
		for(num = 0; num < 4; num++)
		{
			switch(axis[num])
			{
			case 45:
				draw_object(pixel_buffer, object, 45, num);
			case 50:
				draw_object(pixel_buffer, object, 50, num);
			case 55:
				draw_object(pixel_buffer, object, 55, num);
			case 60:
				draw_object(pixel_buffer, object, 60, num);
			case 65:
				draw_object(pixel_buffer, object, 65, num);
			case 70:
				draw_object(pixel_buffer, object, 70, num);
			case 75:
				draw_object(pixel_buffer, object, 75, num);
			case 80:
				draw_object(pixel_buffer, object, 80, num);
			case 85:
				draw_object(pixel_buffer, object, 85, num);
			case 90:
				draw_object(pixel_buffer, object, 90, num);
			case 95:
				draw_object(pixel_buffer, object, 95, num);
			case 100:
				draw_object(pixel_buffer, object, 100, num);
			case 105:
				draw_object(pixel_buffer, object, 105, num);
			case 110:
				draw_object(pixel_buffer, object, 110, num);
			case 115:
				draw_object(pixel_buffer, object, 115, num);
			case 120:
				draw_object(pixel_buffer, object, 120, num);
			case 125:
				draw_object(pixel_buffer, object, 125, num);
			case 130:
				draw_object(pixel_buffer, object, 130, num);
			case 135:
				draw_object(pixel_buffer, object, 135, num);

			}

		}

			int i;
			int k = 0;
		for(i = 0; i < 4 ; i++)// if player fall
		{
			if(player_fall[i] == 1)
			{
			//draw ending screen;
			//draw_object();
			//end game for player i
			//end andriod como for player i
			k++;
			}
			if(k == 3)
			{
				// end game
				// show victory screen for player who didnt lose
				// option to go back to menu
				break;
			}
		}

	}
    return 0;
}
int acc(int axis)
{
	int Rlimit  = 70; // if the angle is higher or lower than a certain degree, acc the changes
	//might need higher degree of changes
	int Llimit = 110;
	if(axis < Rlimit)
		return axis - 10;
	else if( axis < Llimit )
		return axis + 10;
	else
		return axis;
}

void wind(int i , int dir) //wind pushes player one way or another,
{
	if(dir == 1)// some random fact that affects the way wind pushes
	{
		 axis[i] - 10;
	}
	else
	{
		 axis[i] + 10;
	}

}


void cal_fall(int axis, int i) // check if the angle of over a limit, if it is ends the game
{

	if(axis > RSL || axis < LSL)
		player_fall[i] = 1;
	else
		player_fall[i] = 0;


}
void handle_timer_interrupts(){ //Interrupt for when hardware timer counts to zero
	//printf("successfully interrupted\n");


	//update the info, checks
	//axis[4] = ANDRIOD_INPUT[4];


	int i;
	for(i = 0; i< 4; i++)
	{
		if(axis[i] > RSL && axis[i] < mid )
			axis[i] -= 5;
		else if(axis[i] < LSL && axis[i] > mid)
			axis[i] +=5;

		if(wind == 5000)
			wind(i,1);
		else if(wind == 9999)
		{
			wind(i,0);
			wind = 0;
		}
		//axis[i] = wind(axis[i]);
		axis[i] = acc(axis[i]);
		cal_fall(axis[i], i);

	}





	IOWR_16DIRECT(TIMER_0_BASE,0,0); //needed to show that interrupt finished executing
	IOWR_16DIRECT(TIMER_0_BASE,4,0x5); //restarts the hardware timer before exiting the isr
	return;
}

void dump_data(unsigned char id, unsigned char *data){
	if(id==p.pid0){
		ANDROID_INPUT[0] = *((float*)data);
	}else if(id==p.pid1){
		ANDROID_INPUT[1] = *((float*)data);
	}else if(id==p.pid2){
		ANDROID_INPUT[2] = *((float*)data);
	}else{
		ANDROID_INPUT[3] = *((float*)data);
	}
}

