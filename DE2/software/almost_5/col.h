/*
 * col.h
 *
 *  Created on: 2014-02-02
 *      Author: Phil
 */

#ifndef COL_H_
#define COL_H_

#include "object_spawner.h"
#include "sd_audio.h"

bool check_collision(charac *c  );
void init_char_update(master* mstr );
void char_update_isr(void *context);

#endif /* COL_H_ */
