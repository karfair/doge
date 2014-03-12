/*
 * col.c
 *
 *  Created on: 2014-02-02
 *      Author: Phil
 */

#include "col.h"

int d_counter = 0;

bool check_collision(charac *c){
	return c->collide;
}

void init_char_update(master* mstr) {

	mstr->c->x = 50;
	mstr->c->y = 150;
	mstr->c->width = 39;
	mstr->c->height = 31;
	mstr->c->collide = false;
	mstr->c->win = false;
	mstr->c->jumping = false;
	mstr->c->grounded = false;
	mstr->c->jump_pressed = false;
	mstr->c->points = 0;
	//mstr->c->jump_released = false;
	mstr->c->ducking = false;
	mstr->c->color = 0;

	d_counter = 0;

	IOWR_16DIRECT(TIMER_2_BASE, 8, 500000 & 0xFFFF); //writes the period to the hardware timer
	IOWR_16DIRECT(TIMER_2_BASE, 12, 500000 >> 16);
	IOWR_16DIRECT(TIMER_2_BASE, 4, 1 << 3); //stop timer

	alt_irq_register(TIMER_2_IRQ,mstr,(void*)char_update_isr);//registers function to a specific IRQ

	IOWR_16DIRECT(TIMER_2_BASE, 4, 0x5);	//starts timer with interrupt
}


void char_update_isr(void *context) {

	IOWR_16DIRECT(TIMER_2_BASE,0,0); //needed to show that interrupt finished executing

	master* m = (master*)context;
	charac* c = m->c;
	object* o = m->o;
	object* co = m->co;
	object* spikes = m->spikes;
	object* box_1 = m->box_1;
	object* box_3 = m->box_3;
	object* flag = m->flag;
	//object* gates = m->color_gates;

	//printf("in interrupt collide: %i\n", c->collide);

	//dy--;
	//m->c->y + dy;

	//if( c->y == 239 ) c->collide = true; //check if fell off screen
	if(d_counter <= 300){
		if (d_counter%10 == 0)
		c->y += 1;
		else;
		d_counter++;
		IOWR_16DIRECT(TIMER_2_BASE, 4, 0x5); //restart timer
		return;
	}


	if(c->ducking == false){

			c->height = 31;
	}
	//temp variables
	int i;


	//obstacle collision
	for(i = 0; i < OBJECT_SIZE; i++) {//checks all boxes
		if(c->x >= o->x[i] && c->y-2 >= o->y[i] && c->y-c->height+10 <= o->y[i]+o->height[i] &&
			c->x - c->width + 10 <= o->x[i] + o->temp_length[i] && o->temp_length[i] != -1)
		{
			c->collide = true;
			break;
		}
	}
	if(c->collide) return;

	//box3 collision
		for(i = 0; i < OBJECT_SIZE; i++) {//checks all boxes
			if(c->x >= box_3->x[i] && c->y-2 >= box_3->y[i] && c->y-c->height+10 <= box_3->y[i]+box_3->height[i] &&
				c->x - c->width + 10 <= box_3->x[i] + box_3->temp_length[i] && box_3->temp_length[i] != -1)
			{
				c->collide = true;
				break;
			}
		}
		if(c->collide) return;

		//box1 collision
				for(i = 0; i < OBJECT_SIZE; i++) {//checks all boxes
					if(c->x >= box_1->x[i] && c->y-2 >= box_1->y[i] && c->y-c->height+10 <= box_1->y[i]+box_1->height[i] &&
						c->x - c->width + 10 <= box_1->x[i] + box_1->temp_length[i] && box_1->temp_length[i] != -1)
					{
						c->collide = true;
						break;
					}
				}
				if(c->collide) return;

	//spike collision
	for(i = 0; i < OBJECT_SIZE; i++) {//checks all boxes
		if(c->x >= spikes->x[i] && c->y-2 >= spikes->y[i] && c->y-c->height+10 <= spikes->y[i]+spikes->height[i] &&
			c->x - c->width + 10 <= spikes->x[i] + spikes->temp_length[i] && spikes->temp_length[i] != -1)
		{
			c->collide = true;
			break;
		}
	}
	if(c->collide) return;

	//gate collision
//	for(i = 0; i < OBJECT_SIZE; i++) {//checks all boxes
//		if(c->x >= gates->x[i] && c->y >= gates->y[i] && c->y-c->height <= gates->y[i]+gates->height[i] &&
//					c->x - c->width <= gates->x[i] + gates->temp_length[i] && gates->color[i] != c->color){
//			c->collide = true;
//			break;
//		}
//	}
//	if(c->collide) return;

	//coin collision
	for(i = 0; i < OBJECT_SIZE; i++) {//checks all coins
		if(c->x >= co->x[i] && c->y >= co->y[i] && c->y-c->height <= co->y[i]+ co->height[i] && co->temp_length[i] != -1 &&
			c->x - c->width <= co->x[i] + co->temp_length[i])
		{
			m->ab->sfx_flag1 = true;
			m->ab->sfx_count1 = 0;
			c->points++;
			co->temp_length[i] = -1;
			co->x[i] = 340;

			//printf("points:%i\n", c->points);
			break;
		}
	}
	//flag collision
	if(c->x >= flag->x[0] && c->y >= flag->y[0] && c->y - c->height <= flag->y[0] + flag->height[0] && flag->temp_length[0] != -1 &&
		c->x - c->width <= flag->x[0] + flag->temp_length[0])
	{
		c->points+=5;
		c->win = true;

		printf("points:%i\n", c->points);
		return;
	}

	//check if already jumping
	if(c->jump_pressed && !c->jumping && c->grounded){
		//printf("jumping: %i %i %i %i \n", c->jump_pressed, c->jumping, c->j_counter, c->grounded);
		c->jump_pressed = false;
		c->jumping = true;
		c->j_counter = 0;
		c->grounded = false;
		c->f_counter = 0;

		m->ab->sfx_flag = true;
		m->ab->sfx_count = 0;
		//c->jump_released = false;
	}

	//jumping mechanics
	if(c->jumping) {
		if(c->j_counter < 25){
			m->c->y-=2;
			c->j_counter++;
			c->jump_pressed = false;
		}
		else if(c->j_counter < 40){
			m->c->y-=1;
			c->j_counter++;
			c->jump_pressed = false;
		}
		else{
			c->jumping = false;
			c->jump_pressed = false;
		}
	}
	else { //if not, he's falling
		//if(c->grounded) goto A;
		if(c->y < 239)
			c->jump_pressed = false;
			//c->jump_released = true;
			if(c->f_counter < 25){
			c->y+=1;
			c->f_counter++;
			}
			else{
			c->y+=2;
			c->f_counter++;
			}
		if(c->y+4 >= 239){
			c->collide=true;
			return;
		}
		for(i = 0; i < OBJECT_SIZE; i++) { //checks all boxes
			if( c->x >= o->x[i] && c->y  >= o->y[i] &&c->y < o->y[i]+2 && o->temp_length[i] != -1 &&
				c->x - c->width  <= o->x[i] + o->temp_length[i] && (c->color == o->color[i] || o->color[i] == -1)) {
				c->grounded = true;
				c->y = o->y[i] - 2; //if he went into a box, move him up
				break;
			}
		}
		for(i = 0; i < OBJECT_SIZE; i++) { //checks all boxes
			if( c->x >= box_1->x[i] && c->y  >= box_1->y[i] &&c->y < box_1->y[i]+2 && box_1->temp_length[i] != -1 &&
				c->x - c->width  <= box_1->x[i] + box_1->temp_length[i]) {
				c->grounded = true;
				c->y = box_1->y[i] - 2; //if he went into a box, move him up
				break;
				}
		}
	}
	IOWR_16DIRECT(TIMER_2_BASE, 4, 0x5); //restart timer

	return;
}
