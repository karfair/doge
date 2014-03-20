/*
 * sd_audio.h
 *
 *  Created on: Mar 19, 2014
 *      Author: Owen
 */

#ifndef SD_AUDIO_H_
#define SD_AUDIO_H_

#include <stdio.h>
#include <stdlib.h>
#include "Altera_UP_SD_Card_Avalon_Interface.h"
#include "sys/alt_irq.h"
#include "altera_up_avalon_audio_and_video_config.h"
#include "altera_up_avalon_audio.h"
#include "system.h"
#include "io.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"

//For new faster audio write instruction
#include "altera_up_avalon_audio_regs.h"

#define BUFFER_SIZE 95*16 //audio
#define LEVEL_SIZE 400

#define MAX_PLAYER 15
#define MAX_NAME_LENGTH 15
#define MAX_LEVEL 10

typedef short int sdf;

typedef struct{

	short int cir_buffer[BUFFER_SIZE];

	int start;
	int end;

	bool sfx_flag;
	short int* sfx_ptr;
	int sfx_size;
	int sfx_count;

	bool sfx_flag1;
	short int* sfx_ptr1;
	int sfx_size1;
	int sfx_count1;

	bool finished; //song end
	alt_up_audio_dev *audio_dev;

	sdf wav; //file handle to the background music
} audio_buffer;

typedef struct{
	int obj_type[LEVEL_SIZE];
	unsigned int wait_time[LEVEL_SIZE];
	int height[LEVEL_SIZE];

	int x[LEVEL_SIZE];
	int y[LEVEL_SIZE];

	int color[LEVEL_SIZE];

	int size; //total number of objects
	int obj_index;
} level;

typedef struct{
	char names[MAX_PLAYER][MAX_NAME_LENGTH+1];
	int scores[MAX_PLAYER][MAX_LEVEL];

	int num_player;
	int num_level;
} player_data;



void reset_audio_buffer(audio_buffer* ab);

void av_config_setup();
void audio_isr(void *context, unsigned int id);

void sd_init();
sdf init_read_wav(char *name);

void refill_buffer(audio_buffer *ab, char* music_to_loop);
void fill_buffer(audio_buffer *ab);

sdf sd_fopen(char* name);
void sd_fclose(sdf handle);
short int sd_readbyte(sdf handle);

//optimization
void audio_write_fifo(alt_up_audio_dev *audio, short int *buf, int len);

void preload_sfx(char* name, audio_buffer* ab);
void preload_sfx1(char* name, audio_buffer* ab);

void free_sfx(audio_buffer* ab);
void free_sfx1(audio_buffer* ab);

//read lvl
void sd_read_level(char* filename, level* l );

void sd_read_players(char* filename, player_data* p, int num_level );
void sd_write_players(char* filename, player_data* p ) ;

typedef struct{
	int x,y;
	short int** pic; //array of ptr to a horz line
						//each line is sized x
						//array of ptr is sized y
} bmp;

//.bmp file
void free_bmp(bmp *b);
bmp* read_bmp(char* name);
void draw_bmp(alt_up_pixel_buffer_dma_dev * pixel_buffer, int x, int y, bmp * b);

void clear_loc(alt_up_pixel_buffer_dma_dev *pixel_buffer, int x0, int y0, int x1, int y1, bmp* b) ;

void unrolled_parallax_draw(alt_up_pixel_buffer_dma_dev *pixel_buffer, bmp* b);


#endif /* SD_AUDIO_H_ */
