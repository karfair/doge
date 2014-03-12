//draw.h
#ifndef draw_H_
#define draw_H_
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"
#include "object_spawner.h"

int speed = 1;

extern object co;
extern object spikes;
extern object box_3;
extern object box_1;

extern bmp * title;
extern bmp * menu;
extern bmp * selA;
extern bmp * selB;
extern bmp * selC;
extern bmp * dead;
extern bmp * win;
extern bmp * name;
extern bmp * instr;
extern bmp * dcol;


extern volatile int game_state;
extern volatile int highlighted_item;
extern volatile int highlighted_level;
extern volatile int priviledge;

alt_up_pixel_buffer_dma_dev* pixel_buffer;

extern alt_up_char_buffer_dev *char_buffer;

unsigned int pixel_buffer_addr1 = PIXEL_BUFFER_BASE;
unsigned int pixel_buffer_addr2 = PIXEL_BUFFER_BASE + PIXEL_BUFFER_SPAN/2; //320*240*2

void update_highlight();

void update_object( object* box, int num ){

	int* draw = box->draw;

	if(box->x[num] == 340) return;

	if(box->x[num] == 319 )
			{
				draw[num] = 1;
				box->x[num] -= speed;
			}

		else if((319 - box->x[num]) <= box->length[num])
			{
				box->temp_length[num] = 319 - box->x[num];
				draw[num] = 0;
				box->x[num] -= speed;
			}
		else if( box->x[num] <= 2)
		{

			if( box->temp_length[num] <= 0)
			{
				box->x[num] = 340;
				box->temp_length[num] = -1;
				draw[num] = 1;
			}
			else
			{
				box->temp_length[num] -= speed;
				draw[num] = 0;
				box->dec[num] +=1;
			}
		}

		else
		{
			//box.temp_length = box.length;
			box->x[num] -= speed;
			draw[num] = 0;
			box->dec[num] = 0;
		}
}

void draw_object(alt_up_pixel_buffer_dma_dev *pixel_buffer, object object, bmp* b, int num) {
	unsigned int i,j,k,l;
	unsigned int addr = pixel_buffer->back_buffer_start_address;
	unsigned int limit_x = pixel_buffer->x_resolution;
	short int** pic = b->pic;
	short int temp;

	if( object.draw[num] == 1) return;

	limit_x = limit_x << 1;
	addr += object.y[num] * (limit_x);

	for(i = object.y[num], k = 0; i < object.y[num] + b->y; i++, k++) {
		for (j = object.x[num], l = object.dec[num]; j < object.temp_length[num]+ object.x[num]; j++, l++)
		{
			//alternative pixel drawing function
			//alt_up_pixel_buffer_dma_draw(pixel_buffer, pic[k][l], l, k);

			//draw pixel
			temp = pic[k][l];
			if(pic[k][l] != 0x789) //if pure white, don't draw
				IOWR_16DIRECT(addr, j << 1, temp);
		}
		addr += limit_x;

		//if(V_VERBOSE) printf("finished line %i\n", k);
	}
	//if(V_VERBOSE) printf("ended bmp draw\n");


}

void clear_object(alt_up_pixel_buffer_dma_dev *pixel_buffer, object box, bmp* b, int num)
{
	int x0, x1;

	x0 = box.x[num] - 7 < 0 ? 0 :  box.x[num] - 7;
	x1 = box.x[num] + box.temp_length[num] + 7 > 319 ? 319 : box.x[num] + box.temp_length[num] + 7;
	//y0 = box.y[num] - 1 <
	//y1 =
//	clear_loc(pixel_buffer, box.x[num] , box.y[num] - 5, box.x[num]+box.temp_length[num], box.y[num]+box.height[num] + 5,b);

	clear_loc(pixel_buffer, x0, box.y[num], x1, box.y[num]+box.height[num],b);

}
void clear_doge(alt_up_pixel_buffer_dma_dev *pixel_buffer,int x ,int y, bmp* b)
{
	int y1;

	y1 = y + 40 + 7 > 239 ? 239 : y + 40 + 7;

	clear_loc(pixel_buffer, x,y-30, x+60, y1 ,b);
}

void draw_menu(int state){
	if (state == 0) {
			//game_state == 0 is TITLE SCREEN
			draw_bmp(pixel_buffer,0,0,title);
			alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
			while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
			draw_bmp(pixel_buffer,0,0,title);
			alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
			while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));

			free_bmp(title);

	}else if (state == 1){
			//game_state == 1 is MAIN MENU
			draw_bmp(pixel_buffer,0,0,menu);
			alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
			while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
			draw_bmp(pixel_buffer,0,0,menu);
			alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
			while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));

			update_highlight();

	} else if (state == 2){
			//game_state == 2 is INSTRUCTIONS
			draw_bmp(pixel_buffer,0,0,instr);
			alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
			while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
			draw_bmp(pixel_buffer,0,0,instr);
			alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
			while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));

	} else if (state == 3){
			//game_state == 3 is LEVEL SCREEN
			if (priviledge == 1){
					draw_bmp(pixel_buffer,0,0,selA);
					alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
					while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
					draw_bmp(pixel_buffer,0,0,selA);
					alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
					while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));

			} else if (priviledge == 2) {
					draw_bmp(pixel_buffer,0,0,selB);
					alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
					while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
					draw_bmp(pixel_buffer,0,0,selB);
					alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
					while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));

			} else if (priviledge == 3) {
					draw_bmp(pixel_buffer,0,0,selC);
					alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
					while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
					draw_bmp(pixel_buffer,0,0,selC);
					alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
					while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
			}

			update_highlight();

	} else if (state == 4){
			//game_state == 4 is DOGE COLOUR
			draw_bmp(pixel_buffer,0,0,dcol);
			alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
			while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
			draw_bmp(pixel_buffer,0,0,dcol);
			alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
			while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));

	} else if (state == 5){
			//game_state == 5 is PLAY GAME

	} else if (state == 6){
			//game_state == 6 is LOSE
			draw_bmp(pixel_buffer,80,60,dead);
			alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
			while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
			draw_bmp(pixel_buffer,80,60,dead);
			alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
			while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));

			if (priviledge == 2) priviledge = 3;
			else priviledge = 2;

	} else if (state == 7){
			//game_state == 7 is WIN
			draw_bmp(pixel_buffer,80,60,win);
			alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
			while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
			draw_bmp(pixel_buffer,80,60,win);
			alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
			while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));

			if (priviledge == 2) priviledge = 3;
			else priviledge = 2;

	} else if (state == 9){
		//game_state == 9 is NAME
		draw_bmp(pixel_buffer,0,0,name);
		alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
		while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
		draw_bmp(pixel_buffer,0,0,name);
		alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
		while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));

		free_bmp(name);
	}

	return;
}

void update_highlight(){
	short int clr1, clr2, clr3;
	clr1 = clr2 = clr3 = 0x0000;

	//printf("Update Highlight\n\n");

	if (game_state == 1) {
			if (highlighted_item == 2) {
					//printf("1\n\n");
					// Draw the three underlined selections in black, highlight middle
					alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 6, 180, 154, 183, 0xA000, 1);
					alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 83, 135, 230, 138, 0x0000, 1);
					alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 166, 180, 313, 183, 0x0000, 1);
			} else if (highlighted_item == 3) {
					//printf("2\n\n");
					// Draw the three underlined selections in black, highlight middle
					alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 6, 180, 154, 183, 0x0000, 1);
					alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 83, 135, 230, 138, 0xA000, 1);
					alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 166, 180, 313, 183, 0x0000, 1);

			} else if (highlighted_item == 4) {
					//printf("3\n\n");
					// Draw the three underlined selections in black, highlight middle
					alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 6, 180, 154, 183, 0x0000, 1);
					alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 83, 135, 230, 138, 0x0000, 1);
					alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 166, 180, 313, 183, 0xA000, 1);
			}
	} else if (game_state == 3) {

			if (priviledge == 1){
					alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 28, 197, 99, 200, 0xA000, 1);

			} else if (priviledge == 2) {

					if (highlighted_level == 1) {
							clr1 = 0xA000;
							clr2 = 0x0000;
					} else if (highlighted_level == 2) {
							clr1 = 0x0000;
							clr2 = 0xA000;
					}

					alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 28, 197, 99, 200, clr1, 1);
					alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 125, 197, 195, 200, clr2, 1);


			} else if (priviledge == 3){

					if (highlighted_level == 1) {
							clr1 = 0xA000;
							clr2 = 0x0000;
							clr3 = 0x0000;
					} else if (highlighted_level == 2) {
							clr1 = 0x0000;
							clr2 = 0xA000;
							clr3 = 0x0000;
					} else if (highlighted_level == 3) {
							clr1 = 0x0000;
							clr2 = 0x0000;
							clr3 = 0xA000;
					}

					alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 28, 197, 99, 200, clr1, 1);
					alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 125, 197, 195, 200, clr2, 1);
					alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 221, 197, 293, 200, clr3, 1);
			}
	}

	alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
	while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));

	return;
}

#endif
