/*
 * bomb.h
 *
 *  Created on: 2014-03-19
 *      Author: Phil
 */

#ifndef BOMB_H_
#define BOMB_H_

typedef struct{
	unsigned char p0name[32];
	unsigned char p1name[32];
	unsigned char p2name[32];
	unsigned char p3name[32];

	unsigned char size0,size1,size2,size3;
	unsigned char pid0, pid1, pid2, pid3;
} players;

extern players p;
extern int set_name_i;

void setName(unsigned char id, unsigned char size, unsigned char *name);
void sendNameAll();

#endif /* BOMB_H_ */
