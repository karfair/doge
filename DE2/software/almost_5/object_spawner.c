/*
 * object_spawner.c
 *
 *  Created on: 2014-01-31
 *      Author: Kit
 */
#include <time.h>
#include <stdlib.h>
#include "object_spawner.h"
#include "col.h"

#define SFX "las.wav"
#define BG_MUSIC "a/abcd.wav"
#define LEVEL_LOCATION "lvl/1.txt"

extern volatile int highlighted_level;

extern object box;
extern object co;
extern object spikes;
extern object box_3;
extern object box_1;
extern object flag;

int count = 0;

bool first_platform = true;
int prev_y;
int inc;

extern void sd_fclose(sdf handle);
extern alt_up_pixel_buffer_dma_dev* pixel_buffer;

extern int p_shift;

master* master_init() {
	//init sd
	sd_init();

	//allocates memory
	master* m = (master*)malloc(sizeof(master));
	m->l = malloc(sizeof(level));
	m->c = malloc(sizeof(charac));
	//m->o = malloc(sizeof(object));
	m->o = &box;
	m->co = &co;
	m->spikes = &spikes;
	m->box_3 = &box_3;
	m->box_1 = &box_1;
	m->flag = &flag;


	srand(time(NULL));


	m->ab = malloc(sizeof(audio_buffer));

	//audio init
	av_config_setup();
	m->ab->audio_dev = alt_up_audio_open_dev(AUDIO_NAME); //audio setup

	reset_audio_buffer(m->ab); //init circular buffer
	preload_sfx(SFX, m->ab); //preloads sfx
	//TODO: fix sfx
	preload_sfx1("coin.wav", m->ab);

	alt_irq_register(AUDIO_IRQ,m->ab,(alt_isr_func)audio_isr);

	return m;
}

void game_start(master* m, bmp* bg, char* level_to_read, char* bg_music) {
	p_shift = 0;
	first_platform = true;
	inc = 0;
	//background drawing
	clear_loc(pixel_buffer,0,0,319,239,bg);
	alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
	while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
	clear_loc(pixel_buffer,0,0,319,239,bg);
	alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
	while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));



	m->ab->wav = init_read_wav(bg_music); //reads wav file into buffer
	fill_buffer(m->ab);

	sd_read_level(level_to_read, m->l);
	object_spawn_init(m);
	init_char_update(m);

	//start timer
	IOWR_16DIRECT(TIMER_0_BASE, 4, 0x5); //start timer

	//enable audio interrupt
	alt_up_audio_enable_write_interrupt(m->ab->audio_dev);

	//enable character interrupt
	IOWR_16DIRECT(TIMER_2_BASE, 4, 0x5);

	//enable object spawning interrupt
	IOWR_16DIRECT(TIMER_1_BASE, 4, 0x5);
}

void game_reset(master* m) {
	alt_up_audio_disable_write_interrupt(m->ab->audio_dev);
	IOWR_16DIRECT(TIMER_0_BASE, 4, 1 << 3);
	IOWR_16DIRECT(TIMER_1_BASE, 4, 1 << 3);
	IOWR_16DIRECT(TIMER_2_BASE, 4, 1 << 3);


	sd_fclose(m->ab->wav);
	reset_audio_buffer(m->ab);
	alt_up_audio_reset_audio_core(m->ab->audio_dev);
}

void object_spawn_init(master *mstr){

	//init timer1 and interrupt


	//set base values
	init_obj(mstr->o);
	init_obj(mstr->co);
	init_obj(mstr->spikes);
	init_obj(mstr->box_3);
	init_obj(mstr->box_1);
	init_obj(mstr->flag);

	IOWR_16DIRECT(TIMER_1_BASE, 8, mstr->l->wait_time[mstr->l->obj_index] & 0xFFFF); //writes the period to the hardware timer
	IOWR_16DIRECT(TIMER_1_BASE, 12, mstr->l->wait_time[mstr->l->obj_index] >> 16);
	IOWR_16DIRECT(TIMER_1_BASE, 4, 1 << 3); //stop timer

	alt_irq_register(TIMER_1_IRQ,mstr,(void*)object_spawn_interrupt);//registers function to a specific IRQ

	//IOWR_16DIRECT(TIMER_1_BASE, 4, 0x5);	//starts timer with interrupt

	return;
}

void init_obj(object* o){
	o->size = 0;
	int i;
	for(i=0;i<OBJECT_SIZE;i++){
		o->temp_length[i] = -1;
		o->x[i] = 340;
		o->draw[i] = 1;
		o->dec[i] = 0;
	}
}

void up_obj(object* o, level* l){
	int i, index = 0;
	//if # of objects in obj struct is OBJECT_SIZE or over, program finds and index with an invalid value and replaces that
	if(o->size >= OBJECT_SIZE){
		for(i = 0; i<OBJECT_SIZE; i++){
			//printf("%i ", o->temp_length[i]);
			if(o->temp_length[i]==-1) {
				index = i;
				o->temp_length[i] = 0;
				break;
			}
		}
	}
	else
		index = o->size;

	o->color[index] = l->color[l->obj_index];
	o->y[index] = l->height[l->obj_index];
	o->length[index] = l->x[l->obj_index];
	o->height[index] = l->y[l->obj_index];
	o->x[index] = 319;

	//increment # of objects and level index
	//printf("count: %i y:%i l:%i h:%i x:%i\n", count++,o->y[index],o->length[index],o->height[index],o->x[index]);

	o->size++;
}

void object_spawn_interrupt( void* context ){
	//printf("entered interrupt\n");
	IOWR_16DIRECT(TIMER_1_BASE,0,0); //needed to show that interrupt finished executing

	master *mstr_spwn = (master*)context;

	if(highlighted_level == 3){
		object* o = mstr_spwn->o;
		object* coin = mstr_spwn->co;

		int i, index = 0;
		//if # of objects in obj struct is OBJECT_SIZE or over, program finds and index with an invalid value and replaces that
		if(o->size >= OBJECT_SIZE){
			for(i = 0; i<OBJECT_SIZE; i++){
				//printf("%i ", o->temp_length[i]);
				if(o->temp_length[i]==-1) {
					index = i;
					o->temp_length[i] = 0;
					break;
				}
			}
		}
		else
			index = o->size;

		int temp5;
		if(first_platform == true){
			temp5 = 20000000;
			first_platform = false;
			o->color[index] = -1;
			o->y[index] = 220;
			o->length[index] = 80;
			o->height[index] = 13;
			o->x[index] = 319;
			prev_y = 220;
		}else{
			temp5 = 0;
			inc += 500000;
			temp5 -= inc;
			o->color[index] = rand() % 2 ;
			int temp = rand() % 101 - 50;
			prev_y += temp;
			if( prev_y > 225 ) prev_y = 225;
			if( prev_y < 125 ) prev_y = 125;
			o->y[index] = prev_y;
			o->length[index] = rand()%20 + 40;
			o->height[index] = 13;
			o->x[index] = 319;
		}

		//increment # of objects and level index
		//printf("count: %i y:%i l:%i h:%i x:%i\n", count++,o->y[index],o->length[index],o->height[index],o->x[index]);

		o->size++;


		//if # of objects in obj struct is OBJECT_SIZE or over, program finds and index with an invalid value and replaces that
		if(coin->size >= OBJECT_SIZE){
			for(i = 0; i<OBJECT_SIZE; i++){
				//printf("%i ", o->temp_length[i]);
				if(coin->temp_length[i]==-1) {
					index = i;
					coin->temp_length[i] = 0;
					break;
				}
			}
		}
		else
			index = coin->size;

		coin->y[index] = prev_y-14;
		coin->length[index] = 10;
		coin->height[index] = 14;
		coin->x[index] = 319;

		coin->size++;

		temp5 += (40000000 + rand() % 20000000);
		 //writes the period to the hardware timer
		IOWR_16DIRECT(TIMER_1_BASE, 8, temp5 & 0xFFFF);
		IOWR_16DIRECT(TIMER_1_BASE, 12, temp5 >> 16);

		IOWR_16DIRECT(TIMER_1_BASE,4,0x5); //restarts the hardware timer before exiting the isr
		return;
	}
	else{
		again:
		//printf("obji: %i\n",mstr_spwn->l->obj_index);
		//printf("size: %i\n",mstr_spwn->l->size);
		//pass properties of object type to object struct
		switch(mstr_spwn->l->obj_type[mstr_spwn->l->obj_index])
		{
		case 0:
			up_obj(mstr_spwn->o, mstr_spwn->l);
			break;
		case 1:
			up_obj(mstr_spwn->co, mstr_spwn->l);
			break;
		case 2:
			up_obj(mstr_spwn->spikes, mstr_spwn->l);
			break;
		case 3:
			up_obj(mstr_spwn->box_3, mstr_spwn->l);
			break;
		case 4:
			up_obj(mstr_spwn->box_1, mstr_spwn->l);
			break;
		case 5:
			up_obj(mstr_spwn->flag, mstr_spwn->l);
			break;
		default:
			break;
		}

		//if object spawns at the same time
		mstr_spwn->l->obj_index++;
		if(mstr_spwn->l->wait_time[mstr_spwn->l->obj_index] == 0) goto again;

		//if the obj_index of a level is greater than or equal to level size, return without adding an object
		if( mstr_spwn->l->obj_index >= mstr_spwn->l->size){
				return;
		}


		 //writes the period to the hardware timer
		IOWR_16DIRECT(TIMER_1_BASE, 8, mstr_spwn->l->wait_time[mstr_spwn->l->obj_index] & 0xFFFF);
		IOWR_16DIRECT(TIMER_1_BASE, 12, mstr_spwn->l->wait_time[mstr_spwn->l->obj_index] >> 16);

		IOWR_16DIRECT(TIMER_1_BASE,4,0x5); //restarts the hardware timer before exiting the isr
		return;

	}

}


