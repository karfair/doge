//draw.h
#ifndef draw_H_
#define draw_H_

#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"
#include "sd_audio.h"

#define pixel_buffer_addr1 PIXEL_BUFFER_BASE
#define pixel_buffer_addr2 PIXEL_BUFFER_BASE + PIXEL_BUFFER_SPAN/2 //320*240*2

extern alt_up_pixel_buffer_dma_dev* pixel_buffer;

typedef struct
{
	int x[4];
	int y[4];
	int length[4];
	int height[4];
}draw;

void draw_object(alt_up_pixel_buffer_dma_dev *pixel_buffer, draw object, bmp* b, int num);


#endif


