#include "draw.h"
#include "io.h"

alt_up_pixel_buffer_dma_dev* pixel_buffer;



void draw_object(alt_up_pixel_buffer_dma_dev *pixel_buffer, draw object, bmp* b, int num) {
	unsigned int i,j,k,l;
	unsigned int addr = pixel_buffer->back_buffer_start_address;
	unsigned int limit_x = pixel_buffer->x_resolution;
	short int** pic = b->pic;
	short int temp;


	limit_x = limit_x << 1;
	addr += object.y[num] * (limit_x);

	for(i = object.y[num], k = 0; i < object.y[num] + b->y; i++, k++) {
		for (j = object.x[num], l = 0; j < object.length[num]+ object.x[num]; j++, l++)
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

void clear_object(alt_up_pixel_buffer_dma_dev *pixel_buffer, draw box, bmp* b, int num)
{

	clear_loc(pixel_buffer, box.x[num], box.y[num], box.x[num] + box.length[num], box.y[num]+box.height[num],b);

}





