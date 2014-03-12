//project main

#include <stdio.h>
#include <stdlib.h>

#include "draw.h"
#include "interrupt.h"
#include "keyboard.h"
#include "col.h"

object box;
object co;
object spikes;
object box_3;
object box_1;
object flag;

volatile int game_state = 0;
volatile int highlighted_item = 2;
volatile int highlighted_level = 1;

int p_shift = 0;
int p_counter = 0;

bmp * title;
bmp * menu;
bmp * name;
bmp * instr;
bmp * selA;
bmp * selB;
bmp * selC;
bmp * dead;
bmp * win;
bmp * dcol;

alt_up_char_buffer_dev *char_buffer;

extern char playername[12];


int main(){
	printf("NEW GAME!\n\n");

	volatile int old_game_state = 0;
	int doge = 0; //animation counter

	//buffer init stuff
	pixel_buffer = alt_up_pixel_buffer_dma_open_dev("/dev/pixel_buffer_dma");
	char_buffer = alt_up_char_buffer_open_dev("/dev/char_drawer");

	//phil's stuff
	master* m = master_init();

	printf("NEW GAME!\n\n");

	// Set the 1st buffer address
	alt_up_pixel_buffer_dma_change_back_buffer_address(pixel_buffer,pixel_buffer_addr1);
	// Swap buffers – we have to swap because there is only an API function
	// to set the address of the background buffer.
	alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
	while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
	// Set the 2nd buffer address
	alt_up_pixel_buffer_dma_change_back_buffer_address(pixel_buffer,pixel_buffer_addr2);

	printf("NEW GAME!\n\n");

	// Clear the screen
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 1);
	alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
	while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 1);
	alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
	while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));

	alt_up_char_buffer_clear(char_buffer);
	//Write some text
	alt_up_char_buffer_string(char_buffer, "LOADING...", 0, 0);

	//load bitmap files
	title = read_bmp("title.bmp");
	alt_up_char_buffer_string(char_buffer, "title.bmp", 0, 2);

	menu = read_bmp("menu.bmp");
	alt_up_char_buffer_string(char_buffer, "MENU.BMP", 0, 3);

	selA = read_bmp("selA.bmp");
	alt_up_char_buffer_string(char_buffer, "selA.bmp", 0, 4);

	selB = read_bmp("selB.bmp");
	alt_up_char_buffer_string(char_buffer, "selB.bmp", 0, 5);

	selC = read_bmp("selC.bmp");
	alt_up_char_buffer_string(char_buffer, "selC.bmp", 0, 6);

	dead = read_bmp("dead.bmp");
	alt_up_char_buffer_string(char_buffer, "dead.bmp", 0, 7);



	bmp * b 	= read_bmp("para1.bmp");
	alt_up_char_buffer_string(char_buffer, "para.bmp", 0, 8);

	bmp * doge0 = read_bmp("doge0.bmp");
	alt_up_char_buffer_string(char_buffer, "doge0.bmp", 0, 9);

	bmp * doge1 = read_bmp("doge1.bmp");
	alt_up_char_buffer_string(char_buffer, "doge1.bmp", 0, 10);

	bmp * doge2 = read_bmp("doge2.bmp");
	alt_up_char_buffer_string(char_buffer, "doge2.bmp", 0, 11);

	bmp * doge3 = read_bmp("doge3.bmp");
	alt_up_char_buffer_string(char_buffer, "doge3.bmp", 0, 12);

	bmp * flat  = read_bmp("flat.bmp");
	alt_up_char_buffer_string(char_buffer, "flat.bmp", 0, 13);

	bmp * coin  = read_bmp("coin.bmp");
	alt_up_char_buffer_string(char_buffer, "coin.bmp", 0, 14);

	bmp * spike = read_bmp("spike.bmp");
	alt_up_char_buffer_string(char_buffer, "spike.bmp", 0, 15);

	bmp * box1  = read_bmp("box1.bmp");
	alt_up_char_buffer_string(char_buffer, "box1.bmp", 0, 16);

	bmp * box3  = read_bmp("box3.bmp");
	alt_up_char_buffer_string(char_buffer, "box3.bmp", 0, 17);

	bmp * low  = read_bmp("low.bmp");
	alt_up_char_buffer_string(char_buffer, "low.bmp", 0, 18);

	bmp * flatb = read_bmp("flatb.bmp");
	alt_up_char_buffer_string(char_buffer, "flatb.bmp", 0, 19);

	bmp * flatr = read_bmp("flatr.bmp");
	alt_up_char_buffer_string(char_buffer, "flatr.bmp", 0, 20);

	bmp * blue = read_bmp("bstar.bmp");
	alt_up_char_buffer_string(char_buffer, "blue.bmp", 0, 21);

	bmp * red = read_bmp("rstar.bmp");
	alt_up_char_buffer_string(char_buffer, "red.bmp", 0, 22);

	bmp * flag_img = read_bmp("flag.bmp");
	alt_up_char_buffer_string(char_buffer, "flag.bmp", 0, 23);

	name = read_bmp("name.bmp");
	alt_up_char_buffer_string(char_buffer, "name.bmp", 0, 24);

	instr = read_bmp("instr.bmp");
	alt_up_char_buffer_string(char_buffer, "instr.bmp", 0, 25);

	dcol = read_bmp("dcol.bmp");
	alt_up_char_buffer_string(char_buffer, "dcol.bmp", 0, 26);

	win = read_bmp("win.bmp");
	alt_up_char_buffer_string(char_buffer, "win.bmp", 0,27);



	alt_up_char_buffer_clear(char_buffer);



	printf("NEW GAME!\n\n");

	//interrupt init stuff (for object writing)
	//TIMERPERIOD
	int timer_period = 1 * 500000;
	IOWR_16DIRECT(TIMER_0_BASE, 8, timer_period & 0xFFFF); //writes the period to the hardware timer
	IOWR_16DIRECT(TIMER_0_BASE, 12, timer_period >> 16);
	IOWR_16DIRECT(TIMER_0_BASE, 4, 1 << 3); //stop timer
	alt_irq_register(TIMER_0_IRQ,NULL,(void*)handle_timer_interrupts);//registers function to a specific IRQ
	//IOWR_16DIRECT(TIMER_0_BASE, 4, 0x5); //start timer

	//SET UP KEYBOARD INTERRUPT//
	ps2 = alt_up_ps2_open_dev(KEYBOARD_NAME);
	alt_up_ps2_init(ps2);
	alt_up_ps2_clear_fifo(ps2);
	//void* keyboard_control_register_ptr = (void*) (PS2_0_BASE + 4);
	alt_irq_register(PS2_0_IRQ, m, keyboard_ISR);
	alt_up_ps2_enable_read_interrupt(ps2);

	char sw = 0;
	char p_sw = 0;


/////////////////////////////////////////////////////////////////////////
	printf("NEW GAME!\n\n");
	//SUPERDUPERLOOP
	while (1){

		printf("old state:%i\nnew state: %i\n\n",old_game_state, game_state);

		draw_menu(game_state);					//update screen

		while (old_game_state == game_state);

		printf("old state:%i\nnew state: %i\n\n",old_game_state, game_state);	//only when entering a new menu
		alt_up_char_buffer_clear(char_buffer);


		//ENTER GAME LOOP
		if (game_state == 5){

			printf("START GAME! LEVEL: %i\n\n", highlighted_level);
			alt_up_char_buffer_string(char_buffer, playername, 10, 4);

			if (highlighted_level == 1)
			{
				free_bmp(b);
				b = read_bmp("para1.bmp");
				game_start(m,b,"lvl/1.txt","song1.wav");
			}
			else if (highlighted_level == 2) {
				free_bmp(b);
				b = read_bmp("bg2.bmp");
				game_start(m,b,"lvl/2.txt","a/abcd.wav");
			}
			else{
				free_bmp(b);
				b = read_bmp("bg3.bmp");
				game_start(m,b,"lvl/2.txt","a/nyan1.wav");
			}


			//collision loop
			while(!m->c->collide && !m->c->win){
				alt_up_char_buffer_string(char_buffer, "POINTS: ", 50, 4);
				char str[15];
				sprintf(str, "%d", m->c->points);
				alt_up_char_buffer_string(char_buffer, str, 58, 4);

				sw = IORD_8DIRECT(SWITCHES_BASE,0);
				IOWR_8DIRECT(LEDS_BASE,0,sw);

				if(sw == 1 && p_sw == 0){
					//m->ab->sfx_flag = 1;
					m->c->jump_pressed = 1;
				}

				p_sw = sw;

				//boxes
				int i;
				for( i= 0 ; i < OBJECT_SIZE ; i++) {
					if(m->o->color[i] == -1)
						draw_object(pixel_buffer, box, flat, i);
					else if(m->o->color[i] == 0)
						draw_object(pixel_buffer, box, flatb, i);
					else if(m->o->color[i] == 1)
						draw_object(pixel_buffer, box, flatr, i);
					draw_object(pixel_buffer, co, coin, i );
					draw_object(pixel_buffer, spikes, spike, i);
					draw_object(pixel_buffer, box_3, box3, i);
					draw_object(pixel_buffer, box_1, box1, i);
//								if(m->color_gates->color[i] == 1)
//									draw_object(pixel_buffer,cgates, rgate,i);
//								else if (m->color_gates->color[i] == 0)
//									draw_object(pixel_buffer,cgates, bgate,i);
				}
				//draws the win flag
				draw_object(pixel_buffer, flag, flag_img, 0);

				//Draw Doge
				if (m->c->ducking)
					draw_bmp(pixel_buffer,m->c->x - m->c->width, m->c->y - m->c->height,low);
				else{
					doge++;
					if(doge == 37) doge = 0;

					if( doge <9)
						draw_bmp(pixel_buffer,m->c->x - m->c->width, m->c->y - m->c->height,doge0);
					else if (doge <18)
						draw_bmp(pixel_buffer,m->c->x - m->c->width, m->c->y - m->c->height,doge1);
					else if (doge <27)
						draw_bmp(pixel_buffer,m->c->x - m->c->width, m->c->y - m->c->height,doge2);
					else
						draw_bmp(pixel_buffer,m->c->x - m->c->width, m->c->y - m->c->height,doge3);
				}


				//Draw Color Indicator
				if(m->c->color == 0)
					draw_bmp(pixel_buffer, m->c->x- m->c->width + 5, m->c->y - m->c->height - 10, blue);
				else
					draw_bmp(pixel_buffer,m->c->x- m->c->width + 5, m->c->y - m->c->height - 10, red);

				p_counter++;
				if(p_counter == 3){
					p_shift++;
					alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
					while(alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer))
						refill_buffer(m->ab, "a/abcd.wav");//refills the audio buffer
					unrolled_parallax_draw(pixel_buffer, b);

				}else if(p_counter == 4){ //if(p_counter == 1){
					alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
					while(alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer))
						refill_buffer(m->ab, "a/abcd.wav");//refills the audio buffer
					unrolled_parallax_draw(pixel_buffer, b);
					p_counter = 0;


				}else{
//					alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
//					while(alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer))
//						refill_buffer(m->ab, "a/abcd.wav");//refills the audio buffer
//					unrolled_parallax_draw(pixel_buffer, b);

					alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
					while(alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer))
						refill_buffer(m->ab, "a/abcd.wav");//refills the audio buffer

					int j; for( j = 0 ; j < OBJECT_SIZE ; j++) {
						clear_object(pixel_buffer,  box,  b, j);
						clear_object(pixel_buffer,  co,  b, j);
						clear_object(pixel_buffer,  spikes,  b, j);
						clear_object(pixel_buffer,  box_3,  b, j);
						clear_object(pixel_buffer,  box_1,  b, j);
					}
					clear_object(pixel_buffer,flag,b,0);

					//clear doge
					clear_doge(pixel_buffer, m->c->x - m->c->width, m->c->y - m->c->height , b);
					//clear_loc(pixel_buffer,m->c->x- m->c->width + 5, m->c->y - m->c->height - 10,m->c->x- m->c->width + 5 - 10, m->c->y - m->c->height - 20,b);

				}





			}
			alt_up_char_buffer_string(char_buffer, "POINTS: ", 50, 4);
			char str[15];
			sprintf(str, "%d", m->c->points);
			alt_up_char_buffer_string(char_buffer, str, 58, 4);

			printf("game exited\n");
			if(m->c->win)
				game_state  = 7;
			else
				game_state  = 6;

			highlighted_item = 3;

			game_reset(m);
		}



		//exit game mode, restart superduperloop in main menu
		old_game_state = game_state;
	}

	return 0;
}

void handle_timer_interrupts(){ //Interrupt for when hardware timer counts to zero
	int i; for( i = 0 ;  i < OBJECT_SIZE ;i++ ) {
		update_object(&co,i);
		update_object(&box,i);
		update_object(&spikes, i);
		update_object(&box_3 , i);
		update_object(&box_1, i );
	}
	update_object(&flag,0);//update winning flag

	IOWR_16DIRECT(TIMER_0_BASE,0,0); //needed to show that interrupt finished executing
	IOWR_16DIRECT(TIMER_0_BASE,4,0x5); //restarts the hardware timer before exiting the isr
	return;
}
