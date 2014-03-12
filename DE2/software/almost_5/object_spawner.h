/*
 * object_spawner.h
 *
 *  Created on: 2014-01-31
 *      Author: Kit
 */

#ifndef OBJECT_SPAWNER_H_
#define OBJECT_SPAWNER_H_

#include "sd_audio.h"

#define OBJECT_SIZE 10

typedef struct{
	int x,y; //bot right location
	int width, height;
	bool jumping,ducking,collide,grounded,jump_pressed,slow_fall,win;
	int j_counter,f_counter;
	int slow_count;
	int points;
	int color;
} charac;

typedef struct object{
	int y[OBJECT_SIZE];
	int x[OBJECT_SIZE];
	int length[OBJECT_SIZE];
	int height[OBJECT_SIZE];
	int color[OBJECT_SIZE];

	int temp_length[OBJECT_SIZE];

	int size;
	int draw[OBJECT_SIZE];

	int dec[OBJECT_SIZE];
}object;

typedef struct master{
	object* o;
	object* co;
	object* box_3;
	object* box_1;
	object* spikes;
	object* flag;

	level* l;
	charac* c;
	audio_buffer* ab;
}master;

master* master_init();

void object_spawn_init(master *mstr);

void object_spawn_interrupt( void* context );

void init_obj(object* o);
void up_obj(object* o, level* l);

void game_start(master* m, bmp* bg, char* level_to_read, char* bg_music);
void game_reset(master* m);

#endif /* OBJECT_SPAWNER_H_ */


